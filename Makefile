# Makefile for compiler implementation of imperative programing language IFJ20
#
# Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
#          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
#          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
#          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
#


CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
TMPS = *.o *.a test_codegen test_ll test_error
AR = ar -csr


all: test_codegen
# TODO : temp. Makefile default - change from test to final

########## Static libraries ##########
scanner.a: scanner.o error.o str.o
	${AR} $@ $^

error.a: error.o ll.o symtable.o scanner.a
	${AR} -T $@ $^

ll.a: ll.o error.a
	${AR} -T $@ $^

symtable.a: symtable.o
	${AR} -T $@ $^

#------ end of Static libraries -----#

# Rule for __all__ object files
%.o: %.c %.h
	${CC} ${CFLAGS} -c $<

########## Testing ##########
tests: test_codegen test_ll test_error
	-./test_error
	./test_codegen
	./test_ll

test_codegen: tests/test_codegen.c
	${CC} ${CFLAGS} $^ -o $@

test_ll: tests/test_ll.c ll.a
	${CC} ${CFLANG} $^ -o $@

test_error: tests/test_error.c error.a ll.a symtable.a
	${CC} ${CFLAGS} $^ -o $@
#------ end of Testing -----#

clean:
	rm -f ${TMPS}
