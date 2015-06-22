CC = gcc
#CFLAGS = -W -Wall -g
CFLAGS = -W -Wall -O3 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -g
LDFLAGS = -lm -lrt -laio
#MYSQL_INCLUDE=$(shell mysql_config --include)
MYSQL_INCLUDE=

all: innodb_page_inspect

#innodb_page_inspect: innodb_page_inspect.o uniform.o pmc.o 
innodb_page_inspect: innodb_page_inspect.o
	$(CC) $^ -o $@  $(LDFLAGS)

innodb_page_inspect.o: innodb_page_inspect.c
	$(CC) $(CFLAGS) $(MYSQL_INCLUDE) -c -o $@ $^

clean:
	rm -f innodb_page_inspect *.o core 
