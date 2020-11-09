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
  func_list();
  match(T_EOL);
  printf("Program matched\n");
}

void prolog() {
  match(T_PACKAGE);
  match(T_MAIN);
  match(T_EOL);
}

//printf("88888888888888888888888\n");

void func_list_pre() {
  eps = true;
  match(T_FUNC_ID);
  if (eps) return;
  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);
  func_def_type();
  printf("88888888888888888888888\n");
  match(T_EOL);
  match(T_FUNC);
  func_list_pre();
  printf("func_list_pre matched\n");
}

void func_list() {
  eps = true;
  func_def();
  if (eps) return;
  func_list();
  printf("func_list matched\n");
}

void func_def() {
  match(T_FUNC);
  if (eps) return; // from func_list
  match(T_FUNC_ID);
  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);
  func_def_type();
  match(T_EOL);
  printf("Func_def matched\n");
}

void param_list() {
  eps = true;
  match(T_VAR_ID);
  if (eps) return;
  type();
  next_param();
  printf("param_list matched\n");
}

void next_param() {
  eps = true;
  match(T_COMMA);
  if (eps) return;
  match(T_VAR_ID);
  type();
  next_param();
  printf("Next_param matched\n");
}

void func_def_type() {
  // TODO
}

void func_def_void() {
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
  }
  printf("func_def_void matched\n");
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
    match(T_RETURN);
    expr_list();
    match(T_RIGHT_BRACE);
  }
  printf("func_def_ret matched\n");
}

void ret_list() {
  type();
  next_ret();
  printf("ret_list matched\n");
}

void next_ret() {
  eps = true;
  match(T_COMMA);
  if (eps) return;
  type();
  next_ret();
  printf("next_ret matched\n");
}

void body() {
  eps = true;
  command();
  if (eps) return;
  match(T_EOL);
  body();
  printf("body matched\n");
}

void command() {
  // TODO
  if (eps) return;
  printf("Matched command\n");
}
// void var_();
// void var_def();
// void var_move();
// void next_id();
// void if_();
// void cycle();
// void for_def();
// void for_move();
// void return_();
// void ret();
// void func_call();

void expr_list() {
  eps = true;

  if (!eps) return; // if func_call was done
}

// void next_expr();

void type() {
  switch (next.token_type) {
    case T_INT:
      printf("Matched int\n");
      break;

    case T_FLOAT64:
      printf("Matched float\n");
      break;

    case T_STRING:
      printf("Matched string\n");
      break;
  }
}
