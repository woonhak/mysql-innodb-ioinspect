/*******************************************************************
 * Innodb page inspector
 *
 * Author : Woonhak Kang (woonagi319@gmail.com)
 *
 ******************************************************************/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <libaio.h>
#include <assert.h>

/*Time measuring */
#include<time.h>
#include<sys/time.h>
#include<assert.h>

/*Uniform random generator*/
#include"innodb_page_inspect.h"		/*file page header data from innodb*/


#define BLCKSZ 			8192L			//block size 8KB
#define SECTOR_SIZE	512
#define SECTORS_PER_BLOCK	(BLCKSZ/SECTOR_SIZE)
#define BUFFER_SIZE (BLCKSZ) 
#define SECTOR_ALIGN SECTOR_SIZE

/*********************************
	 This program specific defines 
 **********************************/
/*for debug */
#define DEBUG 1

#define log_debug(fmt, ...) \
	do { if(DEBUG) \
		fprintf(stdout, "DEBUG : %s:%d:%s(): " fmt, __FILE__, \
			__LINE__, __func__, ##__VA_ARGS__); } while (0);

#define log_info(fmt, ...) \
	do { fprintf(stderr, "INFO : %s:%d:%s(): " fmt, __FILE__, \
			__LINE__, __func__, ##__VA_ARGS__); } while (0);

/*********************************
	 GLOBAL variables 
 **********************************/

/*store target device name (or file name) */
char device_path[256];
char log_path[256];

unsigned fil_type_count[FIL_TYPE_END+1];

/*********************************//*
print_fil_type_count:	 
prit current count numbers
*/
void print_fil_type_count(void)
{
	int i;
	/*init fil_type_count */
	for(i=0;i<FIL_TYPE_END+1;i++)
		fprintf(stdout, "%s:%u\n",fil_type_names[i],fil_type_count[i]);
}

/*********************************//*
check_blocks:	 read start sector number and size and
read that block from the target device and decode file page type*/
void check_blocks(
		int fd, /*<-: target device file desc.*/
		FILE *infile) /*<-: sector,size list file */
{
	int i=0;
	char *buf;                // aligned buffer
	int sect_num=0, size=0;

	log_debug("enter\n");

	/*init fil_type_count */
	for(i=0;i<FIL_TYPE_END+1;i++)
		fil_type_count[i]=0;

	/*FIXME:in Linux, minimum aligned size is sector, 
		If you want to use another align size, then change alignment size*/
	assert(!posix_memalign((void**) &buf, SECTOR_ALIGN, BUFFER_SIZE));

	fseek(infile, SEEK_SET, 0);

	while(fscanf(infile, "%d,%d\n", &sect_num, &size) != EOF){
		//fprintf(stdout, "%d line - sect_num:%d, size:%d\n",i++, sect_num, size);

#if 0
		/*check alignment, this is not always correct*/
		if(sect_num %(BLCKSZ/SECTOR_SIZE) != 0){
			log_info("Not aligned sect_num :%d\n", sect_num);
		}
		else
#endif	
		{

			/*if current size is not the same as block size,
				then skip it*/
			if(size != (BLCKSZ/SECTOR_SIZE))
				continue;

			off64_t offset = (off64_t)sect_num*SECTOR_SIZE;
			/*read a sector data info buffer and check it's header*/
			size_t ret=pread64(fd, buf, BLCKSZ, offset);

			if(ret != BLCKSZ)
				log_debug("block read error offset : %lu\n", offset);
			
			ulint fil_page_type = mach_read_from_2( (byte *)buf+FIL_PAGE_TYPE) ;

			switch(fil_page_type)
			{
				case FIL_PAGE_INDEX:
					fil_type_count[INDEX]++;
					log_debug("PAGE INFO : FIL_PAGE_INDEX\n");
				break;
				
				case FIL_PAGE_INODE:
					fil_type_count[INODE]++;
					log_debug("PAGE INFO : INODE\n");
				break;
				
				case FIL_PAGE_UNDO_LOG:
					fil_type_count[UNDO_LOG]++;
					log_debug("PAGE INFO : UNDO_LOG\n");
				break;

				case FIL_PAGE_IBUF_FREE_LIST : /*!< Insert buffer free list */
					fil_type_count[IBUF_FREE_LIST]++;
					log_debug( "PAGE INFO : FIL_PAGE_IBUF_FREE_LIST\n");
				break;
			
				case FIL_PAGE_TYPE_ALLOCATED : /*!< Freshly allocated page */
					fil_type_count[ALLOCATED]++;
					log_debug( "PAGE INFO : FIL_PAGE_IBUF_FREE_LIST\n");
				break;

				case FIL_PAGE_IBUF_BITMAP  : /*!< Insert buffer bitmap */
					fil_type_count[BITMAP]++;
					log_debug( "PAGE INFO : FIL_PAGE_IBUF_BITMAP\n");
				break;

				case FIL_PAGE_TYPE_SYS : /*!< System page */
					fil_type_count[SYS]++;
					log_debug( "PAGE INFO : FIL_PAGE_SYSTEM\n");
				break;

				case FIL_PAGE_TYPE_TRX_SYS : /*!< Transaction system data */
					fil_type_count[TRX_SYS]++;
					log_debug( "PAGE INFO : FIL_PAGE_TRX_SYS\n");
				break;

				case FIL_PAGE_TYPE_FSP_HDR : /*!< File space header */
					fil_type_count[FSP_HDR]++;
					log_debug( "PAGE INFO : FIL_PAGE_FSP_HDR\n");
				break;

				case FIL_PAGE_TYPE_XDES  : /*!< Extent descriptor page */
					fil_type_count[XDES]++;
					log_debug( "PAGE INFO : FIL_PAGE_XDES\n");
				break;

				case FIL_PAGE_TYPE_BLOB  :  /*!< Uncompressed BLOB page */
					fil_type_count[BLOB]++;
					log_debug( "PAGE INFO : FIL_PAGE_BLOB\n");
				break;

				case FIL_PAGE_TYPE_ZBLOB :  /*!< First compressed BLOB page */
					fil_type_count[ZBLOB]++;
					log_debug( "PAGE INFO : FIL_PAGE_ZBLOB\n");
				break;

				case FIL_PAGE_TYPE_ZBLOB2  :  /*!< Subsequent compressed BLOB page */
					fil_type_count[ZBLOB2]++;
					log_debug( "PAGE INFO : FIL_PAGE_ZBLOB2\n");
				break;

				default :
					fil_type_count[FIL_TYPE_END]++;
					log_info("PAGE_INFO : OTHER\n");
			}
//			log_debug("read done\n");
		}
	}

	free(buf);
}

int main(int argc, char *argv[])
{
	int fd;
	FILE *logfile;

	/*add test program input arguments */
	if(argc != 3){ 
		fprintf(stderr, "io_test : This program requires device path\n"
		 	              "program : for ex) ./io_test /dev/sdb input_filename\n\n");
		return -1; 
	}

	/*device path should not be NULL */
	assert(argv[1]);

	/*Get the device name */
	memset((void *) device_path, 0x00, sizeof(char)*256);    

	/*copy device path and put terminal character*/
	strncpy(device_path, argv[1], strlen(argv[1]));
	device_path[strlen(argv[1])] = '\0';

	log_info("Selected device name - %s\n", device_path);
	log_info("Phase 0 - Open target device file\n");

	if( access( device_path, F_OK ) != -1 ) {
		// file exists
		log_debug("Open fd: %d\n", (int)(fd=open64(device_path, O_RDONLY|O_DIRECT)));

		if(fd < 0){
			log_debug("File open error\n");
			exit(-1);
		}
	} 
	else {
		log_debug("Device file not exists error\n");
		exit(-1);
	}

	/*device path should not be NULL */
	assert(argv[2]);

	/*Get the device name */
	memset((void *) log_path, 0x00, sizeof(char)*256);    

	/*copy device path and put terminal character*/
	strncpy(log_path, argv[2], strlen(argv[2]));
	log_path[strlen(argv[2])] = '\0';


	log_info("Selected log file name - %s\n", log_path);
	log_info("Phase 1 - Open log file\n");

	if( access(log_path, F_OK ) != -1 ) {
		// file exists
		logfile=fopen(log_path, "r");

		assert(logfile != NULL);
		if(logfile == NULL){
			log_debug("File open error\n");
			exit(-1);
		}
	}
	else {
		log_debug("File Not exists error\n");
		exit(-1);
	}

	check_blocks(fd, logfile);

	close(fd);
	fclose(logfile);

	print_fil_type_count();

	return 0;
}
