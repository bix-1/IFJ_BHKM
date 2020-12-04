/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: parser.c
 * Brief: Implementation of parser
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */


#include "parser.h"   // own header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "error.h"    // error calls handling
#include "scanner.h"  // get_next_token
#include "symtable.h" // symbol table for codegenerator
#include "codegen.h"  // instruction types
#include "ll.h"       // list of instructions
#include "str.h"      // contextual IDs
#include "expression.h" // expression parser

/*__________________________________________*/
/*_____________GLOBAL_VARIABLES_____________*/
// list of instructions
extern list_t * list;
// symbol table
extern symtable_t * symtable;
// list of scopes (for keeping track of var/func_call context)
extern struct scope_t scope;
// token from scanner yet to be processed
tToken next;

// generating of unique IDs (for symtable, tmp variables)
unsigned long U_ID = 0;
#define U_ID_LENGTH 20
char u_id[U_ID_LENGTH + 1] = "";

/*_____________SYMTABLE_HANDLING____________*/
// keeps track of function that's being defined
elem_t * last_func;
// keeps track of last variable
//  for purposes of type assigning, ...
elem_t * last_elem;


/*_______________GLOBAL_FLAGS_______________*/
// EPS terminal
// outside match():
//    for allowing eps terminal
// inside of match():
//    for checking whether EPS or NON-EPS terminal matched
bool eps = false;
// DEFINITION / CALL switch for variable & function IDs
// outside match():
//    for allowing definiton
// inside of match():
//    for checking whether ID is being defined or called
bool def = false;
// get_next_token controll inside of match()
bool get_next = true;
// for checking whether function with defined return types
// contains at least a single return call
bool has_ret = false;


/*_______________MAIN_FUNCTION______________*/
void parse() {
  // initialization
  // global
  source_file_setup(stdin);   // for scanner
  list = list_create();
  symtable = symtable_init(1061);
  get_next_token(&next);
  // local
  scope_init();
  func_defs_init();
  undef_types_init();
  add_built_in();

  // entry point to parsing
  program();

  // semantic checks
  func_defs_check();
  undef_types_check();

  // local termination
  scope_destroy();
  func_defs_destroy();
  undef_types_destroy();
  id_list_destroy();

  // code generating
  codegen();

  // global termination
  list_destroy(&list);
  symtable_free(symtable);
  jmp_label_stack_free_all();
}


/*______________TOKEN_HANDLING______________*/
void token_cleanup() {
  if (
    ( next.token_type == T_IDENTIFIER ||
      next.token_type == T_MAIN ||
      next.token_type == T_VAR_ID ||
      next.token_type == T_FUNC_ID
    ) && next.attr.str_lit.str != NULL
  ) {
    free(next.attr.str_lit.str);
    next.attr.str_lit.str = NULL;
  }
}

// Getters for token attributes
int64_t to_int(tToken *t) {
  return t->attr.int_lit;
}

double to_double(tToken *t) {
  return t->attr.dec_lit;
}

char * to_string(tToken *t) {
  return t->attr.str_lit.str;
}

void match(int term) {
  switch (term) {
    case T_MAIN:
      // check type
      if (next.token_type != T_IDENTIFIER) {
        if (eps) return;
        goto default_err;
      }
      // check ID
      if (strcmp(to_string(&next), "main")) {
        if (eps) return;
        error(3, "parser", "match", "Missing main function");
      }

      // not changing scope -- in prolog: (package main)
      if (!def) {
        token_cleanup();
        break;
      }

      // create ID
      char * name_old = to_string(&next);
      char * name_new = malloc(sizeof(char) * (strlen(name_old) + 1));
      if (name_new == NULL) error(99, "parser", NULL, "Memory allocation failed");
      strcpy(name_new, name_old);

      // create func elem
      sym_func_t * func = sym_func_init(name_new, NULL, NULL);
      symbol_t func_sym = {.sym_func = func};
      elem_t * func_elem = elem_init(SYM_FUNC, func_sym);
      symtable_insert(symtable, name_old, func_elem);
      last_func = func_elem;

      // add to scope
      char * new_scope = malloc(sizeof(char) * 5);
      if (new_scope == NULL) error(99, "parser", NULL, "Memory allocation failed");
      strcpy(new_scope, to_string(&next));
      U_ID = 0; // new founc -- can reset counter
      scope_push(new_scope);

      break;

    case T_FUNC:
      if (next.token_type != T_FUNC) {
        if (eps) return;
        if (next.token_type != T_EOF) goto default_err;
        else
          error( 3, "parser", "program", "Missing function [main]" );
      }
      break;

    case T_FUNC_ID:
      // check type
      if (next.token_type != T_IDENTIFIER) {
        if (eps) {
          def = false;
          return;
        }
        goto default_err;
      }

      // check ID
      if (!strcmp( to_string(&next), "main")) {
        if (eps) {
          def = false;
          return;
        }
        next.token_type = T_MAIN;
        goto default_err;
      }
      if (!strcmp(to_string(&next), "_")) {
        if (def)  error (3, "parser", NULL, "Redefinition of special variable \"_\"");
        else      error (3, "parser", NULL, "Unable to read from special variable \"_\"");
      }

      // expected func definition && func != main
      // --> cannot be EPS terminal
      // otherwise if in func_list_pre:
      //    ends user_func_def segment
      //    && "Missing main func" error called
      if (def) eps = false;

      if (def) { //definition expected
        symtable_iterator_t it = symtable_find(symtable, to_string(&next));
        if (symtable_iterator_valid(it)) {  // found in symtable
          if (eps) {
            def = false;
            return;
          } else error(3, "parser", "match", "Redefinition of function \"%s\"", to_string(&next));
        }
        else { // first definition
          // create ID
          char * name_old = to_string(&next);
          char * name_new = malloc(sizeof(char) * (strlen(name_old) + 1));
          if (name_new == NULL) error(99, "parser", NULL, "Memory allocation failed");
          strcpy(name_new, name_old);

          // create func elem
          sym_func_t * func_sym = sym_func_init(name_new, NULL, NULL);
          symbol_t func = {.sym_func = func_sym};
          elem_t * func_data = elem_init(SYM_FUNC, func);
          symtable_insert(symtable, name_old, func_data);
          last_func = func_data;

          // add to scope
          char * new_scope = malloc(sizeof(char) * (strlen(to_string(&next))+1) );
          if (new_scope == NULL) error(99, "parser", NULL, "Memory allocation failed");
          strcpy(new_scope, to_string(&next));
          U_ID = 0; // new founc -- can reset counter
          scope_push(new_scope);
          last_elem = last_func;
        }
      }
      else { // func call expected
        // create function element
        sym_func_t * func_sym = sym_func_init(to_string(&next), NULL, NULL);
        symbol_t func = {.sym_func = func_sym};
        elem_t * func_elem = elem_init(SYM_FUNC, func);

        // create key
        char * index = get_unique();
        char i_name[strlen(index) + strlen(to_string(&next)) + 1];
        strcpy(i_name, index);
        strcat(i_name, to_string(&next));
        char * func_name = id_add_scope(scope_get_head(), i_name);
        // add to symtable
        symtable_insert(symtable, func_name, func_elem);
        last_elem = func_elem;

        func_defs_add(last_elem);
      }
      break;

    case T_VAR_ID:
      if (next.token_type != T_IDENTIFIER) {
        if (eps) {
          def = false;
          return;
        }
        goto default_err;
      }

      if (def) {  // definition expected
        // handle special variable
        if (!strcmp(to_string(&next), "_")) {
          error (3, "parser", NULL, "Redefinition of special variable \"_\"");
        }

        // get variable's contextual ID
        char * id = id_add_scope(scope_get_head(), to_string(&next));
        symtable_iterator_t it = symtable_find(symtable, id);

        if (symtable_iterator_valid(it)) {  // found in symtable
          free(id);
          if (eps) {
            def = false;
            return;
          }
          else error(3, "parser", "match", "Redefinition of variable \"%s\"", to_string(&next));
        }
        else {  // first occurrence
          token_cleanup(); // non-contextual ID no longer needed

          // create var
          sym_var_item_t * var_item = sym_var_item_init(id);
          symbol_t var_sym = {.sym_var_item = var_item};
          elem_t * var = elem_init(SYM_VAR_ITEM, var_sym);
          var->symbol.sym_var_item->data.string_t = NULL;
          symtable_insert(symtable, id, var);

          // check if var is parameter of func
          // --> skip declaration
          if (last_elem == NULL || last_elem->sym_type != SYM_FUNC) {
            instr_add_var_decl(var);
          }

          last_elem = var;
        }
      }
      else {  // var_call expected
        elem_t * elem;
        if (!strcmp(to_string(&next), "_")) elem = NULL;
        else elem = id_find(scope_get_head(), to_string(&next));
        token_cleanup(); // elem found & token.attr no longer needed

        if (last_elem->sym_type == SYM_VAR_LIST) {
          sym_var_item_t * item;
          if (elem == NULL) item = NULL;
          else item = elem->symbol.sym_var_item;
          list_item_t * list_item = list_item_init(item);
          sym_var_list_add(last_elem->symbol.sym_var_list, list_item);
        }
        else last_elem = elem;
      }
      break;

    default:
      if (next.token_type != term) {
        if (eps) return;
        default_err:
        error(
          2, "parser", "match",
          "Expected: '%s' -- Got: '%s'", term, next.token_type
        );
      }
      break;
  }
  // for checking in nonterminals whether
  // eps == TRUE  --> terminal eps
  // eps == FALSE --> non-eps terminal matched
  eps = false;

  if (get_next) {
    get_next_token(&next);
  }
}


/*_______________SCOPE_CONTROL______________*/
void scope_init() {
  scope.first = NULL;
}

void scope_destroy() {
  scope_elem_t * tmp, * next;

  for (
    tmp = scope.first;
    tmp != NULL;
    tmp = next
  ) {
    next = tmp->next;
    free(tmp->name);
    free(tmp);
  }

  scope.first = NULL;
}

void scope_push(char * new_name) {
  scope_elem_t * tmp = scope.first;
  scope_elem_t * new = malloc(sizeof(scope_elem_t));
  if (new == NULL) error(99, "parser", NULL, "Memory allocation failed");
  if (tmp == NULL) {
    new->name = malloc(sizeof(char) * (strlen(new_name) + 2));
    if (new->name == NULL) error(99, "parser", NULL, "Memory allocation failed");
    strcpy(new->name, new_name);
    strcat(new->name, "-");
  } else {
    new->name = malloc(
      sizeof(char) *
      (strlen(tmp->name) + strlen(new_name) + 2)
    );
    if (new->name == NULL) error(99, "parser", NULL, "Memory allocation failed");
    strcpy(new->name, tmp->name);
    strcat(new->name, new_name);
    strcat(new->name, "-");
  }
  free(new_name);
  new->next = tmp;
  scope.first = new;
}

void scope_pop() {
  if (scope.first == NULL) return;
  scope_elem_t * tmp = scope.first->next;
  free(scope.first->name);
  free(scope.first);
  scope.first = tmp;
}

scope_elem_t * scope_get_head() {
  return scope.first;
}

char * id_add_scope(scope_elem_t * tmp_scope, char * old_id) {
  // no scope given
  if (tmp_scope == NULL) {
    char * new_id = malloc(sizeof(char) * (strlen(old_id) + 1));
    if (new_id == NULL) error(99, "parser", NULL, "Memory allocation failed");
    strcpy(new_id, old_id);
    return new_id;
  }

  // create scope name
  char * scope_name = tmp_scope->name;
  char * new_id = malloc(
    sizeof(char) *  // prefix + : + name + \0
    (strlen(scope_name) + strlen(old_id) + 2)
  );
  if (new_id == NULL) error(99, "parser", NULL, "Memory allocation failed");
  new_id[0] = '\0';
  strcat(new_id, scope_name);
  strcat(new_id, old_id);

  return new_id;
}

elem_t * id_find(scope_elem_t *scope, char *old_id) {
  char * id;
  scope_elem_t * tmp_scope = scope;
  symtable_iterator_t it;

  // look through all higher contexts
  while (true) {
    // check for end of scope
    if (tmp_scope == NULL) {
      if (eps) return NULL;
      if (!strcmp(old_id, "_")){
        error (3, "parser", NULL, "Unable to read from special variable \"_\"");
      } else {
        error(3, "parser", "match", "Variable \"%s\" undefined", old_id);
      }
    }
    // get variable's contextual ID
    id = id_add_scope(tmp_scope, old_id);
    // look for in symtable
    it = symtable_find(symtable, id);
    free(id);
    if (symtable_iterator_valid(it)) {  // found in symtable
      if (symtable_iterator_get_value(it)->symbol.sym_var_item->is_defined) {
        eps = false;
        return symtable_iterator_get_value(it);
      }
    }
    tmp_scope = tmp_scope->next;
  }
}

char * get_unique() {
  sprintf(u_id, "%ld", U_ID++);
  return u_id;
}

char * make_unique(elem_t * elem) {
  char type[6] = "";
  switch(elem->sym_type) {
    case SYM_VAR_LIST:
      strcpy(type, "list");
      break;
    case SYM_FUNC:
      strcpy(type, "func");
      break;
    case SYM_VAR_ITEM:
      strcpy(type, "f_ret");
      break;
    default:
      strcpy(type, "undef");
  }
  char * index = get_unique();
  char i_name[strlen(index) + strlen(type) + 1];
  strcpy(i_name, index);
  strcat(i_name, type);
  return id_add_scope(scope_get_head(), i_name);
}


void skip_empty() {
  eps = true;
  match(T_EOL);
  eps = false;
}


/*__________Instruction_generating__________*/
void instr_add_func_def() {
  instr_t * new_func = instr_create();
  instr_set_type(new_func, IC_DEF_FUN);
  instr_add_dest(new_func, last_func);
  instr_add_elem1(new_func, decl_list_init());
  list_add(list, new_func);
}

void instr_add_func_end() {
  instr_t * end_func = instr_create();
  instr_set_type(end_func, IC_END_FUN);
  instr_add_dest(end_func, last_func);
  list_add(list, end_func);

  decl_list_clear();
}

void instr_add_func_call(elem_t * func, instr_type_t type) {
  instr_t * func_call = instr_create();
  instr_set_type(func_call, type);
  instr_add_dest(func_call, func);
  list_add(list, func_call);
}

void instr_add_var_decl(elem_t * var) {
  decl_list_add(var->symbol.sym_var_item);
}

void instr_add_if_def() {
  instr_t * new_if = instr_create();
  instr_set_type(new_if, IC_IF_DEF);
  list_add(list, new_if);

  // create new scope
  char * prefix = get_unique();
  char * if_scope = malloc(sizeof(char) * (strlen(prefix) + 3)); // u_id + if\0
  if (if_scope == NULL) error(99, "parser", NULL, "Memory allocation failed");
  strcpy(if_scope, prefix);
  strcat(if_scope, "if");

  scope_push(if_scope);
}

void instr_add_if_end() {
  instr_t * if_end = instr_create();
  instr_set_type(if_end, IC_IF_END);
  list_add(list, if_end);

  scope_pop();
}

void instr_add_else_start() {
  instr_t * new_else = instr_create();
  instr_set_type(new_else, IC_ELSE_START);
  list_add(list, new_else);

  // create new scope
  char * prefix = get_unique();
  char * else_scope = malloc(sizeof(char) * (strlen(prefix) + 5)); // u_id + else\0
  if (else_scope == NULL) error(99, "parser", NULL, "Memory allocation failed");
  strcpy(else_scope, prefix);
  strcat(else_scope, "else");

  scope_push(else_scope);
}

void instr_add_else_end() {
  instr_t * else_end = instr_create();
  instr_set_type(else_end, IC_ELSE_END);
  list_add(list, else_end);

  scope_pop();
}

void instr_add_for_def() {
  instr_t * new_for = instr_create();
  instr_set_type(new_for, IC_FOR_DEF);
  list_add(list, new_for);

  // create new scope
  char * prefix = get_unique();
  char * for_scope = malloc(sizeof(char) * (strlen(prefix) + 4)); // u_id + for\0
  if (for_scope == NULL) error(99, "parser", NULL, "Memory allocation failed");
  strcpy(for_scope, prefix);
  strcat(for_scope, "for");

  scope_push(for_scope);
}


/*_____________FUNCTION_CONTROLL____________*/
elem_t * try_func(tToken * token) {
  // either parses func_call & returns func OR returns NULL
  // keep the ID before calling this func
  // following token will be in given token (parameter)

  char * id = to_string(token);
  get_next_token(token);

  if (token->token_type == T_L_BRACKET) {
    elem_t * func = func_call(id);
    token->token_type = next.token_type;
    token->attr = next.attr;
    return func;
  } else return NULL;
}

// built-in functions insertion
enum built_in_names {
  INPUTS, INPUTI, INPUTF, INPUTB,
  PRINT,
  INT2FLOAT, FLOAT2INT,
  LEN, SUBSTR, ORD, CHR
};

void add_built_in() {

  char * built_ins[] = {
    "inputs", "inputi", "inputf", "inputb",
    "print",
    "int2float", "float2int",
    "len", "substr", "ord", "chr"
  };

  const int N = 11;
  char * name;

  for (int i = 0; i < N; i++) {
    // create name
    name = malloc(sizeof(char) * (strlen(built_ins[i]) + 1));
    if (name == NULL) error(99, "parser", NULL, "Memory allocation failed");
    strcpy(name, built_ins[i]);

    // create func
    sym_func_t * func_sym = sym_func_init(NULL, NULL, NULL);
    symbol_t func_s = {.sym_func = func_sym};
    elem_t * func = elem_init(SYM_FUNC, func_s);
    symtable_insert(symtable, name, func);

    // assigning of parameters & returns
    var_type_t type;
    switch (i) {
      case INPUTS:
        type = VAR_STRING;
        break;
      case INPUTI:
        type = VAR_INT;
        break;
      case INPUTF:
        type = VAR_FLOAT64;
        break;
      case INPUTB:
        type = VAR_BOOL;
        break;
      default: goto not_input;
    }
    // was an input function
    func_def_add_ret(func, type);
    func_def_add_ret(func, VAR_INT);

    not_input:
    switch (i) {
      case INT2FLOAT:
        func_def_add_param(func, VAR_INT);
        func_def_add_ret(func, VAR_FLOAT64);
        break;
      case FLOAT2INT:
        func_def_add_param(func, VAR_FLOAT64);
        func_def_add_ret(func, VAR_INT);
        break;
      case LEN:
        func_def_add_param(func, VAR_STRING);
        func_def_add_ret(func, VAR_INT);
        break;
      case SUBSTR:
        func_def_add_param(func, VAR_STRING);
        func_def_add_param(func, VAR_INT);
        func_def_add_param(func, VAR_INT);
        func_def_add_ret(func, VAR_STRING);
        func_def_add_ret(func, VAR_INT);
        break;
      case ORD:
        func_def_add_param(func, VAR_STRING);
        func_def_add_param(func, VAR_INT);
        func_def_add_ret(func, VAR_INT);
        func_def_add_ret(func, VAR_INT);
        break;
      case CHR:
        func_def_add_param(func, VAR_INT);
        func_def_add_ret(func, VAR_STRING);
        func_def_add_ret(func, VAR_INT);
        break;
      default:
        break;
    }
  }
}

void func_def_add_ret(elem_t * func, var_type_t type) {
  // create var item
  sym_var_item_t * ret = sym_var_item_init(NULL);
  sym_var_item_set_type(ret, type);
  if (type == VAR_STRING) ret->data.string_t = NULL;

  // add to symtable for cleanup
  symbol_t ret_sym = {.sym_var_item = ret};
  elem_t * ret_elem = elem_init(SYM_VAR_ITEM, ret_sym);
  char * func_name = *(func->key);
  char * ret_id = malloc(sizeof(char) * (strlen(func_name) + 6));
  if (ret_id == NULL) error(99, "parser", NULL, "Memory allocation failed");
  strcpy(ret_id, func_name);
  strcat(ret_id, "-");
  if (func->symbol.sym_func->returns == NULL) strcat(ret_id, "0ret");
  else strcat(ret_id, "1ret");

  symtable_insert(symtable, ret_id, ret_elem);

  func_add_ret(func, ret);
}

void func_def_add_param(elem_t * func, var_type_t type) {
  // create item
  sym_var_item_t * param = sym_var_item_init(NULL);
  sym_var_item_set_type(param, type);
  if (type == VAR_STRING) param->data.string_t = NULL;

  // add to symtable for cleanup
  symbol_t param_sym = {.sym_var_item = param};
  elem_t * param_elem = elem_init(SYM_VAR_ITEM, param_sym);
  char * func_name = *(func->key);
  char * param_id = malloc(sizeof(char) * (strlen(func_name) + 6));
  if (param_id == NULL) error(99, "parser", NULL, "Memory allocation failed");
  strcpy(param_id, func_name);
  strcat(param_id, "-");

  // create param's ID
  if (func->symbol.sym_func->params == NULL) strcat(param_id, "0prm");
  else if (func->symbol.sym_func->params->first->next == NULL) strcat(param_id, "1prm");
  else strcat(param_id, "2prm");

  symtable_insert(symtable, param_id, param_elem);

  func_add_param(func, param);
}


void func_add_param(elem_t *func, sym_var_item_t *param) {
  if (func == NULL || param == NULL)
    error(99, "parser", "type", "Failed to access function's parameters");

  // get param list
  sym_var_list_t ** params = &(func->symbol.sym_func->params);
  if (params == NULL)
    error(99, "parser", "func_add_param", "Failed to access function's parameters");
  if (*params == NULL) { // param list empty
    *params = new_sym_var_list();
  }

  // add to param list
  list_item_t * list_item = list_item_init(param);
  sym_var_list_add(*params, list_item);
}

void func_add_ret(elem_t *func, sym_var_item_t *ret) {
  if (func == NULL || ret == NULL)
    error(99, "parser", "type", "Failed to access function's returns");

  // get ret list
  sym_var_list_t ** rets = &(func->symbol.sym_func->returns);
  if (rets == NULL)
    error(99, "parser", "func_add_ret", "Failed to access function's returns");
  if (*rets == NULL)  // param list empty
    *rets = new_sym_var_list();

  // add to ret list
  list_item_t * list_item = list_item_init(ret);
  sym_var_list_add(*rets, list_item);
}

instr_type_t get_func_instr_type(char * func) {
  if (
    !strcmp(func, "inputs") ||  // string
    !strcmp(func, "inputi") ||  // int
    !strcmp(func, "inputf") ||  // float
    !strcmp(func, "inputb")     // bool
  ) return IC_READ_VAR;
  if (!strcmp(func, "print"))     return IC_WRITE_VAR;
  if (!strcmp(func, "int2float")) return IC_INT2FLOAT_VAR;
  if (!strcmp(func, "float2int")) return IC_FLOAT2INT_VAR;
  if (!strcmp(func, "len"))       return IC_STRLEN_STR;
  if (!strcmp(func, "SUBSTR"))    return IC_SUBSTR_STR;
  if (!strcmp(func, "ORD"))       return IC_STR2INT_VAR;
  if (!strcmp(func, "CHR"))       return IC_GETCHAR_STR;

  return IC_CALL_FUN;
}

elem_t * create_expr(elem_t * func) {
  // create ID
  char * key = make_unique(func);
  sym_var_item_t * expr = sym_var_item_init(key);
  symbol_t expr_sym = {.sym_var_item = expr};
  elem_t * expr_elem = elem_init(SYM_VAR_ITEM, expr_sym);

  symtable_insert(symtable, key, expr_elem);
  instr_add_var_decl(expr_elem);

  return expr_elem;
}

void var_init(elem_t * var) {
  // create initializing expression
  char * key = make_unique(var);
  sym_var_item_t * expr_item = sym_var_item_init(key);
  symbol_t expr_sym = {.sym_var_item = expr_item};
  elem_t * expr = elem_init(SYM_VAR_ITEM, expr_sym);
  symtable_insert(symtable, key, expr);

  // create initializing data
  variable_t init;
  switch (var->symbol.sym_var_item->type) {
    case VAR_INT:
      init.int_t = 0;
      break;
    case VAR_FLOAT64:
      init.float64_t = 0.0;
      break;
    case VAR_STRING: {
      char * empty = malloc(sizeof(char));
      if (empty == NULL) error(99, "parser", NULL, "Memory allocation failed");
      empty[0] = '\0';
      init.string_t = empty;
      break;}
    case VAR_BOOL:
      init.bool_t = false;
      break;
    default:
      break;
  }
  sym_var_item_set_type(
    expr->symbol.sym_var_item,
    var->symbol.sym_var_item->type
  );
  sym_var_item_set_data(expr->symbol.sym_var_item, init);
  sym_var_item_set_const(expr->symbol.sym_var_item, true);

  // create lists for var_def instruction
  sym_var_list_t * dest_list = sym_var_list_init();
  sym_var_list_t * src_list = sym_var_list_init();
  symbol_t dest_sym = {.sym_var_list = dest_list};
  symbol_t src_sym = {.sym_var_list = src_list};
  elem_t * dest_elem = elem_init(SYM_VAR_LIST, dest_sym);
  elem_t * src_elem = elem_init(SYM_VAR_LIST, src_sym);
  symtable_insert(symtable, make_unique(dest_elem), dest_elem);
  symtable_insert(symtable, make_unique(src_elem), src_elem);

  // add initializator & var to lists
  list_item_t * var_item = list_item_init(var->symbol.sym_var_item);
  list_item_t * init_item = list_item_init(expr->symbol.sym_var_item);
  sym_var_list_add(dest_list, var_item);
  sym_var_list_add(src_list, init_item);

  // add initializing instruction
  instr_t * new_init = instr_create();
  instr_set_type(new_init, IC_DEF_VAR);
  instr_add_dest(new_init, dest_elem);
  instr_add_elem1(new_init, src_elem);
  list_add(list, new_init);
}


/*___________LISTS_OF_DECLARATIONS__________*/
extern elem_t * decl_list;

elem_t * decl_list_init() {
  sym_var_list_t * list = sym_var_list_init();
  symbol_t list_sym = {.sym_var_list = list};
  elem_t * list_elem = elem_init(SYM_VAR_LIST, list_sym);

  symtable_insert(symtable, make_unique(list_elem), list_elem);
  decl_list = list_elem;

  return list_elem;
}

void decl_list_clear() {
  // all items are inserted into symtable
  // & cleaned during symtable cleanup
  decl_list = NULL;
}

void decl_list_add(sym_var_item_t *item) {
  list_item_t * list_item = list_item_init(item);
  sym_var_list_add(decl_list->symbol.sym_var_list, list_item);
}


/*____________LISTS_OF_VARIABLES____________*/
extern struct id_list_t id_list;

void id_list_init() {
  id_list.first = NULL;
  id_list.last = NULL;
}

void id_list_destroy() {
  id_t * next;
  for (
    id_t * it = id_list.first;
    it != NULL;
    it = next
  ) {
    next = it->next;
    free(it->id);
    free(it);
  }
  id_list_init();
}

void id_list_add(char * id) {
  // create ID
  id_t * new = malloc(sizeof(id_t));
  char * new_id = malloc(sizeof(char) * (strlen(id) + 1));
  if (new == NULL || new_id == NULL) error(99, "parser", NULL, "Memory allocation failed");
  strcpy(new_id, id);

  // init
  new->id = new_id;
  new->next = NULL;

  // add to list
  if (id_list.first == NULL) {
    id_list.first = new;
  } else {
    id_list.last->next = new;
  }

  id_list.last = new;
}

elem_t * make_dest(int operation) {
  // create dest
  sym_var_list_t * dest_list = sym_var_list_init();
  symbol_t dest_list_sym = {.sym_var_list = dest_list};
  elem_t * dest = elem_init(SYM_VAR_LIST, dest_list_sym);
  symtable_insert(symtable, make_unique(dest), dest);

  // handle according to operation
  id_t * next;
  if (operation == T_DEF_IDENT) { // :=
    for (
      id_t * tmp = id_list.first;
      tmp != NULL;
      tmp = next
    ) {
      next = tmp->next;
      // for cleanup in case of error
      id_list.first = tmp;

      // try to get existing variable -- in current scope
      char * id = id_add_scope(scope_get_head(), tmp->id);
      symtable_iterator_t it = symtable_find(symtable, id);

      // handling of special variable
      if (!strcmp(tmp->id, "_")) {
        list_item_t * list_item = list_item_init(NULL);
        sym_var_list_add(dest_list, list_item);
        free(tmp->id);
        free(tmp);
        free(id);
        continue;
      }

      if (symtable_iterator_valid(it)) { // found in current scope
        // add existing item to list
        sym_var_item_t * item = symtable_iterator_get_value(it)->symbol.sym_var_item;
        list_item_t * list_item = list_item_init(item);
        sym_var_list_add(dest_list, list_item);
        free(id); // contextual id no longer needed
        free(tmp->id);
      } else {
        // create new item
        free(tmp->id);
        sym_var_item_t * new_item = sym_var_item_init(id);
        symbol_t new_sym = {.sym_var_item = new_item};
        elem_t * new = elem_init(SYM_VAR_ITEM, new_sym);
        new->symbol.sym_var_item->data.string_t = NULL;
        symtable_insert(symtable, id, new);
        // add to the list
        list_item_t * list_item = list_item_init(new_item);
        sym_var_list_add(dest_list, list_item);
        // add instruction
        instr_add_var_decl(new);
      }
      free(tmp);
    }
  } else { // =
    for (
      id_t * tmp = id_list.first;
      tmp != NULL;
      tmp = next
    ) {
      next = tmp->next;
      // for cleanup in case of error
      id_list.first = tmp;

      // handling of special variable
      if (!strcmp(tmp->id, "_")) {
        list_item_t * list_item = list_item_init(NULL);
        sym_var_list_add(dest_list, list_item);
        free(tmp->id);
        free(tmp);
        continue;
      }

      // try to get ID -- in all scopes
      eps = false;
      elem_t * var = id_find(scope_get_head(), tmp->id);

      // add exitsting item to list
      list_item_t * list_item = list_item_init(var->symbol.sym_var_item);
      sym_var_list_add(dest_list, list_item);

      free(tmp->id);
      free(tmp);
    }
  }

  id_list_init();
  return dest;
}

/*__________________________________________*/
/*_____________SEMANTIC_ANALYSIS____________*/

/*__________VARIABLE_TYPE_CHECKING__________*/
void assign_var_def_types(elem_t * dest_elem, elem_t * src_elem) {
  // assign src to dest & handle their types

  // flag for ensuring the presence of
  // at least a single undefined variable
  bool has_def = false;

  // dest setup
  sym_var_list_t * dest_list = dest_elem->symbol.sym_var_list;
  list_item_t * dest_item = dest_list->first;

  // src setup
  sym_var_list_t * src_list = src_elem->symbol.sym_var_list;
  list_item_t * src_item = src_list->first;

  sym_var_item_t * dest;
  sym_var_item_t * src;
  int n_dest = 0, n_src = 0;
  while (dest_item != NULL) {
    // unmatched number of expressions / variables
    if (src_item == NULL) {
      // count vars
      while (dest_item != NULL) {
        dest_item = dest_item->next;
        n_dest++;
      }
      error(
        7, "parser", "assign_var_def_types",
        "Failed to assign [%d] expressions to [%d] variables",
        n_src, n_dest
      );
    }

    dest = dest_item->item;
    src = src_item->item;

    if (dest != NULL) {
      // handle def state
      if (!(dest->is_defined)) {
        has_def = true;
        dest->is_defined = true;
      }

      // handle types
      if (!check_types(dest, src)) {
        error(
          7, "parser", "assign_var_def_types",
          "Variable [%s] and Expression [n.%d] are of different data types",
          dest->name, n_dest
        );
      }
    }
    // ^ skips special variable "_"
    // & leaves out raising has_def flag
    // -- as it cannot be defined

    // next step
    dest_item = dest_item->next;
    src_item = src_item->next;
    n_dest++;
    n_src++;
  }

  if (!has_def) {
    error(3, "parser", "definition check", "Definition requires at least a single undefined variable");
  }

  // check if src also finished
  if (src_item != NULL) {
    // count exprs
    while (src_item != NULL) {
      src_item = src_item->next;
      n_src++;
    }
    error(
      7, "parser", "assign_var_def_types",
      "Failed to assign [%d] expressions to [%d] variables",
      n_src, n_dest
    );
  }
}

void assign_var_move_types(elem_t * dest_elem, elem_t * src_elem) {
  // assign src to dest & handle their types

  // dest setup
  sym_var_list_t * dest_list = dest_elem->symbol.sym_var_list;
  list_item_t * dest_item = dest_list->first;

  // src setup
  sym_var_list_t * src_list = src_elem->symbol.sym_var_list;
  list_item_t * src_item = src_list->first;

  sym_var_item_t * dest;
  sym_var_item_t * src;
  int n_dest = 0, n_src = 0;
  while (dest_item != NULL) {
    // unmatched number of expressions / variables
    if (src_item == NULL) {
      // count vars
      while (dest_item != NULL) {
        dest_item = dest_item->next;
        n_dest++;
      }
      error(
        7, "parser", "assign_var_def_types",
        "Failed to assign [%d] expressions to [%d] variables",
        n_src, n_dest
      );
    }

    dest = dest_item->item;
    src = src_item->item;

    if (dest != NULL) {
      if (!(dest->is_defined)) {
        error(
          3, "parser", "assignment checking", "Variable [%s] undefined",
          dest->name
        );
      }

      if (!check_types(dest, src)) {
        error(
          7, "parser", "assign_var_move_types",
          "Variable [%s] and Expression [n.%d] are of different data types",
          dest->name, n_dest
        );
      }
    }

    // next step
    dest_item = dest_item->next;
    src_item = src_item->next;
    n_dest++;
    n_src++;
  }

  // check if src also finished
  if (src_item != NULL) {
    // count exprs
    while (src_item != NULL) {
      src_item = src_item->next;
      n_src++;
    }
    error(
      7, "parser", "assign_var_def_types",
      "Failed to assign [%d] expressions to [%d] variables",
      n_src, n_dest
    );
  }
}

bool check_types(sym_var_item_t * var1, sym_var_item_t * var2) {
  var_type_t type1 = var1->type;
  var_type_t type2 = var2->type;

  if (type1 != VAR_UNDEFINED) {
    if (type2 != VAR_UNDEFINED) { // compare
      if (type1 == type2) return true;
      else return false;
    }
    else {  // [assign] type2 <-- type1
      var2->type = type1;
      if (var2->type == VAR_STRING) {
        var2->data.string_t = NULL;
      }
    }
  }
  else {
    if (type2 != VAR_UNDEFINED) { // [assign] type1 <-- type2
      var1->type = type2;
      if (var1->type == VAR_STRING) {
        var1->data.string_t = NULL;
      }
    }
    else {
      undef_types_add(var1, var2);
    }
  }
  return true;
}

void undef_types_init() {
  undef_types.first = NULL;
  undef_types.last = NULL;
}

void undef_types_destroy() {
  undef_type_t * next;
  for (
    undef_type_t * it = undef_types.first;
    it != NULL;
    it = next
  ) {
    next = it->next;
    free(it);
  }

  undef_types_init();
}

void undef_types_add(sym_var_item_t * var1, sym_var_item_t * var2) {
  undef_type_t * new = malloc(sizeof(undef_type_t));
  if (new == NULL) error(99, "parser", NULL, "Memory allocation failed");

  new->var1 = var1;
  new->var2 = var2;
  new->next = NULL;

  if (undef_types.first == NULL) {
    undef_types.first = new;
  } else {
    undef_types.last->next = new;
  }
  undef_types.last = new;
}

void undef_types_check() {
  for (
    undef_type_t * it = undef_types.first;
    it != NULL;
    it = it->next
  ) {
    if (!check_types(it->var1, it->var2)) {
      error(5, "parser", "undef_types_check", "Function call destination types do not match");
    }
  }
}


/*__________FUNCTION_CALL_CHECK-UP__________*/
extern struct func_defs_t func_defs;

// [var_type_t] --> [char *] conversion for error handling
// of non-matching variable types
#define NAME_MAX_L 8 // strlen("FLOAT64") + \0

sym_var_list_t * new_sym_var_list() {
  sym_var_list_t * var_list = sym_var_list_init();
  symbol_t var_list_sym = {.sym_var_list = var_list};
  elem_t * dest = elem_init(SYM_VAR_LIST, var_list_sym);

  symtable_insert(symtable, make_unique(dest), dest);

  return var_list;
}

void func_defs_init() {
    func_defs.first = NULL;
}

void func_defs_destroy() {
  func_def_t * tmp = func_defs.first;
  func_def_t * next;
  while (tmp != NULL) {
    next = tmp->next;
    free(tmp);
    tmp = next;
  }
  func_defs.first = NULL;
}

void func_defs_add(elem_t * func) {
  func_def_t * new = malloc(sizeof(func_def_t));
  if (new == NULL) error(99, "parser", NULL, "Memory allocation failed");

  new->func = func;
  new->next = func_defs.first;
  func_defs.first = new;
}

void func_defs_check() {
  func_def_t * func_call = func_defs.first;
  char * name;

  while (func_call != NULL) {
    name = func_call->func->symbol.sym_func->name;
    symtable_iterator_t it = symtable_find(symtable, name);

    if (symtable_iterator_valid(it)) {
      elem_t * elem = symtable_iterator_get_value(it);
      if (!strcmp(name, "print")) {
        // no need to check arguments -- print can take any ammount of anything
        check_func_call_rets(elem, func_call->func);
      } else {
        check_func_call_args(elem, func_call->func);
        check_func_call_rets(elem, func_call->func);
      }
    } else {
      error(3, "parser", "func_check", "Function '%s' called but not defined", name);
    }
    // next step
    func_call = func_call->next;
  }
}

void check_func_call_args(elem_t * def_e, elem_t * call_e) {
  // init
  sym_var_list_t * def_params = def_e->symbol.sym_func->params;
  sym_var_list_t * call_params = call_e->symbol.sym_func->params;

  list_item_t * def;
  list_item_t * call;
  if (def_params == NULL) def = NULL;
  else def = def_params->first;
  if (call_params == NULL) call = NULL;
  else call = call_params->first;

  int n_def = 0, n_call = 0;

  while (def != NULL) {
    // unmatched number of returns / defined returns
    if (call == NULL) {
      // count vars
      while (def != NULL) {
        def = def->next;
        n_def++;
      }
      error(
        6, "parser", "check_args",
        "Number of arguments [%d] differs from defined [%d] for function [%s]",
        n_call, n_def, *(def_e->key)
      );
    }

    // handle types
    if (!check_types(def->item, call->item)) {

      char def_type[NAME_MAX_L] = "";
      char call_type[NAME_MAX_L] = "";
      switch (def->item->type) {
        case VAR_INT:
          strcpy(def_type, "INT");
        break;
        case VAR_FLOAT64:
          strcpy(def_type, "FLOAT64");
        break;
        case VAR_STRING:
          strcpy(def_type, "STRING");
        break;
        case VAR_BOOL:
          strcpy(def_type, "BOOL");
        break;
        default:
        break;
      }
      switch (call->item->type) {
        case VAR_INT:
          strcpy(call_type, "INT");
        break;
        case VAR_FLOAT64:
          strcpy(call_type, "FLOAT64");
        break;
        case VAR_STRING:
          strcpy(call_type, "STRING");
        break;
        case VAR_BOOL:
          strcpy(call_type, "BOOL");
        break;
        default:
        break;
      }

      error(
        6, "parser", "check_args",
        "[%s] argument given where [%s] was defined for parameter [%d] of function [%s]",
        call_type, def_type, n_def, *(def_e->key)
      );
    }

    // next step
    def = def->next;
    call = call->next;
    n_def++;
    n_call++;
  }

  // check if call also finished
  if (call != NULL) {
    // count exprs
    while (call != NULL) {
      call = call->next;
      n_call++;
    }
    error(
      6, "parser", "check_args",
      "Number of arguments [%d] differs from defined [%d] for function [%s]",
      n_call, n_def, *(def_e->key)
    );
  }
}

void check_func_call_rets(elem_t * def_e, elem_t * call_e) {
  // init
  sym_var_list_t * def_rets = def_e->symbol.sym_func->returns;
  sym_var_list_t * call_rets = call_e->symbol.sym_func->returns;

  list_item_t * def;
  list_item_t * call;
  if (def_rets == NULL) def = NULL;
  else def = def_rets->first;
  if (call_rets == NULL) call = NULL;
  else call = call_rets->first;

  int n_def = 0, n_call = 0;
  while (def != NULL) {
    // unmatched number of returns / defined returns
    if (call == NULL) {
      // count vars
      while (def != NULL) {
        def = def->next;
        n_def++;
      }
      error(
        6, "parser", "function call assignment",
        "Number of func_call destinations [%d] differs from defined [%d] returns of function [%s]",
        n_call, n_def, *(def_e->key)
      );
    }

    // handle types
    if (!check_types(def->item, call->item)) {
      char def_type[NAME_MAX_L] = "";
      char call_type[NAME_MAX_L] = "";
      switch (def->item->type) {
        case VAR_INT:
          strcpy(def_type, "INT");
        break;
        case VAR_FLOAT64:
          strcpy(def_type, "FLOAT64");
        break;
        case VAR_STRING:
          strcpy(def_type, "STRING");
        break;
        case VAR_BOOL:
          strcpy(def_type, "BOOL");
        break;
        default:
        break;
      }
      switch (call->item->type) {
        case VAR_INT:
          strcpy(call_type, "INT");
        break;
        case VAR_FLOAT64:
          strcpy(call_type, "FLOAT64");
        break;
        case VAR_STRING:
          strcpy(call_type, "STRING");
        break;
        case VAR_BOOL:
          strcpy(call_type, "BOOL");
        break;
        default:
        break;
      }

      error(
        6, "parser", "function call assignment",
        "Assignment to [%s] where [%s] was defined for return [%d] of function [%s]",
        call_type, def_type, n_def, *(def_e->key)
      );
    }

    if (call->item->type == VAR_STRING) {
      call->item->data.string_t = NULL;
    }

    // next step
    def = def->next;
    call = call->next;
    n_def++;
    n_call++;
  }

  // check if call also finished
  if (call != NULL) {
    // count exprs
    while (call != NULL) {
      call = call->next;
      n_call++;
    }
    error(
      6, "parser", "check_func_call_rets",
      "Number of func_call destinations [%d] differs from defined [%d] returns of function [%s]",
      n_call, n_def, *(def_e->key)
    );
  }
}

void check_ret(elem_t * func) {
  // check whether return types of func_call match the definition

  // returns setup
  sym_var_list_t * ret_list = func->symbol.sym_func->returns;

  // definitions setup
  elem_t * func_defs = symtable_iterator_get_value(
    symtable_find(symtable, func->symbol.sym_func->name)
  );
  sym_var_list_t * def_list = func_defs->symbol.sym_func->returns;

  list_item_t * ret;
  list_item_t * def;
  if (ret_list == NULL) ret = NULL;
  else ret = ret_list->first;
  if (def_list == NULL) def = NULL;
  else def = def_list->first;

  int n_defs = 0, n_rets = 0;

  while (def != NULL) {
    // unmatched number of returns / defined returns
    if (ret == NULL) {
      // count vars
      while (def != NULL) {
        def = def->next;
        n_defs++;
      }
      error(
        6, "parser", "check_rets",
        "Number of returns [%d] differs from defined [%d] for function [%s]",
        n_rets, n_defs, *(last_func->key)
      );
    }

    // handle types
    if (!check_types(def->item, ret->item)) {
      char def_type[NAME_MAX_L] = "";
      char ret_type[NAME_MAX_L] = "";
      switch (def->item->type) {
        case VAR_INT:
          strcpy(def_type, "INT");
        break;
        case VAR_FLOAT64:
          strcpy(def_type, "FLOAT64");
        break;
        case VAR_STRING:
          strcpy(def_type, "STRING");
        break;
        case VAR_BOOL:
          strcpy(def_type, "BOOL");
        break;
        default:
        break;
      }
      switch (ret->item->type) {
        case VAR_INT:
          strcpy(ret_type, "INT");
        break;
        case VAR_FLOAT64:
          strcpy(ret_type, "FLOAT64");
        break;
        case VAR_STRING:
          strcpy(ret_type, "STRING");
        break;
        case VAR_BOOL:
          strcpy(ret_type, "BOOL");
        break;
        default:
        break;
      }

      error(
        6, "parser", "check_rets",
        "Returning [%s] where [%s] was defined for parameter [%d] of function [%s]",
        ret_type, def_type, n_defs, *(func_defs->key)
      );
    }

    // next step
    def = def->next;
    ret = ret->next;
    n_defs++;
    n_rets++;
  }

  // check if src also finished
  if (ret != NULL) {
    // count exprs
    while (ret != NULL) {
      ret = ret->next;
      n_rets++;
    }
    error(
      6, "parser", "check_rets",
      "Number of returns [%d] differs from defined [%d] for function [%s]",
      n_rets, n_defs, *(last_func->key)
    );
  }

  has_ret = true;
}


/*
  ___________FUNCTIONS_REPRESENTING___________
  ___________LL_GRAMMAR_NONTERMINALS__________

*/
void program() {
  prolog();

  if (next.token_type == T_EOF)
    error(3, "parser", "program", "Missing function main");

  // user functions before main
  match(T_FUNC);
  func_list_pre();

  // start of main definition
  def = true;
  match(T_MAIN);
  instr_add_func_def();
  // parameters
  match(T_L_BRACKET);
  if (next.token_type != T_R_BRACKET)
    error(6, "parser", "program", "Main cannot take parameters");
  match(T_R_BRACKET);
  // returns
  eps = true;
  match(T_L_BRACKET);
  if (!eps) {
    if (next.token_type != T_R_BRACKET)
      error(
        6, "parser", "program",
        "Main cannot have return types"
      );
    match(T_R_BRACKET);
  }
  eps = false;
  // end of func def
  match(T_LEFT_BRACE);
  match(T_EOL);
  // main func body
  body();
  match(T_RIGHT_BRACE);
  match(T_EOL);
  // end of main func
  instr_add_func_end();
  scope_pop();

  // user functions after main
  func_list();

  skip_empty();
  match(T_EOF);
}

void prolog() {
  skip_empty();
  match(T_PACKAGE);
  skip_empty();

  if (next.token_type != T_IDENTIFIER || strcmp(to_string(&next), "main"))
    error(
      2, "parser", "match",
      "Expected: '%s' -- Got: '%s'", T_MAIN, next.token_type
    );

  def = false;
  match(T_MAIN);
  match(T_EOL);
}

void func_list_pre() {
  def = true; eps = true;
  match(T_FUNC_ID);
  if (eps) {
    eps = false;
    return;
  }

  // params
  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);

  // needs to be after declaration of parameters
  // so codegen's POPS have a target
  instr_add_func_def();

  // returns & body
  func_def_type();
  match(T_EOL);

  // end func def
  instr_add_func_end();
  scope_pop();

  match(T_FUNC);
  // check for next func
  func_list_pre();
}

void func_list() {
  eps = true;
  func_def();
  if (eps) {
    eps = false;
    return;
  }

  func_list();
}

void func_def() {
  match(T_FUNC);
  if (eps) return; // checked in func_list

  def = true; eps = false;
  match(T_FUNC_ID);
  instr_add_func_def();

  // parameters
  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);

  func_def_type();
  match(T_EOL);

  // end of func def
  instr_add_func_end();
  scope_pop();
}

void param_list() {
  if (next.token_type != T_IDENTIFIER) return;

  def = true; eps = false;
  match(T_VAR_ID);
  func_add_param(last_func, last_elem->symbol.sym_var_item);
  last_elem->symbol.sym_var_item->is_defined = true;
  type();

  next_param();
}

void next_param() {
  def = true; eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  skip_empty();

  match(T_VAR_ID);
  func_add_param(last_func, last_elem->symbol.sym_var_item);
  last_elem->symbol.sym_var_item->is_defined = true;
  type();

  next_param();
}

void func_def_type() {
  switch( next.token_type ) {
    case T_LEFT_BRACE:
      match(T_LEFT_BRACE);
      match(T_EOL);
      body();
      match(T_RIGHT_BRACE);
      break;

    case T_L_BRACKET:
      match(T_L_BRACKET);
      func_def_ret();
      break;

    default:  // error
      match(T_LEFT_BRACE);
      break;
  }

  // each function definition ends here
  // check whether defined return_list was matched by a return_call
  if (last_func->symbol.sym_func->returns != NULL) {
    if (!has_ret)
      error(
        6, "parser", "func_def_type",
        "Function [%s] with nonempty return list missing a return call",
        *(last_func->key)
      );
  }
}

void func_def_ret() {
  if (next.token_type == T_R_BRACKET) {
    match(T_R_BRACKET);
    match(T_LEFT_BRACE);
    match(T_EOL);
    body();
    match(T_RIGHT_BRACE);
  }
  else {
    if (next.token_type == T_IDENTIFIER) {
      ret_list_def_named();
    } else {
      ret_list_def();
    }
    match(T_R_BRACKET);
    match(T_LEFT_BRACE);
    match(T_EOL);
    body();
    match(T_RIGHT_BRACE);
  }
}

void ret_list_def() {
  type();
  next_ret_def();
}

void next_ret_def() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  skip_empty();

  type();
  next_ret_def();
}

void ret_list_def_named() {
  // add variable
  def = true; eps = false;
  match(T_VAR_ID);
  elem_t * var = last_elem;
  func_add_ret(last_func, var->symbol.sym_var_item);
  var->symbol.sym_var_item->is_defined = true;
  instr_add_var_decl(var);

  // add type & initialize
  type();
  var_init(var);

  next_ret_def_named();
}

void next_ret_def_named() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  skip_empty();

  // add variable
  def = true; eps = false;
  match(T_VAR_ID);
  elem_t * var = last_elem;
  func_add_ret(last_func, var->symbol.sym_var_item);
  var->symbol.sym_var_item->is_defined = true;
  instr_add_var_decl(var);

  // add type & initialize
  type();
  var_init(var);

  next_ret_def_named();
}

void body() {
  last_elem = NULL; // leaving func def header
  skip_empty();

  command();
  if (eps) {
    eps = false;
    return;
  }
  match(T_EOL);

  body();
}

void command() {
  switch (next.token_type) {
    case T_IDENTIFIER: {
      // check for func call
      char * id = to_string(&next);
      elem_t * tmp = try_func(&next);
      if (tmp == NULL) var_(id);
      else if (tmp->sym_type == SYM_FUNC) {
        instr_type_t type = get_func_instr_type(tmp->symbol.sym_func->name);
        instr_add_func_call(tmp, type);
      }
      else error(99, "parser", NULL, "INVALID STATE");

      break;
    }

    // if
    case T_IF:
      if_();
      break;

    // cycle
    case T_FOR:
      cycle();
      break;

    // return
    case T_RETURN:
      return_();
      break;

    default:
      eps = true; // checked in body
      break;
  }
}

// expects ID of variable & following token in [next]
int var_(char * id) { // collect list of dest variables
  // get list of dest variables
  id_list_add(id);
  free(id);
  next_id();

  int operation = next.token_type;

  if (operation == T_DEF_IDENT)
    match(T_DEF_IDENT);
  else if (operation == T_ASSIGNMENT)
    match(T_ASSIGNMENT);
  else error(2, "parser", NULL, "Invalid operation following list of variables");

  // create dest & src lists
  elem_t * dest = make_dest(operation);
  elem_t * src = get_expr_list();

  // handle a := foo()
  if (src->sym_type == SYM_FUNC) {
    // add func call instruction
    instr_type_t type = get_func_instr_type(src->symbol.sym_func->name);
    instr_add_func_call(src, type);

    src->symbol.sym_func->returns = dest->symbol.sym_var_list;

    if (operation == T_DEF_IDENT)
      assign_var_def_types(dest, dest);  // & assign types
    else
      assign_var_move_types(dest, dest);

    return operation;
  }

  if (operation == T_DEF_IDENT)
    assign_var_def_types(dest, src);
  else
    assign_var_move_types(dest, src);

  // add instruction for value assignment
  instr_t * new_move = instr_create();
  instr_set_type(new_move, IC_DEF_VAR);
  instr_add_dest(new_move, dest);
  instr_add_elem1(new_move, src);
  list_add(list, new_move);

  return operation;
}

void next_id() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  skip_empty();

  if (next.token_type != T_IDENTIFIER) {
    match(T_VAR_ID);  // error state
  }

  id_list_add(to_string(&next));
  token_cleanup();
  get_next_token(&next);

  next_id();
}

void if_() {
  // if def
  match(T_IF);
  instr_add_if_def();

  // condition
  elem_t * cond = parse_expression(); // condition handling
  if (cond->symbol.sym_var_item->type != VAR_BOOL)
    error(2, "parser", NULL, "Invalid condition");
  match(T_LEFT_BRACE);
  match(T_EOL);

  // if start instr
  instr_t * if_start = instr_create();
  instr_set_type(if_start, IC_IF_START);
  list_add(list, if_start);

  // if body
  body();
  match(T_RIGHT_BRACE);
  instr_add_if_end();

  if_cont();
}

void if_cont() {
  if (next.token_type != T_ELSE) return;

  match(T_ELSE);
  instr_add_else_start();
  else_();
}

void else_() {
  if (next.token_type != T_LEFT_BRACE) {
    if_();
    instr_add_else_end();
    return;
  }
  match(T_LEFT_BRACE);
  match(T_EOL);
  body();
  match(T_RIGHT_BRACE);
  instr_add_else_end();
}

void cycle() {
  match(T_FOR);

  // initialization
  instr_add_for_def();
  for_def();
  match(T_SEMICOLON);

  // condition
  instr_t * for_cond = instr_create();
  instr_set_type(for_cond, IC_FOR_COND);
  list_add(list, for_cond);
  elem_t * cond = parse_expression(); // condition handling
  if (cond->symbol.sym_var_item->type != VAR_BOOL)
    error(2, "parser", NULL, "Invalid condition");
  match(T_SEMICOLON);

  // step
  for_move();
  match(T_LEFT_BRACE);
  match(T_EOL);

  // body start
  instr_t * for_body_start = instr_create();
  instr_set_type(for_body_start, IC_FOR_BODY_START);
  list_add(list, for_body_start);
  // body is a new scope subordinate to cycle's head
  char * body_scope = malloc(sizeof(char) * 5);
  if (body_scope == NULL) error(99, "parser", NULL, "Memory allocation failed");
  strcpy(body_scope, "body");
  scope_push(body_scope);
  // ----------------------------
  body();
  match(T_RIGHT_BRACE);

  // body end
  instr_t * for_body_end = instr_create();
  instr_set_type(for_body_end, IC_FOR_BODY_END);
  list_add(list, for_body_end);
  scope_pop();  // body
  scope_pop();  // for
}

void for_def() {
  if (next.token_type != T_IDENTIFIER) return;

  // setup for var_()
  char * id = to_string(&next);
  get_next_token(&next);
  var_(id);
}

void for_move() {
  if (next.token_type != T_IDENTIFIER) return;

  // setup for var_move
  char * id = to_string(&next);
  get_next_token(&next);
  int operation = var_(id);
  if (operation == T_DEF_IDENT) {
    error(2, "parser", "for cycle", "Cannot declare in post statement of for loop");
  }
}

void return_() {
  match(T_RETURN);

  sym_var_list_t * rets = last_func->symbol.sym_func->returns;

  elem_t * ret_list = return_list();

  // handling of return list
  if (ret_list == NULL) {
    if (rets == NULL) return;
    // check for named returns
    else if (rets->first->item->name != NULL) {
      // create return list
      sym_var_list_t * r_list = sym_var_list_init();
      symbol_t rl_sym = {.sym_var_list = r_list};
      elem_t * rl_elem = elem_init(SYM_VAR_LIST, rl_sym);
      symtable_insert(symtable, make_unique(rl_elem), rl_elem);

      ret_list = rl_elem;

      // add named returns to list
      for (
        list_item_t * it = rets->first;
        it != NULL;
        it = it->next
      ) {
        list_item_t * list_item = list_item_init(it->item);
        sym_var_list_add(r_list, list_item);
      }
    }
    else {
      error(
        6, "parser", "return call",
        "Function [%s] with unempty return list returns void",
        *(last_func->key)
      );
    }
  } else {
    if (rets->first->item->name != NULL) {
      error(
        6, "parser", "return call",
        "Function [%s]'s return call must be empty -- named returns",
        *(last_func->key)
      );
    }
  }

  // create function element
  char * func_name = malloc(sizeof(char) * (strlen(*(last_func->key)) + 1));
  if (func_name == NULL) error(99, "parser", NULL, "Memory allocation failed");
  strcpy(func_name, *(last_func->key));
  sym_func_t * func_sym = sym_func_init(func_name, NULL, NULL);
  symbol_t func_sym_sym = {.sym_func = func_sym};
  elem_t * func = elem_init(SYM_FUNC, func_sym_sym);
  symtable_insert(symtable, make_unique(func), func);

  // handle "return foo()"
  if ( ret_list != NULL && ret_list->sym_type == SYM_FUNC) {
    // check for empty returns
    if (rets != NULL) {
      // assign defined returns as dests for func_call
      for (
        list_item_t * it = rets->first;
        it != NULL;
        it = it->next
      ) {
        elem_t * expr = create_expr(last_func);
        func_add_ret(func, expr->symbol.sym_var_item);
        func_add_ret(ret_list, expr->symbol.sym_var_item);
      }
    }
    instr_type_t type = get_func_instr_type(ret_list->symbol.sym_func->name);
    instr_add_func_call(ret_list, type);
  } else {
    func->symbol.sym_func->returns = ret_list->symbol.sym_var_list;
  }

  check_ret(func);

  // add return instruction
  instr_t * func_ret = instr_create();
  instr_set_type(func_ret, IC_RET_FUN);
  instr_add_dest(func_ret, func);
  list_add(list, func_ret);
}

elem_t * return_list() {
  if (next.token_type == T_EOL) return NULL;

  elem_t * expr = get_expr_list();
  return expr;
}

elem_t * func_call(char * name) {
  next.token_type = T_IDENTIFIER;
  next.attr.str_lit.str = name;
  def = false; eps = false;
  match(T_FUNC_ID);
  elem_t * func = last_elem;

  // '(' already matched in try_func
  skip_empty();
  func_args();
  match(T_R_BRACKET);

  return func;
}

void func_args() {
  if (next.token_type == T_R_BRACKET) return;

  // stash last function locally
  elem_t * func = last_elem;

  elem_t * arg = parse_expression();
  if (arg->sym_type != SYM_FUNC) {
    func_add_param(func, arg->symbol.sym_var_item);
  } else {
    // create expression
    elem_t * expr = create_expr(arg);

    func_add_ret(arg, expr->symbol.sym_var_item);
    func_add_param(func, expr->symbol.sym_var_item);

    // add func call instruction
    instr_type_t type = get_func_instr_type(arg->symbol.sym_func->name);
    instr_add_func_call(arg, type);
  }

  // restore current func
  last_elem = func;

  next_arg();
}

void next_arg() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  skip_empty();

  // stash last function locally
  elem_t * func = last_elem;
  // get argument
  elem_t * arg = parse_expression();

  if (arg->sym_type != SYM_FUNC) {
    func_add_param(func, arg->symbol.sym_var_item);
  } else {
    // create expression
    elem_t * expr = create_expr(arg);

    func_add_ret(arg, expr->symbol.sym_var_item);
    func_add_param(func, arg->symbol.sym_var_item);

    // add func call instruction
    instr_type_t type = get_func_instr_type(arg->symbol.sym_func->name);
    instr_add_func_call(arg, type);
  }

  next_arg();
}

elem_t * get_expr_list() {
  elem_t * expr = parse_expression();
  if (expr->sym_type == SYM_FUNC) return expr;

  // create var list
  sym_var_list_t * var_list = sym_var_list_init();
  symbol_t var_list_sym = {.sym_var_list = var_list};
  elem_t * var_list_elem = elem_init(SYM_VAR_LIST, var_list_sym);
  symtable_insert(symtable, make_unique(var_list_elem), var_list_elem);
  last_elem = var_list_elem;

  // add first expression
  list_item_t * list_item = list_item_init(expr->symbol.sym_var_item);
  sym_var_list_add(var_list, list_item);

  next_expr();

  return var_list_elem;
}

void next_expr() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  skip_empty();

  elem_t * expr = parse_expression();
  if (expr->sym_type == SYM_FUNC) {
    elem_t * func_expr = create_expr(expr);
    func_add_ret(expr, func_expr->symbol.sym_var_item);
    list_item_t * list_item = list_item_init(expr->symbol.sym_var_item);
    sym_var_list_add(last_elem->symbol.sym_var_list, list_item);
  } else {
    list_item_t * list_item = list_item_init(expr->symbol.sym_var_item);
    sym_var_list_add(last_elem->symbol.sym_var_list, list_item);
  }

  next_expr();
}

void type() {
  if (last_elem == NULL)
    error(99, "parser", "type", "Failed to access last element");

  // get type
  var_type_t type;
  switch (next.token_type) {
    case T_INT:
      type = VAR_INT;
      break;
    case T_FLOAT64:
      type = VAR_FLOAT64;
      break;
    case T_STRING:
      type = VAR_STRING;
      break;
    case T_BOOL:
      type = VAR_BOOL;
      break;
    default:
      error(2, "parser", "type", "Invalid variable type");
      break;
  }

  if (last_elem->sym_type == SYM_VAR_ITEM) {  // assign type to function parameter
    sym_var_item_set_type(last_elem->symbol.sym_var_item, type);
    if (type == VAR_STRING)
      last_elem->symbol.sym_var_item->data.string_t = NULL;
  }
  else { // return list of a function definition
    if (last_func == NULL) {
      error(99, "parser", "type", "Missing function to assign parameter to");
    }

    //create item
    sym_var_item_t * new = sym_var_item_init(NULL);
    sym_var_item_set_type(new, type);
    if (type == VAR_STRING)
      new->data.string_t = NULL;
    symbol_t new_sym = {.sym_var_item = new};
    elem_t * new_elem = elem_init(SYM_VAR_ITEM, new_sym);
    symtable_insert(symtable, make_unique(new_elem), new_elem);

    func_add_ret(last_func, new);
  }

  last_elem = last_func;
  get_next_token(&next);
}
