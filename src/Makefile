SRC= ./
TARGET=   ../target/

include ../common.mk

# INFO variable enables INFO macro
ifdef INFO
INFO_FLAGS = -DINFO
endif

# DEFS is for appending CFLAGS from shell.
CFLAGS=   $(DEFS) $(INCLUDE) -c -funsigned-char -Wno-char-subscripts -Wall -Werror -Wno-unused-variable $(INFO_FLAGS) -O3 -flto

.IGNORE: clean clobber
.DELETE_ON_ERROR: ac

.SECONDEXPANSION:
%_goto.o: $(GOTO_DIR)$$(notdir $$(basename $$@)).c
	$(CC) $(CFLAGS) goto/$(basename $@).c
.c.o:
	$(CC) $(CFLAGS) $<

all:	$(TARGET)scs

$(TARGET)scs:	$(OBJS) | $(TARGET)
	$(CC) -lm -flto $(OBJS) -o $(TARGET)scs

$(TARGET):
	mkdir $(TARGET)

clean:
	rm -f *.o

clobber:	clean
	rm $(TARGET)scs
