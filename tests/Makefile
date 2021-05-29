CC=	  gcc
OBJS=	  main.o fifo.o ac.o input.o

SRC=	  ../src/
INCLUDE=  $(SRC)include/
TARGET=   test_bin/
CFLAGS=   -I $(INCLUDE) -c -fsigned-char

# Since different tests needs dirrefent outputs from the scs binary, there
# is several different binaries compiled with differend macros defined.

# Test1 related variables. Test set 1 is for testing the correctness of the
# generated superstring. E.g to test that every keyword in every instance
# is a substring of the resulted superstring.

TEST1_BIN=	$(TARGET)test1
TEST1_ODIR=	test1_objs/
TEST1_OBJ=	$(addprefix $(TEST1_ODIR), $(OBJS))
TEST1_INIT=	$(TEST1_DIR)/.INIT
TEST1_DIR=	correct_superstring_tests
TEST1_FILECOUNT = 2
TEST1_FLAGS=	-DMAX_LINE=3335 -DTEST1


# Test2 related variables. Test set 2 is for testing the correctness of the
# compression aquired from the greedy heuristic. The compression should be
# at least half of the optimal. This is tested with only binary alphabet,
# since randomly generated instances has more overlaps with small alphabets.

TEST2_BIN=	$(TARGET)test2
TEST2_ODIR=	test2_objs/
TEST2_OBJ=	$(addprefix $(TEST2_ODIR), $(OBJS))
TEST2_INIT=	$(TEST2_DIR)/.INIT
TEST2_DIR=	compression_tests
TEST2_FILECOUNT = 10
TEST2_FLAGS=	-DTEST2

# By default, run all tests
all: test1 test2


# Compile object files from the main source files (from $(SRC) with the same names
# Compiles with options xxx_FLAGS, where xxx is the name of the test set
.SECONDEXPANSION:
%.o: $(SRC)$$(notdir $$(basename $$@)).c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $($(shell echo '$(patsubst %_objs/,%,$(dir $@))' | tr  '[:lower:]' '[:upper:]')_FLAGS) $< -o $@

# Target directory for test binaries
$(TARGET):
	mkdir $(TARGET)


#############################################
## Test set 1 related files and operations ##
#############################################

.PHONY: test1
test1: $(TEST1_INIT) $(TEST1_BIN)
	./test1.sh $(TEST1_DIR) $(TEST1_BIN)

$(TEST1_INIT): | $(TEST1_DIR)
	@echo "Creating $(TEST1_FILECOUNT) * 6 = $$(($(TEST1_FILECOUNT)*6)) test files for each alphabet type (binary, dna, hex and alnum)..."
	for alphabet in "binary" "dna" "hex" "alnum" ; do \
		./create_instance.sh $$alphabet 3333 10    $(TEST1_FILECOUNT) $(TEST1_DIR); \
		./create_instance.sh $$alphabet 1000 33   $(TEST1_FILECOUNT) $(TEST1_DIR); \
		./create_instance.sh $$alphabet 333  100   $(TEST1_FILECOUNT) $(TEST1_DIR); \
		./create_instance.sh $$alphabet 100  333  $(TEST1_FILECOUNT) $(TEST1_DIR); \
		./create_instance.sh $$alphabet 33   1000  $(TEST1_FILECOUNT) $(TEST1_DIR); \
		./create_instance.sh $$alphabet 10   3333 $(TEST1_FILECOUNT) $(TEST1_DIR); \
	done 
	touch $@


$(TEST1_BIN): $(TEST1_OBJ) | $(TARGET)
	$(CC) -o $(TEST1_BIN) $(TEST1_OBJ)

$(TEST1_DIR):
	mkdir $@


#############################################
## Test set 2 related files and operations ##
#############################################

.PHONY: test2
test2: $(TEST2_INIT) $(TEST2_BIN)
	./test2.sh $(TEST2_DIR) $(TEST2_BIN)

$(TEST2_INIT): | $(TEST2_DIR)
	@echo "Creating $(TEST2_FILECOUNT) * 2 = $$(($(TEST2_FILECOUNT)*3)) small instances with binary alphabet..."
	./create_instance.sh binary 5 5 $(TEST2_FILECOUNT) $(TEST2_DIR)
	./create_instance.sh binary 7 7 $(TEST2_FILECOUNT) $(TEST2_DIR)
	touch $@

$(TEST2_BIN): $(TEST2_OBJ) | $(TARGET)
	$(CC) -o $(TEST2_BIN) $(TEST2_OBJ)

$(TEST2_DIR):
	mkdir $@


## PHONY targets to clean up the test folder ##
.PHONY: clean clobber delete_tests

clean:
	-rm -rf $(TEST1_ODIR) $(TEST2_ODIR)

clobber: clean
	-rm -rf $(TARGET)

delete_tests:
	-rm -r $(TEST1_DIR) $(TEST2_DIR)

