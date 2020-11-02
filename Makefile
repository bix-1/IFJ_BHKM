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
	${CC} ${CFLAGS} -c $< -o $@

ll.o: ll.c ll.h
	${CC} ${CFLAGS} -c $< -o $@

error.o: error.c error.h
	${CC} ${CFLAGS} -c $< -o $@

symtable.o: symtable.c symtable.h
	${CC} ${CFLAGS} -c $< -o $@

scanner.o: scanner.c scanner.h str.o
	${CC} ${CFLAGS} -c scanner.c -o $@

str.o: str.c str.h
	${CC} ${CFLAGS} -c $< -o $@

stack.o: stack.c stack.h
	${CC} ${CFLAGS} -c $< -o $@

expression.o: expression.c expression.h
	${CC} ${CFLAGS} -c $< -o$@

########## Testing ##########
test_codegen: tests/test_codegen.c ll.o error.o symtable.o codegen.o scanner.o str.o
	${CC} ${CFLAGS} $^ -o $@

test_ll: tests/test_ll.c ll.o error.o symtable.o scanner.o str.o
	${CC} ${CFLANG} $^ -o $@

test_error: tests/test_error.c error.o ll.o symtable.o scanner.o str.o
	${CC} ${CFLAGS} $^ -o $@


#------ end of Testing -----#

clean:
	rm -f ${TMPS}
