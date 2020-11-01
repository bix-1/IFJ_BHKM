/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: scanner.c
 * Brief: Implementation of scanner.
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include <stdlib.h>
#include "stack.h"
#include "scanner.h"

// Initialization of the stack
void stack_init(tokenStack *stack)
{
    stack->top.token_type = T_EMPTY;
    stack->nextTok = NULL;
}

void stack_push(tokenStack *stack, Token tokPush);
{
    Token *newToken = (Token *)malloc(sizeof(Token));
}



void stack_pop(tokenStack *stack);
void stack_empty(tokenStack *stack);
void stack_free(tokenStack *stack);
