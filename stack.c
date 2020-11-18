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
#include "error.h"

void stack_init(tokenStack *stack)
{

    // Insterting the empty flag token
    stackElemPtr newElem = (stackElemPtr)malloc(sizeof(struct stackElem));
    if (!newElem)
    {
        // instert error call
        error(99, "stack", "stack_init", "Allocation error");
    }

    newElem->nextTok = NULL;

    newElem->token.token_type = T_EMPTY;
    //newElem->token.attr.int_lit = 0; Not important ??

    stack->topToken = newElem;
    //printf("STACK INIT\t%p\t%p\t%d\n", newElem, stack->topToken, newElem->token.token_type);
}

void stack_push(tokenStack *stack, tToken tokPush)
{
    stackElemPtr newElem = (stackElemPtr)malloc(sizeof(struct stackElem));

    // Allocation error
    if (!newElem)
    {
        // instert error call
        error(99, "stack", "stack_push", "Allocation error");
        printf("\nSTACK PUSH ERROR\n");
    }

    newElem->token.token_type = tokPush.token_type;
    newElem->token.attr = tokPush.attr;
    newElem->originalType = tokPush.token_type;
    newElem->element = NULL;

    // Push the token
    //printf("STACK TOP\t%p\t%d\t%d\n", stack->topToken, stack->topToken->token.token_type);
    newElem->nextTok = stack->topToken;
    stack->topToken = newElem;
    //printf("STACK PUSH\t%p\t%p\t%d\n", newElem, stack->topToken,newElem->token.token_type);
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
        error(99, "stack", "stack_pop", "Trying to access non-existent element"); // or "trying to pop non-existent element"
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
        // MAYBE ERROR ? TRYING TO FREE UNALLOCATED
        return;
    }

    stackElemPtr tmp;

    while (stack->topToken != NULL)
    {
        tmp = stack->topToken;
        stack->topToken = stack->topToken->nextTok;
        free(tmp);
    }
}
