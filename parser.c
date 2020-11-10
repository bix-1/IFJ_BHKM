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
  REMOVE RULE 13 IN DOC
*/


#include "parser.h"
#include "scanner.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// additional terminals not necessary in scanner
#define T_MAIN 1000
#define T_FUNC_ID 1001
#define T_VAR_ID 1002

tToken next;
// for checking whether eps terminal is allowed
bool eps = false;

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

// TODO remove
// placeholder for expression parser
void expr() {
  printf("\texpr: %d", next.token_type);
  while (next.token_type != T_EOL) {
    get_next_token(&next);
    printf("\texpr: %d", next.token_type);
  }
  printf("Expression matched\n");
}

// functions representing LL grammar nonterminals

void parse() {
  // initialization
  source_file_setup(stdin);
  get_next_token(&next);

  program();
}

void match(int term) {
  switch (term) {
    case T_MAIN:
      if (next.token_type == T_IDENTIFIER && !strcmp(to_string(&next), "main")) {
        get_next_token(&next);
        return;
      }
      else
        // check whether syntax error or matched terminal eps
        if (!eps) error(99, "parser.c", "match", "main");
        else return;  // eps == TRUE
      break;

    case T_FUNC_ID:
      if (next.token_type != T_IDENTIFIER || !strcmp( to_string(&next), "main")) {
        // check whether syntax error or matched terminal eps
        if (!eps) error(99, "parser.c", "match", "main");
        else return;  // eps == TRUE
      }
      // TODO function definition handling
      get_next_token(&next);
      break;

    case T_VAR_ID:
      if (next.token_type != T_IDENTIFIER) {
        // check whether syntax error or matched terminal eps
        if (!eps) error(99, "parser.c", "match", "main");
        else return;  // eps == TRUE
      }
      // TODO var definition handling
      get_next_token(&next);
      break;

    default:
      if (next.token_type == term)
        get_next_token(&next);
      else
        // check whether syntax error or matched terminal eps
        if (!eps)
          error(
            99, "parser.c", "match",
            "Wanted: %d -- Got: %d", term, next.token_type
          );
        else return;  // eps == TRUE
      break;
  }
  // for checking in nonterminals whether
  // eps == TRUE  --> terminal eps
  // eps == FALSE --> non-eps terminal matched
  eps = false;
}

void program() {
  prolog();
  printf("Prolog matched\n");

  match(T_FUNC);
  func_list_pre();

  match(T_MAIN);
  match(T_L_BRACKET);
  match(T_R_BRACKET);
  match(T_LEFT_BRACE);
  match(T_EOL);
  body();
  match(T_RIGHT_BRACE);
  match(T_EOL);

  // for debugging purposes TODO delete
  printf("Main matched\n");

  func_list();
  match(T_EOL);
  printf("Program matched\n");
}

void prolog() {
  match(T_PACKAGE);
  match(T_MAIN);
  match(T_EOL);
}

void func_list_pre() {
  eps = true;
  match(T_FUNC_ID);
  if (eps) return;

  // for debugging purposes TODO delete
  char s[50] = "";
  strcat(s, to_string(&next));

  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);
  func_def_type();
  match(T_EOL);
  match(T_FUNC);
  // for debugging purposes TODO delete
  printf("---Func %s matched\n", s);
  func_list_pre();
}

void func_list() {
  eps = true;
  func_def();
  if (eps) return;
  func_list();
}

void func_def() {
  match(T_FUNC);
  if (eps) return; // from func_list
  match(T_FUNC_ID);

  // TODO delete
  // for debugging purposes
  char s[50] = "";
  strcat(s, to_string(&next));

  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);
  func_def_type();
  match(T_EOL);

  // for debugging purposes TODO delete
  printf("---Func %s matched\n", s);
}

void param_list() {
  eps = true;
  match(T_VAR_ID);
  if (eps) return;
  type();
  next_param();
}

void next_param() {
  eps = true;
  match(T_COMMA);
  if (eps) return;
  match(T_VAR_ID);
  type();
  next_param();
}

void func_def_type() {
  switch( next.token_type ) {
    case T_LEFT_BRACE:
      match(T_LEFT_BRACE);
      match(T_EOL);
      body();
      match(T_RIGHT_BRACE);
      break;

    case T_L_BRACKET:
      match(T_L_BRACKET);
      func_def_ret();
      break;

    default:
      error(
        99, "parser.c", "match",
        "Wanted: '(' or '{' -- Got: %d", next.token_type
      );
      break;
  }
}

void func_def_ret() {
  if (next.token_type == T_R_BRACKET) {
    match(T_R_BRACKET);
    match(T_LEFT_BRACE);
    match(T_EOL);
    body();
    match(T_RIGHT_BRACE);
  }
  else {
    ret_list();
    match(T_R_BRACKET);
    match(T_LEFT_BRACE);
    match(T_EOL);
    body();
    match(T_RIGHT_BRACE);
  }
}

void ret_list() {
  type();
  next_ret();
}

void next_ret() {
  eps = true;
  match(T_COMMA);
  if (eps) return;
  type();
  next_ret();
}

void body() {
  eps = true;
  command();
  if (eps) {
    eps = false;
    return;
  }
  match(T_EOL);
  body();
}

void command() {
  switch (next.token_type) {
    case T_IDENTIFIER:  // var_ OR func_call
      eps = true;
      match(T_VAR_ID);
      if (!eps) var_();
      else {
        eps = false;
        func_call();
      }
      break;

    // if
    case T_IF:
      if_();
      break;

    // cycle
    case T_FOR:
      cycle();
      break;

    case T_RETURN:
      return_();
      break;

    default:
      error(99, "parser", "command", "_PLACEHOLDER_");
      break;
  }

  if (eps) { // TODO delete the printf
    printf("...No more commands\n");
    return;
  }
}

void var_() {
  // TODO add ':='
  if (next.token_type == ':=') var_def();
  else var_move();
}

void var_def() {
  // TODO add ':='
  match(':=');
  expr();
}

void var_move() {
  next_id();
  match(T_ASSIGNMENT);
  expr_list();
}

void next_id() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  match(T_VAR_ID);
  next_id();
}

void if_() {
  match(T_IF);
  expr(); // condition handling
  match(T_LEFT_BRACE);
  match(T_EOL);
  body();
  match(T_RIGHT_BRACE);
  match(T_ELSE);
  match(T_LEFT_BRACE);
  match(T_EOL);
  body();
  match(T_RIGHT_BRACE);
}

void cycle() {
  match(T_FOR);
  for_def();
  match(';');  // TODO fucking scanner fix
  expr(); // expression parser call
  match(';');  // TODO fucking scanner fix
  for_move();
  match(T_LEFT_BRACE);
  match(T_EOL);
  body();
  match(T_RIGHT_BRACE);
}

void for_def() {
  eps = true;
  match(T_VAR_ID);
  if (eps) {
    eps = false;
    return;
  }
  var_def();
}

void for_move() {
  eps = true;
  match(T_VAR_ID);
  if (eps) {
    eps = false;
    return;
  }
  var_move();
}

void return_() {
  match(T_RETURN);
  expr_list();
}

void func_call() {
  match(T_FUNC_ID);
  match(T_L_BRACKET);
  expr_list();
  match(T_R_BRACKET);
}

void expr_list() {
  eps = true;
  expr(); // expression handling
  if (!eps) next_expr();  // expr matched
  else      func_call();
  printf("expr_list matched\n");
}

void next_expr() {
  eps = true;
  match(T_COMMA);
  if (eps) return;
  expr(); // expression handling
  next_expr();
}

void type() {
  switch (next.token_type) {
    case T_INT:
      // printf("Matched int\n");
      break;

    case T_FLOAT64:
      // printf("Matched float\n");
      break;

    case T_STRING:
      // printf("Matched string\n");
      break;

    default:
      error(
        99, "parser.c", "match",
        "Wanted: int, float or string -- Got: %d", next.token_type
      );
      break;
  }
  get_next_token(&next);
}
