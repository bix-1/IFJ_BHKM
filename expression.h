/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: expression.h
 * Brief: Expression proccesing header file
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#ifndef IFJ_BHKM_EXPRESSION_H
#define IFJ_BHKM_EXPRESSION_H


#include "stack.h"
#include "scanner.h"

int get_index(tToken token);
int top();
void getToken();
void reduce();
void shift();
void equal();
void parse_expression();

#endif //IFJ_BHKM_EXPRESSION_H
