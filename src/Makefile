SRC= ./
TARGET=   ../target/

include ../common.mk

# DEBUG variable enables DEBUG and INFO macros
ifdef DEBUG
DEBUG_FLAGS = -Wunused-variable -DDEBUG -DINFO
endif

# INFO variable enables INFO macro
ifdef INFO
INFO_FLAGS = -DINFO
endif

# DEFS is for appending CFLAGS from shell.
CFLAGS=   $(DEFS) $(INCLUDE) -c -funsigned-char -Wno-char-subscripts -Wall -Werror -Wno-unused-variable $(DEBUG_FLAGS) $(INFO_FLAGS) -O3 

.IGNORE: clean clobber
.DELETE_ON_ERROR: ac

.SECONDEXPANSION:
%_goto.o: $(GOTO_DIR)$$(notdir $$(basename $$@)).c
	$(CC) $(CFLAGS) goto/$(basename $@).c
.c.o:
	$(CC) $(CFLAGS) $<

all:	$(TARGET)scs

$(TARGET)scs:	$(OBJS) | $(TARGET)
	$(CC) $(OBJS) -o $(TARGET)scs

$(TARGET):
	mkdir $(TARGET)

clean:
	rm -f *.o

clobber:	clean
	rm $(TARGET)scs
