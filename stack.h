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

#ifndef IFJ_BHKM_STACK_H
#define IFJ_BHKM_STACK_H

#include "scanner.h"
#include "symtable.h"


/*
 * Structure which is element of stack, holds token and pointer to next token
 */
typedef struct stackElem
{
    struct stackElem *nextTok;
    tToken token;
    int originalType;
    elem_t *data;
    bool expr;
} *stackElemPtr;

/*
 * Structure, that holds top element and next element on stack
 */
typedef struct tokenStack
{
    stackElemPtr topToken;
} tokenStack;

/*
*   Function which initializes stack
*/
void stack_init(tokenStack **stack);

/*
*   Function pushes tokPush on the top of the stack
*/
void stack_push(tokenStack *stack, tToken tokPush);

/*
*   Function which pops the token on the top of the stack
*/
void stack_pop(tokenStack *stack);

/*
*   Function which empties the stack
*/
void stack_empty(tokenStack *stack);

/*
*   Function which free's the stack
*/
void stack_free(tokenStack **stack);

#endif //IFJ_BHKM_STACK_H
