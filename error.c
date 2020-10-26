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


#include "error.h"
#include "scanner.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


void error( int errCode, const char *fmt, ... )
{
  // int errLine = get_line(); // line containing error
  int errLine = 0;
  char msg[50];             // error report msg

  // Generates error report msg based on errCode
  switch ( errCode )
  {
    case 1:
      strcat( msg, "Lexical analysis failed" );
      break;

    case 2:
      strcat( msg, "Syntactic analysis failed" );
      break;

    case 99:
      strcat( msg, "Internal compiler error (compiler's fault)" );
      break;

    default:
      strcat( msg, "Semantic error" );
      break;
  }

  fprintf(
    stderr,
    "ERROR:%d\t\t%s -- with exit code %d\n\t",
    errLine, msg, errCode
  );

  // Handling error msg from scanner/parser
  if ( fmt != NULL )
  {
    // handling format & its arguments
    va_list pArg;
    va_start( pArg, fmt );
    vfprintf( stderr, fmt, pArg );
    va_end( pArg );
    // for neat spacing
    fprintf( stderr, "\n" );
  }

  exit( errCode );
}
