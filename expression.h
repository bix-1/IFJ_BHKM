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

extern tokenStack * symbolStack; // Symbol stack
extern tokenStack * tokStack;    // Token stack

typedef struct parserData
{
    tToken *token;
    tokenStack *stack;
} tPData;

typedef struct pd_item pd_item_t;
struct pd_item {
  tPData * pd;
  pd_item_t * next;
};

typedef struct stack_item stack_item_t;
struct stack_item {
  tokenStack * ts;
  stack_item_t * next;
};

struct frame_stack_t {
  pd_item_t * first_pd;
  stack_item_t * first_tok;
  stack_item_t * first_sym;
} frame_stack;

void frame_stack_init();
void frame_stack_destroy();
void frame_stack_push();
void frame_stack_pop();

/*
*   Function returns index of the symbol to choose action from precedence table
*/
int get_index(tToken token);

void print();
void check_func_start();
void check_func();

/*
*   Function creates variable in symtable
*/
symtable_value_t create_variable(stackElemPtr elem);

/*
*   Function creates destination in symtable for instruction
*/
symtable_value_t create_dest(stackElemPtr elem);

/*
*   Function crates unique id
*/
char *create_id();

/*
*   Function checks whether the variable is defined
*/
void check_symtable(stackElemPtr elem);

/*
*   Function sets data based on token type
*/
void var_data_set(tToken *token, sym_var_item_t *var);

/*
*   Function checks whether both token types are string
*/
void check_string(stackElemPtr top, stackElemPtr afterTop, tToken *symbol);

/*
*   Function checks whether both token types are same
*/
void check_num(stackElemPtr top, stackElemPtr afterTop);

/*
*   Function returns VAR_TYPE
*/
int var_type_check(stackElemPtr elem);

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
symtable_value_t parse_expression();

#endif //IFJ_BHKM_EXPRESSION_H
