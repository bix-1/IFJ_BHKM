# Makefile for compiler implementation of imperative programing language IFJ20
#
# Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
#          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
#          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
#          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
#


CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
TMPS = *.o *.a *.tgz ${wildcard test_*[^.][^c]} parser ifj20
AR = ar -csr
LIBS = scanner.a parser.a error.a ll.a symtable.a str.a expression.a

.PHONY: run_parser pack

all: ifj20

pack:
	tar -czvf xhladk15.tgz *.c *.h Makefile

ifj20: ifj20.c parser.a expr_parser.a
	${CC} ${CFLAGS} $^ -o $@


########## Static libraries ##########
scanner.a: scanner.o error.o str.o
	rm -f $@
	${AR} $@ $^

parser.a: parser.o scanner.a error.a # expression.o
	rm -f $@
	${AR} -T $@ $^

error.a: error.o ll.o symtable.o scanner.a parser.o
	rm -f $@
	${AR} -T $@ $^

ll.a: ll.o error.a
	rm -f $@
	${AR} -T $@ $^

symtable.a: symtable.o
	rm -f $@
	${AR} -T $@ $^

str.a: str.o error.a
	rm -f $@
	${AR} -T $@ $^

expr_parser.a: expression.o parser.o stack.o scanner.a
	rm -f $@
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

test_parser: tests/parser_tests/parser_test.c str.a parser.a
	${CC} ${CFLAGS} $^ -o $@

test_scanner: tests/scanner_tests/scanner_test.c error.a scanner.a
	${CC} ${CFLAGS} $^ -o $@

test_assignment: tests/test_assignment.c ${LIBS}
	${CC} ${CFLAGS} $^ -o $@

parser: parser.a expr_parser.a
	${CC} ${CFLAGS} $^ -o $@

test_parser2: tests/parser_tests/test_parser.c parser
	${CC} ${CFLAGS} $^ -o $@

run_parser: parser test_parser2
	@bash tests/parser_tests/test.sh
#------ end of Testing -----#

clean:
	rm -f ${TMPS}
