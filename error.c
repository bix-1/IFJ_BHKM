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
#include "ll.h"     // cleaning up linked list
#include "symtable.h"   // cleaning up hash table
#include "scanner.h"  // get_err_line
#include <stdio.h>
#include <string.h>   // string operations
#include <stdarg.h>   // va_start, va_end


// Prints error message
void error(
  int errCode,
  const char * file,
  const char * func,
  const char * fmt, ...
) {
  int errLine = 0;    // line containing error
  char msg[50] = "";  // error report msg

  // internal compiler err -- no line to report
  if ( errCode != 99 ) errLine = get_err_line();

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
    "ERROR:%d\t\t%s -- with exit code %d\n%s:\t%s:\n\t\t",
    errLine, msg, errCode, file, func
  );

  // Handling error msg
  if ( fmt != NULL )
  {
    // handling format & its arguments
    va_list pArg;
    va_start( pArg, fmt );
    vfprintf( stderr, fmt, pArg );
    va_end( pArg );
  }
  // for neat spacing
  fprintf( stderr, "\n" );

  // clean-up of linked list and hash table
  list_destroy( &list );
  htab_free( symtable );

  exit( errCode );
}
