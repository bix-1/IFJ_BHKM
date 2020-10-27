# Makefile for compiler implementation of imperative programing language IFJ20
#
# Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
#          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
#          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
#          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
#

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
TMPS = *.o codegen_test test_ll test_error

all: codegen_test
# TODO : temp. Makefile default - change from test to final

codegen.o: codegen.c codegen.h
	${CC} ${CFLAGS} -c codegen.c -o codegen.o

ll.o: ll.c ll.h
	${CC} ${CFLAGS} -c $< -o $@

error.o: error.c error.h
	${CC} ${CFLAGS} -c $< -o $@

########## Testing ##########
codegen_test: codegen_test.c codegen.o
	${CC} ${CFLAGS} codegen_test.c codegen.o -o codegen_test

test_ll: tests/test_ll.c ll.o error.o
	${CC} ${CFLANG} $^ -o $@

test_error: tests/test_error.c error.o ll.o
	${CC} ${CFLAGS} $^ -o $@


#------ end of Testing -----#

clean:
	rm -f ${TMPS}
