# Makefile for compiler implementation of imperative programing language IFJ20
#
# Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
#          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
#          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
#          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
#

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
TMPS = *.o test_codegen test_ll test_error

all: test_codegen
# TODO : temp. Makefile default - change from test to final

codegen.o: codegen.c codegen.h
	${CC} ${CFLAGS} -c codegen.c -o codegen.o

ll.o: ll.c ll.h
	${CC} ${CFLAGS} -c $< -o $@

error.o: error.c error.h
	${CC} ${CFLAGS} -c $< -o $@

htab.o: htab.c htab.h
	${CC} ${CFLAGS} -c $< -o $@

########## Testing ##########
test_codegen: tests/test_codegen.c codegen.o
	${CC} ${CFLAGS} tests/test_codegen.c codegen.o -o test_codegen

test_ll: tests/test_ll.c ll.o error.o
	${CC} ${CFLANG} $^ -o $@

test_error: tests/test_error.c error.o ll.o htab.o
	${CC} ${CFLAGS} $^ -o $@


#------ end of Testing -----#

clean:
	rm -f ${TMPS}
