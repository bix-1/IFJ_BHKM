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

/*
*   Function returns index of the symbol to choose action from precedence table
*/
int get_index(tToken token);

/*
*   Function checks whether the token types are same
*/
void check_types(stackElemPtr top, stackElemPtr afterTop);

/*
*   Function which checks if the term is missing from operation
*/
void check_expr(tToken *token);

/*
*   Function shifts the current token to stack and gets next token
*/
void shift();

/*
*   Function reduces the expression with rule based on the precedence table 
*/
void reduce();

/*
*   Function removes parenthesis
*/
void equal();

/*
*   Function which free's the memory
*/
void release_resources();

/*
*   Main function which parses the expression
*/
void parse_expression();

#endif //IFJ_BHKM_EXPRESSION_H
