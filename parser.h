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

// skip all empty spaces
// TODO & comments
void skip_empty();

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
