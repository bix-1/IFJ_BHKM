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


typedef struct parserData
{
    tToken *token;
    tokenStack *stack;
} tPData;

tPData parsData;

void parse();

// tests whether next_token matches next_term
// either matches & gets_new_token or calls error
void match(int term);

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
void ret_list();
void next_ret();
void body();
void command();
void var_();
void var_def();
void var_move();
void next_id();
void if_();
void cycle();
void for_def();
void for_move();
void return_();
void ret();
void func_call();
void expr_list();
void next_expr();
void type();

#endif
