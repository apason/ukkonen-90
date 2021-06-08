CC=	  gcc
OBJS=	  main.o fifo.o ac.o input.o rb_tree.o usage.o array_goto.o rb_tree_goto.o

INCLUDE_DIR= $(SRC)include/
GOTO_DIR= $(SRC)goto/
INCLUDE = -I $(INCLUDE_DIR) -I $(GOTO_DIR)


main.o ac.o fifo.o rb_tree.o array_goto.o rb_tree_goto.o: $(INCLUDE_DIR)init.h
main.o ac.o array_goto.o rb_tree_goto.o: $(GOTO_DIR)goto.h
main.o ac.o rb_tree_goto.o: $(INCLUDE_DIR)ac.h
main.o ac.o fifo.o: $(INCLUDE_DIR)fifo.h
main.o rb_tree.o rb_tree_goto.o: $(INCLUDE_DIR)rb_tree.h
main.o ac.o input.o: $(INCLUDE_DIR)input.h
main.o ac.o usage.o: $(INCLUDE_DIR)usage.h

