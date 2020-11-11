/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: error.h
 * Brief: Header file for error.c
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */


#ifndef ERROR_H
#define ERROR_H

/*
  Function error
  --------------
  Prints error message

  file:     name of FILE      in which the error was encounteres
  func:     name of FUNCTION  ...
  msg, ...: custom message & its formatting

  returns:

  -----------------
  Example of usage:
    const char input[] = "Hello Wrld";
    error(99, "code.c", "foo", "Invalid input: %s", input);

  Output:
    ERROR:0		Internal compiler error (compiler's fault) -- with exit code 99
    code.c:	foo:
              Invalid input: Hello Wrld
*/
void error(
  int errCode,
  const char * file,
  const char * func,
  const char * msg, ...
);

const char * code_to_name(int);

#endif
