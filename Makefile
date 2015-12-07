CC = gcc

#CFLAGS = -W -Wall -g
CFLAGS = -W -Wall -O3 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -g
LDFLAGS = -lm -lrt -laio
#MYSQL_INCLUDE=$(shell mysql_config --include)
MYSQL_INCLUDE=

OBJS = innodb_page_inspect.o red_black_tree.o stack.o misc.o

PROGRAM = innodb_page_inspect

all: $(PROGRAM)

$(PROGRAM):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROGRAM)

innodb_page_inspect.o: innodb_page_inspect.c red_black_tree.h

red_black_tree.o: red_black_tree.h stack.h red_black_tree.c stack.c misc.h misc.c

stack.o:    stack.c stack.h misc.h misc.c

clean:
	rm -f innodb_page_inspect *.o core 
