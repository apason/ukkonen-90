SRC=	  ../src/
TARGET=   test_bin/

# By default, run all tests
all: test1 test2 test3

# Includes the contents of test and non-test makefiles that are the same
include ../common.mk

CFLAGS=   $(INCLUDE) -c -funsigned-char

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
TEST1_FLAGS=	-DMAX_LINE=3335 -DARRAY_GOTO -DSCS -DLONG_KEYS


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
TEST2_FLAGS=	-DTEST2 -DARRAY_GOTO -DSCS

# Test3 related variables. Test set 3 tests that the generated superstring is
# is exactly the same regardless of the goto function type. At this moment there
# are only two alternatives: array and red-black tree. This test set generates
# the superstrings with both variants and compares the output.
# This test set uses the same test files that are generated for test set 1 so
# test3 target depends on test1.

TEST3_BIN=	$(TARGET)test3
TEST3_ODIR=	test3_objs/
TEST3_OBJ=	$(addprefix $(TEST3_ODIR), $(OBJS))
TEST3_INIT=	$(TEST1_DIR)/.INIT
TEST3_DIR=	$(TEST1_DIR)
TEST3_FLAGS=	-DMAX_LINE=3335 -DRB_TREE_GOTO -DSCS -DLONG_KEYS




# Compile object files from the main source files (from $(SRC) with the same names
# Compiles with options xxx_FLAGS, where xxx is the name of the test set
.SECONDEXPANSION:
%_goto.o:$(GOTO_DIR)$$(notdir $$(basename $$@)).c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $($(shell echo '$(patsubst %_objs/,%,$(dir $@))' | tr  '[:lower:]' '[:upper:]')_FLAGS) $< -o $@

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
	@echo "Creating $(TEST2_FILECOUNT) * 3 = $$(($(TEST2_FILECOUNT)*3)) small instances with binary alphabet..."
	./create_instance.sh binary 5 5 $(TEST2_FILECOUNT) $(TEST2_DIR)
	./create_instance.sh binary 7 7 $(TEST2_FILECOUNT) $(TEST2_DIR)
#	./create_instance.sh binary 10 10 $(TEST2_FILECOUNT) $(TEST2_DIR)
	touch $@

$(TEST2_BIN): $(TEST2_OBJ) | $(TARGET)
	$(CC) -o $(TEST2_BIN) $(TEST2_OBJ)

$(TEST2_DIR):
	mkdir $@

#############################################
## Test set 3 related files and operations ##
#############################################

.PHONY: test3
test3: $(TEST3_INIT) $(TEST3_BIN) $(TEST1_BIN)
	./test3.sh $(TEST1_BIN) $(TEST3_BIN) $(TEST3_DIR)

$(TEST3_BIN): $(TEST3_OBJ) | $(TARGET)
	$(CC) -o $(TEST3_BIN) $(TEST3_OBJ)

## PHONY targets to clean up the test folder ##
.PHONY: clean clobber delete_tests

clean:
	-rm -rf $(TEST1_ODIR) $(TEST2_ODIR) $(TEST3_ODIR)

clobber: clean
	-rm -rf $(TARGET)

delete_tests:
	-rm -r $(TEST1_DIR) $(TEST2_DIR) $(TEST3_DIR)

