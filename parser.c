/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: parser.c
 * Brief: Implementation of parser
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

/*
TODO:
  refactor func comments
*/


#include "parser.h"
#include "scanner.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T_MAIN 1000

tToken next;

// tests whether next_token matches next_term
// either matches & gets_new_token or calls error
void match(int term);

void program();
void prolog();


/*
  Functions for printing attributes of tokens
  TODO: remove for final implementation
*/
int64_t to_int(tToken *t) {
  return t->attr.int_lit;
}

double to_double(tToken *t) {
  return t->attr.dec_lit;
}

char * to_string(tToken *t) {
  return t->attr.str_lit.str;
}
//-------------------------------------------


void parse() {
  // initialization
  source_file_setup(stdin);
  get_next_token(&next);

  program();
}


void program() {
  prolog();
  printf("Prolog matched\n");

  // switch () {
  //
  //   default:
  //     error("parser.c", "program", "_PLACEHOLDER_");
  // }
}

void prolog() {
  match(T_PACKAGE);
  match(T_MAIN);
  match(T_EOL);
}

void match(int term) {
  if (term == T_MAIN) {
    if (next.token_type == T_IDENTIFIER && !strcmp(to_string(&next), "main")) {
      get_next_token(&next);
      return;
    }
    else
      error(99, "parser.c", "match", "main");
  }

  if (next.token_type == term)
    get_next_token(&next);
  else
    error(99, "parser.c", "match", "_PLACEHOLDER_");
}
