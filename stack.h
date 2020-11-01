/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: stack.h
 * Brief: Stack header file.
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "scanner.h"

typedef struct tokenStack
{
    Token top;
    Token *nextTok;

} tokenStack;

void stack_init(tokenStack *stack);
void stack_push(tokenStack *stack, Token tokPush);
void stack_pop(tokenStack *stack);
void stack_empty(tokenStack *stack);
void stack_free(tokenStack *stack);