/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: error.c
 * Brief: Implementation of functions handling errors
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

// TODO
// less obfuscated error msg
//    -- add  the: ... argument parsing


#include "error.h"
#include "scanner.h"
#include <stdio.h>
#include <string.h>

void error( int errCode ) {
  char filename[] = getFilename();
  int errLine = getLine();
  char msg[100];

  switch ( errCode ) {
    case 1:
      strcat( msg, "Lexical analysis failed" );
      break;
    case 2:
      strcat( msg, "Syntactic analysis failed" );
      break;

    // TODO
    // add the rest

    default:
      break;
  }

  fprintf(
    stderr,
    "%s:%d: ERROR: %s -- with exit code %d\n",
    filename, errLine, msg, errCode
  );

  exit( errCode );
}
