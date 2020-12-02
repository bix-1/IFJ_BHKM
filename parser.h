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
#include "scanner.h"    // tokens
#include "symtable.h"   // elem_t
#include <stdbool.h>    // bool built_in()

// additional terminals not necessary in scanner
#define T_MAIN 1000
#define T_FUNC_ID 1001
#define T_VAR_ID 1002

// Global variables for keeping track of
// the last function / variable definition
// between different functions (of parser)
extern elem_t * last_func;
extern elem_t * last_elem;


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
char * make_unique(elem_t *);


// Skips all empty spaces (EOLs, comments)
// till the first non-empty token
// used inbetween function parameters, in expressions, etc.
void skip_empty();

/*__________Instruction_generating__________*/
// additional instructions-related functions

// IC_DEF_FRUN from last_func
void instr_add_func_def();
// IC_END_FRUN from last_func
void instr_add_func_end();
// Function call instruction of given type
// from given function
void instr_add_func_call(elem_t * func, instr_type_t type);
// IC_DECL_VAR from given variable
void instr_add_var_decl(elem_t * var);
// IC_IF_DEF & adds new scope
void instr_add_if_def();
// IC_IF_END & terminates scope
void instr_add_if_end();
// IC_ELSE_START & adds new scope
void instr_add_else_start();
// IC_ELSE_END & terminates scope
void instr_add_else_end();
// IC_FOR_DEF & adds new scope
void instr_add_for_def();

/*__________________________________________*/
/*______________SYNTAX_ANALYSIS_____________*/


/*_____________FUNCTION_CONTROLL____________*/
// Checks for function call starting at given token
// Used in non-LL(1)-grammar-styled parsing of function calls
//    For more information see documentation
// Either parses func_call & returns function element
// OR returns NULL
//
// Is to be used AFTER checking for T_IDENTIFIER
// & stashing the ID of given token
//
// Following token will be returned in given token,
// parsed function call via function's [elem_t *] return
elem_t * try_func(tToken * token);
// Adds built-in functions to the symtable
// before the start of parsing process
void add_built_in();  // add built-in functions to symtable
// Adds given parameter to param list of given function
void func_add_param(elem_t *func, sym_var_item_t * ret);
// Adds given return to return list of given function
void func_add_ret(elem_t * func, sym_var_item_t * ret);
// Returns type of instruction to be generated
// for given function
// -- built-in functions require defferent instruction types
instr_type_t get_func_instr_type(char *func_name);
// Creates empty expression -- temporary, intermediate variable
// Used as an intermediary destination & source
// e.g. return foo() -- as dest for function call,
// and as src for return call
elem_t * create_expr(elem_t * func);
// Initializes given variable according to its type
// Used for initialization of named return calls
// e.g. func foo () (a int, b string)
//    a -- initialized to 0,
//    b -- initialized to ""
void var_init(elem_t * var);

/*____________LISTS_OF_VARIABLES____________*/
// Used for handling destinations of
//    variable definitions (a,b := 2,2)
//  & variable assignments (a,b == 4,4)

typedef struct id id_t;
struct id {
  char * id;
  id_t * next;
};

struct id_list_t {
  id_t * first;
  id_t * last;
} id_list;

// Initializes list of destination IDs
void id_list_init();
// Cleans-up list of destination IDs
void id_list_destroy();
// Adds new ID to the list of destination IDs
void id_list_add(char * id);
// Converts list of IDs (from id_list)
// to [sym_var_list] destination list
// according to given operation (either := or =)
elem_t * make_dest(int operation);



/*__________________________________________*/
/*_____________SEMANTIC_ANALYSIS____________*/
// Functions used for checking of types, return calls,
// function definitions, redefinitions, etc.
//  For forther information see documentation

// Called in case of variable definitions (a,b := 2,2)
// Assigns types to newly defined variables,
// checks types of already defined ones.
// Checks whether at least a single variable is undefined.
// Checks for yet undefined variables in expressions
// ^: a,b := 0,a
void assign_var_def_types(elem_t *dest_elem, elem_t *src_elem);
// Called in case of variable assigning (a,b = 2,2)
// Functionality similiar to assign_var_def_types
void assign_var_move_types(elem_t *dest_elem, elem_t *src_elem);
// Handles type assigning & cheking of given variables
// Either compares two defined types,
//  assigns defined type to the undefined one,
//  or adds the duo to the list of undefined types
//  for later type check.
// Used for handling expressions        e.g. a := foo() + 2
// and for handling returns & arguments e.g. foo(2,2)
bool check_types(sym_var_item_t * var1, sym_var_item_t * var2);

typedef struct undef_type undef_type_t;
struct undef_type {
  sym_var_item_t * var1;
  sym_var_item_t * var2;
  undef_type_t * next;
};

struct undef_types_t {
  undef_type_t * first;
  undef_type_t * last;
} undef_types;

// Initializes list of variables with undefined types
void undef_types_init();
// Destroys list of variables with undefined types
void undef_types_destroy();
// Adds new entry to the list of variables with undefined types
void undef_types_add(sym_var_item_t * var1, sym_var_item_t * var2);
// Performs a check on list of variables with undefined types
// -- can occur only in case of function call handling
// e.g. a := foo() + fee()
// where both foo & fee function can be undefined
// at the point of parsing their calls,
// But the types of the respective destinations for their calls
// will be assigned (as part of checking of function calls via func_defs_check)
// and can, at the point of calling this function, be checked
void undef_types_check();


/*__________FUNCTION_CALL_CHECK-UP__________*/
typedef struct func_def func_def_t;
struct func_def {
  elem_t * func;
  func_def_t * next;
};

struct func_defs_t {
  func_def_t * first;
} func_defs;

// Initializes list of function calls
void func_defs_init();
// Destroys list of function calls
void func_defs_destroy();
// Adds new entry to the list of function calls
void func_defs_add(elem_t *);
// Performs a check on list of function calls
//  Checks whether function calls match their corresponding definitions
// -- whether corresponding function definition exists,
// -- whether they match in number & type of:
//      parameters -- arguments, and returns
void func_defs_check();
// The following two functions perform a check on the number
// & types of given function's return destinations & arguments
// according to it's definition
void check_func_call_args(elem_t * func, elem_t * call);
void check_func_call_rets(elem_t * func, elem_t * call);
// Checks whether return call matches defined list of
// returns of given (current) function
// wth its definition stored in [elem_t *] last_func
void check_ret(elem_t *func_ret);


/*____________________________________________
  ___________FUNCTIONS_REPRESENTING___________
  ___________LL_GRAMMAR_NONTERMINALS__________

  For implementation see parser.c
  For more information see graph in doc/LL.pdf & documentation

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
void ret_list_def_named();
void next_ret_def_named();
void body();
void command();
void var_(char * id);
void next_id();
void if_();
void if_cont();
void else_();
void cycle();
void for_def();
void for_move();
void return_();
elem_t * return_list();
elem_t * func_call(char * name);
void func_args();
void next_arg();
elem_t * get_expr_list();
void next_expr();
void type();

#endif
