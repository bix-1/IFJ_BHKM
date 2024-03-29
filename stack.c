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

void stack_init(tokenStack **stack)
{
    *stack = malloc(sizeof(tokenStack));

    // Insterting the empty flag token
    stackElemPtr newElem = (stackElemPtr)malloc(sizeof(struct stackElem));
    if (!newElem)
    {
        // instert error call
        error(99, "stack", "stack_init", "Allocation error");
    }

    newElem->nextTok = NULL;

    newElem->token.token_type = T_EMPTY;
    newElem->data = NULL;
    newElem->expr = false;
    //newElem->token.attr.int_lit = 0; Not important ??

    (*stack)->topToken = newElem;
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
    newElem->data = NULL;
    newElem->expr = false;

    // Push the token
    // printf("STACK TOP\t%p\t%d\t%d\n", stack->topToken, stack->topToken->token.token_type);
    newElem->nextTok = stack->topToken;
    stack->topToken = newElem;
    // printf("STACK PUSH\t%p\t%p\t%d\n", newElem, stack->topToken,newElem->token.token_type);

    // printf("%d\n\n", tokPush.token_type);
    // exit(0);
}

void stack_pop(tokenStack *stack)
{

    // If element on the top is not empty flag
    if (stack->topToken->token.token_type != T_EMPTY)
    {
        stackElemPtr tmp = stack->topToken;
        stack->topToken = stack->topToken->nextTok;
        tmp->data = NULL;
        free(tmp);
    }
    else
    {
        //insert STACK_POP Error, trying to pop non-existent element
        error(99, "stack", "stack_pop", "Trying to access non-existent element"); // or "trying to pop non-existent element"
    }
}

int stack_count(tokenStack **stack)
{
    int i = 0;
    stackElemPtr tmp = (*stack)->topToken;
    // While only empty flag token is left on the stack
    while (tmp->token.token_type != T_EMPTY)
    {
        tmp = tmp->nextTok;
        i++;
    }
    return i;
}

void stack_free(tokenStack **stack)
{
    if (stack == NULL || *stack == NULL)
        return;

    if ((*stack)->topToken == NULL)
    {
        // MAYBE ERROR ? TRYING TO FREE UNALLOCATED
        return;
    }

    stackElemPtr tmp;

    while ((*stack)->topToken != NULL)
    {
        tmp = (*stack)->topToken;
        (*stack)->topToken = (*stack)->topToken->nextTok;
        tmp->data = NULL;
        free(tmp);
    }
    free(*stack);
    *stack = NULL;
}
