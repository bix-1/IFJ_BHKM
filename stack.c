/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: stack.c
 * Brief: Implementation of token stack.
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include <stdlib.h>
#include "stack.h"

void stack_init(tokenStack *stack)
{
    // Insterting the empty flag token
    stackElemPtr newElem = (stackElemPtr)malloc(sizeof(stackElemPtr));

    newElem->nextTok = NULL;
    newElem->token.token_type = T_EMPTY;

    stack->topToken = newElem;
}

void stack_push(tokenStack *stack, tToken tokPush)
{
    stackElemPtr newElem = (stackElemPtr)malloc(sizeof(stackElemPtr));

    // Allocation error
    if (!newElem)
    {
        // instert error call
    }

    newElem->token.token_type = tokPush.token_type;
    newElem->token.attr = tokPush.attr;

    // Push the token
    newElem->nextTok = stack->topToken;
    stack->topToken = newElem;
}

void stack_pop(tokenStack *stack)
{

    // If element on the top is not empty flag
    if (stack->topToken->token.token_type != T_EMPTY)
    {
        stackElemPtr tmp = stack->topToken;
        stack->topToken = stack->topToken->nextTok;
        free(tmp);
    }
    else
    {
        //insert STACK_POP Error, trying to pop non-existent element
    }
}

void stack_empty(tokenStack *stack)
{
    // While only empty flag token is left on the stack
    while (stack->topToken->token.token_type != T_EMPTY)
    {
        stack_pop(stack);
    }
}

void stack_free(tokenStack *stack)
{

    if (stack->topToken == NULL)
    {
        return;
    }

    stackElemPtr tmp;

    while (!stack->topToken)
    {
        tmp = stack->topToken;
        stack->topToken = stack->topToken->nextTok;
        free(tmp);
    }
}
