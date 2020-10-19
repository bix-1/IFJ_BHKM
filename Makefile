# Makefile for compiler implementation of imperative programing language IFJ20
#
# Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
#          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
#          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
#          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
#

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

all: codegen_test
# TODO : temp. Makefile default - change from test to final

codegen.o: codegen.c codegen.h
	${CC} ${CFLAGS} -c codegen.c -o codegen.o

# Testing
codegen_test: codegen_test.c codegen.o
	${CC} ${CFLAGS} codegen_test.c codegen.o -o codegen_test

clean:
	rm -f *.o codegen_test
