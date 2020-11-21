/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: parser.h
 * Brief: Header file for parser.c
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */


#ifndef PARSER_H
#define PARSER_H

#include "ll.h"         // instr_t
#include <stdbool.h>     // bool built_in()
#include "scanner.h"
#include "symtable.h"   // elem_t
#include "expression.h"

// additional terminals not necessary in scanner
#define T_MAIN 1000
#define T_FUNC_ID 1001
#define T_VAR_ID 1002


/*_______________MAIN_FUNCTION______________*/
void parse();


/*__________________TOKENS__________________*/
// global variable needed in parser &
// [parser -- expression parser] interface
extern tToken next;
// cleans up last token in [elem_t *] next
void token_cleanup();
/*_______Getters for token attributes_______*/
int64_t to_int(tToken *t);
double to_double(tToken *t);
char * to_string(tToken *);
/*
  Function match
  ----------------
  Matches given terminal with current token

  __PARAMETERS:
  [int] term:    term to match with current token
  in next:
  [tToken *] next:    current token to match with term

  __RETURNS:  _____

  -----------------
  Example of usage:
    ___input file:
    package ;
    ______________

    match(T_PACKAGE);
    match(T_MAIN);

  Output:
    ERROR:	__line 1__	Syntactic analysis failed -- with exit code 2
    parser:	match:
          	Expected: 'main' -- Got: ';'
*/
void match(int term);
// global variables needed by match
extern elem_t * last_func;
extern elem_t * last_elem;


/*__________________SCOPE___________________*/
typedef struct scope_elem scope_elem_t;
struct scope_elem {
  char * name;
  scope_elem_t * next;
};

struct scope_t {
  scope_elem_t * first;
} scope;

/*_______________SCOPE_CONTROL______________*/
/*
  Function scope_init
  -------------------
  Initializes global variable [scope_t] scope
*/
void scope_init();
/*
  Function scope_destroy
  ----------------------
  Cleans up global variable [scope_t] scope
*/
void scope_destroy();
/*
  Function scope_push
  -------------------
  Pushes given [char *] on scope stack
*/
void scope_push(char *);
/*
  Function scope_pop
  ------------------
  Removes the top element from scope stack
*/
void scope_pop();
/*
  Function scope_get_head
  -----------------------
  Returns [scope_elem_t *] ptr to the element on top of scope stack
*/
scope_elem_t * scope_get_head();
/*
  Function scope_get
  ------------------
  Returns [char *] name of the element at the top of scope stack
  in format: <func_foo>:<index><if/else/for/for-body>:
*/
char * scope_get();
/*
  Function id_add_scope
  ---------------------
  Returns [char *] name of given id in the context of given scope
  in format: <format of scope_get>:id
*/
char * id_add_scope(scope_elem_t *scope, char *id);
/*
  Function id_find
  ----------------
  Looks for id in given & higher scopes

  __PARAMETERS:
  [scope_elem_t]  scope:    scope in which to start looking
  [char *]        id:       name to look for

  __RETURNS:
  in last_elem:
    [elem_t *] last_elem:   ptr to matched element

  -----------------
  Example of usage:
    eps = false;
    id_find(scope_get_head(), to_string(&next));
    printf("%s\n", last_elem->key);

  Output:
    Name_of_found_element
    ___OR___
    ERROR:	__line X__	Semantic error -- with exit code 3
    parser:	match:
          	Variable "Y" undefined
*/
elem_t * id_find(scope_elem_t *scope, char *id);

char * get_unique();


// skips all empty spaces
void skip_empty();

// additional instructions-related functions
void instr_add_func_def();
void instr_add_func_end();
void instr_add_var_decl();
void instr_add_var_def();
void instr_var_list_append_dest();
void instr_var_list_append_src();
void instr_add_if_end();
void instr_add_else_start();
void instr_add_else_end();
void instr_add_for_def();

// add parameter (last_elem) to last_func
void func_add_param();

typedef struct func_def func_def_t;
struct func_def {
  elem_t * func;
  func_def_t * next;
};

struct func_defs_t {
  func_def_t * first;
} func_defs;

void func_defs_init();
void func_defs_destroy();
void func_defs_add(elem_t *);
void func_defs_check();

bool built_in();
/*
  ___________FUNCTIONS_REPRESENTING___________
  ___________LL_GRAMMAR_NONTERMINALS__________

  For more info check implementation in parser.c
  or documentation in doc/LL.pdf
*/
void program();
void prolog();
void func_list_pre();
void func_list();
void func_def();
void param_list();
void next_param();
void func_def_type();
void func_def_ret();
void ret_list_def();
void next_ret_def();
void body();
void command();
void var_def();
void var_move();
void next_id();
void if_();
void if_cont();
void else_();
void cycle();
void for_def();
void for_move();
void return_();
void return_list();
void next_ret();
void func_call();
void func_args();
void expr_list();
void next_expr();
void type();

#endif
