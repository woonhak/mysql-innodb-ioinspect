/*****************************************************************************
Author : woonhak.kang (woonagi319@gmail.com)

This code is from innodb source to inspect page type.
*****************************************************************************/

#ifndef innodb_page_inspect_h
#define innodb_page_inspect_h

#include <stdint.h>

#define UNIV_INLINE inline
typedef unsigned int ulint;
typedef uint8_t byte;

#define FIL_ADDR_PAGE	0	/* first in address is the page offset */
#define	FIL_ADDR_BYTE	4	/* then comes 2-byte byte offset within page*/
#define	FIL_ADDR_SIZE	6	/* address size is 6 bytes */

/** The byte offsets on a file page for various variables @{ */
#define FIL_PAGE_SPACE_OR_CHKSUM 0	/*!< in < MySQL-4.0.14 space id the
					page belongs to (== 0) but in later
					versions the 'new' checksum of the
					page */
#define FIL_PAGE_OFFSET		4	/*!< page offset inside space */
#define FIL_PAGE_PREV		8	/*!< if there is a 'natural'
					predecessor of the page, its
					offset.  Otherwise FIL_NULL.
					This field is not set on BLOB
					pages, which are stored as a
					singly-linked list.  See also
					FIL_PAGE_NEXT. */
#define FIL_PAGE_NEXT		12	/*!< if there is a 'natural' successor
					of the page, its offset.
					Otherwise FIL_NULL.
					B-tree index pages
					(FIL_PAGE_TYPE contains FIL_PAGE_INDEX)
					on the same PAGE_LEVEL are maintained
					as a doubly linked list via
					FIL_PAGE_PREV and FIL_PAGE_NEXT
					in the collation order of the
					smallest user record on each page. */
#define FIL_PAGE_LSN		16	/*!< lsn of the end of the newest
					modification log record to the page */
#define	FIL_PAGE_TYPE		24	/*!< file page type: FIL_PAGE_INDEX,...,
					2 bytes.

					The contents of this field can only
					be trusted in the following case:
					if the page is an uncompressed
					B-tree index page, then it is
					guaranteed that the value is
					FIL_PAGE_INDEX.
					The opposite does not hold.

					In tablespaces created by
					MySQL/InnoDB 5.1.7 or later, the
					contents of this field is valid
					for all uncompressed pages. */
#define FIL_PAGE_FILE_FLUSH_LSN	26	/*!< this is only defined for the
					first page in a system tablespace
					data file (ibdata*, not *.ibd):
					the file has been flushed to disk
					at least up to this lsn */
#define FIL_PAGE_ARCH_LOG_NO_OR_SPACE_ID  34 /*!< starting from 4.1.x this
					contains the space id of the page */
#define FIL_PAGE_DATA		38	/*!< start of the data on the page */
#define FIL_PAGE_DATA_ALIGN_32	40
/* @} */
/** File page trailer @{ */
#define FIL_PAGE_END_LSN_OLD_CHKSUM 8	/*!< the low 4 bytes of this are used
					to store the page checksum, the
					last 4 bytes should be identical
					to the last 4 bytes of FIL_PAGE_LSN */
#define FIL_PAGE_DATA_END	8	/*!< size of the page trailer */
/* @} */

/** File page types (values of FIL_PAGE_TYPE) @{ */
#define FIL_PAGE_INDEX		17855	/*!< B-tree node */
#define FIL_PAGE_UNDO_LOG	2	/*!< Undo log page */
#define FIL_PAGE_INODE		3	/*!< Index node */
#define FIL_PAGE_IBUF_FREE_LIST	4	/*!< Insert buffer free list */
/* File page types introduced in MySQL/InnoDB 5.1.7 */
#define FIL_PAGE_TYPE_ALLOCATED	0	/*!< Freshly allocated page */
#define FIL_PAGE_IBUF_BITMAP	5	/*!< Insert buffer bitmap */
#define FIL_PAGE_TYPE_SYS	6	/*!< System page */
#define FIL_PAGE_TYPE_TRX_SYS	7	/*!< Transaction system data */
#define FIL_PAGE_TYPE_FSP_HDR	8	/*!< File space header */
#define FIL_PAGE_TYPE_XDES	9	/*!< Extent descriptor page */
#define FIL_PAGE_TYPE_BLOB	10	/*!< Uncompressed BLOB page */
#define FIL_PAGE_TYPE_ZBLOB	11	/*!< First compressed BLOB page */
#define FIL_PAGE_TYPE_ZBLOB2	12	/*!< Subsequent compressed BLOB page */
#define FIL_PAGE_TYPE_LAST	FIL_PAGE_TYPE_ZBLOB2
					/*!< Last page type */
/* @} */

/** Change buffer B-tree page */
#define I_S_PAGE_TYPE_IBUF    (FIL_PAGE_TYPE_LAST + 1)

/** Any states greater than I_S_PAGE_TYPE_IBUF would be treated as
	unknown. */
#define I_S_PAGE_TYPE_UNKNOWN   (I_S_PAGE_TYPE_IBUF + 1)

/** We also define I_S_PAGE_TYPE_INDEX as the Index Page's position
	in i_s_page_type[] array */
#define I_S_PAGE_TYPE_INDEX   1

#if 0
/** Name string for File Page Types */
static buf_page_desc_t  i_s_page_type[] = {
	{"ALLOCATED", FIL_PAGE_TYPE_ALLOCATED},
	{"INDEX", FIL_PAGE_INDEX},
	{"UNDO_LOG", FIL_PAGE_UNDO_LOG},
	{"INODE", FIL_PAGE_INODE},
	{"IBUF_FREE_LIST", FIL_PAGE_IBUF_FREE_LIST},
	{"IBUF_BITMAP", FIL_PAGE_IBUF_BITMAP},
	{"SYSTEM", FIL_PAGE_TYPE_SYS},
	{"TRX_SYSTEM", FIL_PAGE_TYPE_TRX_SYS},
	{"FILE_SPACE_HEADER", FIL_PAGE_TYPE_FSP_HDR},
	{"EXTENT_DESCRIPTOR", FIL_PAGE_TYPE_XDES},
	{"BLOB", FIL_PAGE_TYPE_BLOB},
	{"COMPRESSED_BLOB", FIL_PAGE_TYPE_ZBLOB},
	{"COMPRESSED_BLOB2", FIL_PAGE_TYPE_ZBLOB2},
	{"IBUF_INDEX", I_S_PAGE_TYPE_IBUF},
	{"UNKNOWN", I_S_PAGE_TYPE_UNKNOWN}
};                                                                 
#endif

/*enumerator for fil page type*/
enum fil_page_types
{
INDEX = 0,	/*!< B-tree node */
UNDO_LOG  ,		/*!< Undo log page */
INODE		 ,	/*!< Index node */
IBUF_FREE_LIST,		/*!< Insert buffer free list */
ALLOCATED,		/*!< Freshly allocated page */
BITMAP,		/*!< Insert buffer bitmap */
SYS,		/*!< System page */
TRX_SYS,		/*!< Transaction system data */
FSP_HDR,		/*!< File space header */
XDES,		/*!< Extent descriptor page */
BLOB,		/*!< Uncompressed BLOB page */
ZBLOB,		/*!< First compressed BLOB page */
ZBLOB2,		/*!< Subsequent compressed BLOB page */
IBUF_INDEX,
UNKNOWN,
FIL_TYPE_END
};

char fil_type_names[FIL_TYPE_END+1][16]=
{
"INDEX",	/*!< B-tree node */
"UNDO_LOG",		/*!< Undo log page */
"INODE",	/*!< Index node */
"IBUF_FREE_LIST",		/*!< Insert buffer free list */
"ALLOCATED",		/*!< Freshly allocated page */
"BITMAP",		/*!< Insert buffer bitmap */
"SYS",		/*!< System page */
"TRX_SYS",		/*!< Transaction system data */
"FSP_HDR",		/*!< File space header */
"XDES",		/*!< Extent descriptor page */
"BLOB",		/*!< Uncompressed BLOB page */
"ZBLOB",		/*!< First compressed BLOB page */
"ZBLOB2",		/*!< Subsequent compressed BLOB page */
"IBUF_INDEX",
"UNKNOWN",
"TYPE_END"
};
/********************************************************//**
The following function is used to fetch data from 2 consecutive
bytes. The most significant byte is at the lowest address.
@return ulint integer */
UNIV_INLINE
ulint
mach_read_from_2(
		/*=============*/
		  const byte* b)  /*!< in: pointer to 2 bytes */
{
	  return(((ulint)(b[0]) << 8) | (ulint)(b[1]));
}


#endif
