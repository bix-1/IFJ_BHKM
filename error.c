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
#include "ll.h"         // cleaning up linked list
#include "symtable.h"   // cleaning up hash table
#include "scanner.h"    // get_err_line
#include "parser.h"     // free(next), T_MAIN
#include <stdio.h>
#include <string.h>     // string operations
#include <stdarg.h>     // va_start, va_end


const char * code_to_name(int in) {
  switch (in) {
    case T_COMMA:
      return ",";
    break;
    case T_L_BRACKET:
      return "(";
    break;
    case T_R_BRACKET:
      return ")";
    break;
    case T_EOL:
      return "EOL";
    break;
    case T_ELSE:
      return "else";
    break;
    case T_FUNC:
      return "func";
    break;
    case T_PACKAGE:
      return "package";
    break;
    case T_LEFT_BRACE:
      return "{";
    break;
    case T_RIGHT_BRACE:
      return "}";
    break;
    case T_SEMICOLON:
      return ";";
    break;
    case T_MAIN:
      return "main";
    break;

    default:
      return "NOT IMPLEMENTED";
    break;
  }
}

// Prints error message
void error(
  int errCode,
  const char * file,
  const char * func,
  const char * msg, ...
) {
  int errLine = 0;    // line containing error
  char tmp[50] = "";  // error report msg

  // internal compiler err -- no line to report
  if ( errCode != 99 ) errLine = get_err_line();

  // Generates error report msg based on errCode
  switch ( errCode )
  {
    case 1:
      strcat( tmp, "Lexical analysis failed" );
      break;

    case 2:
      strcat( tmp, "Syntactic analysis failed" );
      break;

    case 99:
      strcat( tmp, "Internal compiler error (compiler's fault)" );
      break;

    default:
      strcat( tmp, "Semantic error" );
      break;
  }

  fprintf(
    stderr,
    "ERROR:\t__line %d__\t%s -- with exit code %d\n%s:\t%s:\n\t",
    errLine+1, tmp, errCode, file, func
  );

  // Handling error msg
  if ( msg != NULL ){
    va_list pArg;
    va_start( pArg, msg );

    if (errCode == 2) {
      // Argument handling -- type_code --> type_name
      int exp_in = va_arg(pArg, int);
      int real_in = va_arg(pArg, int);
      const char * exp_out = code_to_name(exp_in);
      const char * real_out = code_to_name(real_in);
      fprintf(stderr, msg, exp_out, real_out);
    } else{
      // handling format & its arguments
      vfprintf( stderr, msg, pArg );
      va_end( pArg );
    }
  }
  // for neat spacing
  fprintf( stderr, "\n" );

  // clean-up of linked list and hash table
  list_destroy( &list );
  htab_free( symtable );

  exit( errCode );
}
