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
#include "ll.h"             // cleaning up linked list
#include "symtable.h"       // cleaning up hash table
#include "scanner.h"        // get_err_line
#include "parser.h"         // free(next), T_MAIN
#include "codegen_stack.h"  // free codegen labels stack
#include <stdio.h>
#include <string.h>         // string operations
#include <stdarg.h>         // va_start, va_end


const char * code_to_name(int in) {
  if (in >= 100 && in <= 103) return "arithmetic operator";
  if (in >= 104 && in <= 109) return "relational operator";
  if (in >= 118 && in <= 120) return "data type";

  switch (in) {
    case T_ASSIGNMENT:
      return "=";

    case T_COMMA:
      return ",";

    case T_L_BRACKET:
      return "(";

    case T_R_BRACKET:
      return ")";

    case T_EOL:
      return "EOL";

    case T_EOF:
      return "EOF";

    case T_IF:
      return "if";

    case T_ELSE:
      return "else";

    case T_FUNC:
      return "func";

    case T_RETURN:
      return "return";  // return: return "return" haha

    case T_PACKAGE:
      return "package";

    case T_IDENTIFIER:
      return "identifier";  // TODO add actual ID's name

    case T_LEFT_BRACE:
      return "{";

    case T_RIGHT_BRACE:
      return "}";

    case T_DEF_IDENT:
      return ":=";

    case T_SEMICOLON:
      return ";";

    case T_MAIN:
      return "main";

    case T_VAR_ID:
      return "variable ID";

    case T_FUNC_ID:
      return "function ID";

    case T_INT_VALUE: // TODO add actual value
      return "int value";

    case T_DEC_VALUE: // TODO add actual value
      return "float value";

    case T_STRING_VALUE:
      return "string"; // TODO add actual string

    default:
      return "_TYPE NOT IMPLEMENTED_";
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
    "ERROR:\t__line %d__\t%s -- with exit code %d\n%s:",
    errLine+1, tmp, errCode, file
  );
  if (func != NULL) {
    fprintf(stderr, "\t%s:", func);
  }
  fprintf(stderr, "\n\t");

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
  symtable_free( symtable );
  token_cleanup();
  scope_destroy();
  func_defs_destroy();
  jmp_label_stack_free_all();
  release_resources();
  frame_stack_destroy();
  undef_types_destroy();

  exit( errCode );
}
