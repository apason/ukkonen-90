CC=	  gcc
OBJS=	  main.o fifo.o ac.o input.o
INCLUDE=  -I "include/"
TARGET=   ../target/

# DEBUG variable enables DEBUG and INFO macros
ifdef DEBUG
DEBUG_FLAGS = -Wunused-variable -DDEBUG -DINFO
endif

# INFO variable enables INFO macro
ifdef INFO
INFO_FLAGS = -DINFO
endif

# DEFS is for appending CFLAGS from shell.
CFLAGS=   $(DEFS) $(INCLUDE) -c -fsigned-char -Wno-char-subscripts -Wall -Werror -Wno-unused-variable $(DEBUG_FLAGS) $(INFO_FLAGS)

.IGNORE: clean clobber
.DELETE_ON_ERROR: ac

.c.o:
	$(CC) $(CFLAGS) $<

all:	$(TARGET)scs

main.o ac.o: include/ac.h
main.o ac.o fifo.o: include/fifo.h
main.o input.o: include/input.h

$(TARGET)scs:	$(OBJS) | $(TARGET)
	$(CC) $(OBJS) -o $(TARGET)scs

$(TARGET):
	mkdir $(TARGET)

clean:
	rm -f *.o

clobber:	clean
	rm $(TARGET)scs
