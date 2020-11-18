/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: parser.h
 * Brief: Header file for parser.c
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */


#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "expression.h"

// additional terminals not necessary in scanner
#define T_MAIN 1000
#define T_FUNC_ID 1001
#define T_VAR_ID 1002

typedef struct parserData
{
    tToken *token;
    tokenStack *stack;
} tPData;

tPData parsData;

extern tToken next;

void token_cleanup();

void parse();

// tests whether next_token matches next_term
// either matches & gets_new_token or calls error
// -- contains all error calls that are not context-specific
void match(int term);

/*__________________SCOPE__________________*/
typedef struct scope_elem scope_elem_t;
struct scope_elem {
  char * name;
  scope_elem_t * next;
};

struct scope_t {
  scope_elem_t * first;
} scope;

// Getters for token attributes
int64_t to_int(tToken *t);
double to_double(tToken *t);
char * to_string(tToken *);

// functions for scope control
void scope_init();
void scope_destroy();
void scope_push(char *);
void scope_pop();
char * scope_get();

// skip all empty spaces
void skip_empty();

// additional instructions-related functions
void instr_add_func_def();
void instr_add_func_end();
void instr_add_var_decl();
void instr_add_var_def();
void instr_var_list_append_dest();
void instr_var_list_append_src();
void instr_add_if_end();
void instr_add_else_start();
void instr_add_else_end();

// add parameter (last_elem) to last_func
void func_add_param();

// LL grammar nonterminals
void program();
void prolog();
void func_list_pre();
void func_list();
void func_def();
void param_list();
void next_param();
void func_def_type();
void func_def_ret();
void ret_list_def();
void next_ret_def();
void body();
void command();
void var_();
void var_def();
void var_move();
void next_id();
void if_();
void if_cont();
void else_();
void cycle();
void for_def();
void for_move();
void return_();
void return_list();
void next_ret();
void func_call();
void func_args();
void expr_list();
void next_expr();
void type();

#endif
