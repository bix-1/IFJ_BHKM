# Makefile for compiler implementation of imperative programing language IFJ20
#
# Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
#          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
#          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
#          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
#


CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g
TMPS = *.o *.a *.tgz ${wildcard test_*[^.][^c]} parser ifj20
AR = ar -rc
LIBS = scanner.a parser.a error.a ll.a symtable.a str.a expression.a

.PHONY: run_parser pack tests

all: ifj20

pack:
	tar -czvf xhladk15.tgz *.c *.h Makefile rozdeleni rozsireni dokumentace.pdf

ifj20: ifj20.o parser.a expr_parser.a codegen_stack.o codegen.o
		${CC} ${CFLAGS} $^ -o $@

test_ifj: ifj20.c ll.h ll.c symtable.c symtable.h codegen.h codegen.c error.h error.c scanner.h scanner.c str.c str.h parser.h parser.c expression.h expression.c stack.h stack.c escape_format.h escape_format.c codegen_stack.c codegen_stack.h
	${CC} ${CFLAGS} $^ -o $@

########## Static libraries ##########
scanner.a: scanner.o error.o str.o escape_format.o
	${AR} -T $@ $^

parser.a: parser.o scanner.a error.a ll.a symtable.a
	${AR} -T $@ $^

error.a: error.o ll.o symtable.o scanner.o parser.o expression.o str.o stack.o
	${AR} -T $@ $^

ll.a: ll.o error.a symtable.a
	${AR} -T $@ $^

symtable.a: symtable.o
	${AR} -T $@ $^

str.a: str.o error.a
	${AR} -T $@ $^

expr_parser.a: expression.o parser.o stack.o scanner.a
	${AR} -T $@ $^

#------ end of Static libraries -----#

# Rule for __all__ object files
%.o: %.c %.h
	${CC} ${CFLAGS} -c $<

########## Testing ##########
tests: ifj20
	cp ifj20 tests/
	cd tests && ./test_whole.sh
	@cat tests/failed_tests

test_codegen: tests/codegen_tests/test_codegen_multiple1.c ll.h ll.c symtable.c symtable.h codegen.h codegen.c error.h error.c scanner.h scanner.c str.c str.h parser.h parser.c expression.h expression.c stack.h stack.c escape_format.h escape_format.c tests/codegen_tests/test_codegen_helper.c tests/codegen_tests/test_codegen_helper.h tests/codegen_tests/test_codegen_helper.c tests/codegen_tests/test_codegen_helper.h codegen_stack.c codegen_stack.h
	${CC} ${CFLAGS} $^ -o $@

test_codegen_empty_main: tests/codegen_tests/test_codegen_empty_main.c ll.h ll.c symtable.c symtable.h codegen.h codegen.c error.h error.c scanner.h scanner.c str.c str.h parser.h parser.c expression.h expression.c stack.h stack.c escape_format.h escape_format.c tests/codegen_tests/test_codegen_helper.c tests/codegen_tests/test_codegen_helper.h tests/codegen_tests/test_codegen_helper.c tests/codegen_tests/test_codegen_helper.h codegen_stack.c codegen_stack.h
	${CC} ${CFLAGS} $^ -o $@

test_codegen_add1: tests/codegen_tests/test_codegen_add1.c ll.h ll.c symtable.c symtable.h codegen.h codegen.c error.h error.c scanner.h scanner.c str.c str.h parser.h parser.c expression.h expression.c stack.h stack.c escape_format.h escape_format.c tests/codegen_tests/test_codegen_helper.c tests/codegen_tests/test_codegen_helper.h tests/codegen_tests/test_codegen_helper.c tests/codegen_tests/test_codegen_helper.h codegen_stack.c codegen_stack.h
	${CC} ${CFLAGS} $^ -o $@

test_codegen_for: tests/codegen_tests/test_for.c ll.h ll.c symtable.c symtable.h codegen.h codegen.c error.h error.c scanner.h scanner.c str.c str.h parser.h parser.c expression.h expression.c stack.h stack.c escape_format.h escape_format.c tests/codegen_tests/test_codegen_helper.c tests/codegen_tests/test_codegen_helper.h tests/codegen_tests/test_codegen_helper.c tests/codegen_tests/test_codegen_helper.h codegen_stack.c codegen_stack.h
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

test_parser2: tests/test_parser.c parser.a expr_parser.a
	${CC} ${CFLAGS} $^ -o $@

test_symtable: tests/test_symtable.c symtable.c
	${CC} ${CFLAGS} $^ -o $@

run_parser: ifj20
	@bash tests/parser_tests/test.sh
#------ end of Testing -----#

clean:
	rm -f ${TMPS}
