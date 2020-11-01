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

/*
*   Function which initializes stack
*/
void stack_init(tokenStack *stack);

/*
*   Function pushes tokPush on the top of the stack
*/
void stack_push(tokenStack *stack, Token tokPush);

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
void stack_free(tokenStack *stack);