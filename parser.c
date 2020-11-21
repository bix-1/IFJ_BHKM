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

/*
TODO:
  params, rets

  refactor func comments
  crossreference LL.pdf & implemented rules

  built-in funcs

  handle errors from assignment in:
  - 4.1.1 (4, 3)
  - 4.2   (3,6)
  - 4.3   var_def - 3
          ?? if - 5 (only basic version I think)
          func_call - 6 (2x)
          return - 6
  - 6     (6)
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

/*_____________GLOBAL_VARIABLES_____________*/
extern list_t * list;
extern symtable_t * symtable;
tToken next;
extern struct scope_t scope;


unsigned long U_ID = 0; // unique id
#define U_ID_LENGTH 20
char u_id[U_ID_LENGTH + 1] = "";

/*_____________SYMTABLE_HANDLING____________*/
elem_t * last_func;
elem_t * last_elem;


/*_______________GLOBAL_FLAGS_______________*/
// outside match():
//    for allowing eps terminal
// inside of match():
//    for checking whether EPS or NON-EPS terminal matched
bool eps = false;
// outside match():
//    for allowing definiton
// inside of match():
//    for checking whether ID is being defined or called
bool def = false;


/*_______________MAIN_FUNCTION______________*/
void parse() {
  // initialization
  source_file_setup(stdin);
  get_next_token(&next);
  list = list_create();
  symtable = symtable_init(100);
  scope_init();
  func_defs_init();

  printf("\n\n___________SYMTABLE:____________\n");
  printf("________________________________\n");

  // parsing
  program();


  // TODO delete
  // debugging instructions
  printf("\n\n___________FUNCTIONS:___________\n");
  printf("________________________________\n");
  for (
    instr_t * tmp = list_get_active(list);
    tmp != NULL;
    tmp = list_get_next(list)
  ) {
    switch (instr_get_type(tmp)) {
      case IC_DEF_FUN:
        printf("DEF_FUN\t  %s\n", *(instr_get_dest(tmp)->key));
        sym_var_list_t * params = instr_get_dest(tmp)->symbol.sym_func->params;
        if (params != NULL) {
          printf("\t\t__PARAMS:\n");
          for (
            sym_var_item_t * tmp = params->first;
            tmp != NULL;
            tmp = sym_var_list_next(params)
          )
          printf("\t\t  %s\n", tmp->name);
        }

        sym_var_list_t * rets = instr_get_dest(tmp)->symbol.sym_func->returns;
        if (rets != NULL) {
        printf("\t\t__RETURNS:\n");
        for (
          sym_var_item_t * tmp = rets->first;
          tmp != NULL;
          tmp = tmp->next
        ) {
          printf("\t\t");
          switch (tmp->type) {
            case VAR_INT:
            printf("INT");
            break;
            case VAR_FLOAT64:
            printf("FLOAT64");
            break;
            case VAR_STRING:
            printf("STRING");
            break;
            case VAR_BOOL:
            printf("BOOL");
            break;
            default:
            printf("Invalid return type");
            exit(1);
            break;
          }
          printf("\n");
        }
      }
      break;
      case IC_END_FUN:
        printf("_________________________END_FUN: ");
        printf("%s\n", *(instr_get_dest(tmp)->key));
      break;
      case IC_DECL_VAR:
        printf("\tDECL_VAR");
        printf("\t  %s", *(instr_get_dest(tmp)->key));
      break;
      case IC_DEF_VAR:
        printf("\tDEF_VAR\t    ");
        sym_var_list_t * l = instr_get_dest(tmp)->symbol.sym_var_list;
        for (
          sym_var_item_t * it = sym_var_list_get_active(l);
          it != NULL;
          it = sym_var_list_next(l)
        ) {
          printf("%s, ", it->name);
        }
          printf("  ===  ");
      break;
      case IC_IF_DEF:
        printf("\n\tIF");
      break;
      case IC_IF_START:
        printf("\t__ifstart\n");
      break;
      case IC_IF_END:
        printf("\n\t____________ENDIF\n");
      break;
      case IC_ELSE_START:
        printf("\n\tELSE\n");
      break;
      case IC_ELSE_END:
        printf("\n\t____________END_ELSE\n");
      break;
      case IC_FOR_DEF:
        printf("\n\tFOR\n");
      break;
      case IC_FOR_COND:
        printf("\n\t__for_cond\n");
      break;
      case IC_FOR_STEP:
        printf("\n\t__for_step\n");
      break;
      case IC_FOR_BODY_START:
        printf("\n\t__for_body_start\n");
      break;
      case IC_FOR_BODY_END:
        printf("\n\t____________END_FOR\n");
      break;
      case IC_CALL_FUN:
        printf("\tFUNC CALL\t%s\n", instr_get_elem1(tmp)->symbol.sym_func->name);
      break;
      case IC_RET_FUN:
        // TODO
      break;
      case IC_READ_VAR:
        printf("\tINPUT_FUNC\n");
      break;


      default:
        printf("_instr type [%d] not implemented_", instr_get_type(tmp));
      break;
    }
    printf("\n");
  }

  func_defs_check();

  // termination
  scope_destroy();
  func_defs_destroy();
  list_destroy(&list);
  symtable_free(symtable);
}


/*__________________TOKENS__________________*/
void token_cleanup() {
  if (
    ( next.token_type == T_IDENTIFIER ||
      next.token_type == T_MAIN ||
      next.token_type == T_VAR_ID ||
      next.token_type == T_FUNC_ID
    ) && next.attr.str_lit.str != NULL
  ) {
    free(next.attr.str_lit.str);
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
      if (next.token_type != T_IDENTIFIER) {
        if (eps) return;
        goto default_err;
      }
      if (strcmp(to_string(&next), "main")) {
        if (eps) return;
        error(3, "parser", "match", "Missing main function");
      }

      // not changing scope -- in prolog: (package main)
      if (!def) {
        token_cleanup();
        break;
      }

      // create func
      sym_func_t * func_sym = sym_func_init(to_string(&next), NULL, NULL);
      symbol_t func = {.sym_func = func_sym};
      elem_t * func_data = elem_init(SYM_FUNC, func);
      // add to symtable
      symtable_iterator_t tmp =
        symtable_insert(symtable, to_string(&next), func_data);
      last_func = symtable_iterator_get_value(tmp);
      // add to scope
      char * new_scope = malloc(sizeof(char) * 5);
      if (new_scope == NULL) error(99, "parser", "", "Memory allocation failed");
      strcpy(new_scope, to_string(&next));
      U_ID = 0; // new founc -- can reset counter
      scope_push(new_scope);

      break;

    case T_FUNC:
      if (next.token_type != T_FUNC) {
        if (eps) return;
        if (next.token_type != T_EOF) goto default_err;
        else
          error( 3, "parser", "program", "Missing function main" );
      }
      break;

    case T_FUNC_ID:
      if (next.token_type != T_IDENTIFIER) {
        if (eps) {
          def = false;
          return;
        }
        goto default_err;
      }
      if (!strcmp( to_string(&next), "main")) {
        if (eps) {
          def = false;
          return;
        }
        next.token_type = T_MAIN;
        goto default_err;
      }

      // check for built-in function
      if (built_in()) break;

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
          // create func
          sym_func_t * func_sym = sym_func_init(to_string(&next), NULL, NULL);
          symbol_t func = {.sym_func = func_sym};
          elem_t * func_data = elem_init(SYM_FUNC, func);
          // add to symtable
          symtable_iterator_t tmp =
            symtable_insert(symtable, to_string(&next), func_data);
          last_func = symtable_iterator_get_value(tmp);
          // add to scope
          char * new_scope = malloc(sizeof(char) * (strlen(to_string(&next))+1) );
          if (new_scope == NULL) error(99, "parser", "", "Memory allocation failed");
          strcpy(new_scope, to_string(&next));
          U_ID = 0; // new founc -- can reset counter
          scope_push(new_scope);
          last_elem = last_func;

          printf("Added ---%s\n", to_string(&next));
        }
      }
      else { // func call expected
        // create func elem
        sym_func_t * func_sym = sym_func_init(to_string(&next), NULL, NULL);
        symbol_t func = {.sym_func = func_sym};
        elem_t * func_elem = elem_init(SYM_FUNC, func);
        // add to symtable
        char * index = get_unique();
        char i_name[strlen(index) + strlen(to_string(&next)) + 1];
        strcpy(i_name, index);
        strcat(i_name, to_string(&next));
        char * func_name = id_add_scope(scope_get_head(), i_name);
        symtable_insert(symtable, func_name, func_elem);
        // create instruction
        instr_t * func_call = instr_create();
        instr_set_type(func_call, IC_CALL_FUN);
        instr_add_elem1(func_call, func_elem);
        list_add(list, func_call);
        last_elem = func_elem;

        func_defs_add(func_elem);

        printf("\t---func call:\t%s\t\t\n", func_name);
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
          // create var
          sym_var_item_t * var_item = sym_var_item_init(to_string(&next));
          symbol_t var_sym = {.sym_var_item = var_item};
          elem_t * var = elem_init(SYM_VAR_ITEM, var_sym);
          // add to symtable
          symtable_insert(symtable, id, var);

          printf("\t---%s\n", id);

          // check if var is parameter of func
          if (last_elem != NULL && last_elem->sym_type == SYM_FUNC ) {
            last_elem = var;
            func_add_param();
          } else {
            last_elem = var;
            instr_add_var_decl();
          }
        }
      }
      else {  // var_call expected
        id_find(scope_get_head(), to_string(&next));
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

  get_next_token(&next);
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
  if (tmp == NULL) {
    new->name = malloc(sizeof(char) * (strlen(new_name) + 2));
    strcpy(new->name, new_name);
    strcat(new->name, ":");
  } else {
    new->name = malloc(
      sizeof(char) *
      (strlen(tmp->name) + strlen(new_name) + 2)
    );
    strcpy(new->name, tmp->name);
    strcat(new->name, new_name);
    strcat(new->name, ":");
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

char * scope_get() {
  if (scope.first == NULL) return NULL;
  return scope.first->name;
}

char * id_add_scope(scope_elem_t * tmp_scope, char * old_id) {
  char * scope_name = tmp_scope->name;
  char * new_id = malloc(
    sizeof(char) *  // prefix + : + name + \0
    (strlen(scope_name) + strlen(old_id) + 2)
  );
  if (new_id == NULL) error(99, "parser", "", "Memory allocation failed");
  new_id[0] = '\0';
  strcat(new_id, scope_name);
  strcat(new_id, old_id);

  return new_id;
}

void id_find(scope_elem_t *scope, char *old_id) {
  char * id;
  scope_elem_t * tmp_scope = scope;
  symtable_iterator_t it;

  // look through all higher contexts
  while (true) {
    // check for end of scope
    if (tmp_scope == NULL) {
      if (eps) return;
      else error(3, "parser", "match", "Variable \"%s\" undefined", to_string(&next));
    }
    // get variable's contextual ID
    id = id_add_scope(tmp_scope, old_id);
    // look for in symtable
    it = symtable_find(symtable, id);
    free(id);
    if (symtable_iterator_valid(it)) {  // found in symtable
      last_elem = symtable_iterator_get_value(it);
      token_cleanup();
      break;
    } else {  // move to the next
      tmp_scope = tmp_scope->next;
    }
  }
}

char * get_unique() {
  sprintf(u_id, "%ld", U_ID++);
  return u_id;
}


void skip_empty() {
  eps = true;
  match(T_EOL);
  eps = false;
}


// additional instructions-related functions
void instr_add_func_def() {
  instr_t * new_func = instr_create();
  instr_set_type(new_func, IC_DEF_FUN);
  instr_add_dest(new_func, last_func);
  list_add(list, new_func);
}

void instr_add_func_end() {
  instr_t * end_func = instr_create();
  instr_set_type(end_func, IC_END_FUN);
  instr_add_dest(end_func, last_func);
  list_add(list, end_func);
}

void instr_add_var_decl() {
  instr_t * new_var = instr_create();
  instr_set_type(new_var, IC_DECL_VAR);
  instr_add_dest(new_var, last_elem);
  list_add(list, new_var);
}

void instr_add_var_def() {
  // create var & expr lists
  sym_var_list_t * dest_list = sym_var_list_init();
  sym_var_list_t * src_list = sym_var_list_init();
  symbol_t dest_sym = {.sym_var_list = dest_list};
  symbol_t src_sym = {.sym_var_list = src_list};
  elem_t * dest_elem = elem_init(SYM_VAR_LIST, dest_sym);
  elem_t * src_elem = elem_init(SYM_VAR_LIST, src_sym);
  // create instr & add lists
  instr_t * var_def = instr_create();
  instr_set_type(var_def, IC_DEF_VAR);
  instr_add_dest(var_def, dest_elem);
  instr_add_elem1(var_def, src_elem);
  list_add(list, var_def);
}

void instr_var_list_append_dest() {
  sym_var_list_add(
    instr_get_dest(list->last)->symbol.sym_var_list,
    last_elem->symbol.sym_var_item
  );
}

void instr_var_list_append_src() {
  sym_var_list_add(
    instr_get_elem1(list->last)->symbol.sym_var_list,
    last_elem->symbol.sym_var_item
  );
}

void instr_add_if_def() {
  instr_t * new_if = instr_create();
  instr_set_type(new_if, IC_IF_DEF);
  list_add(list, new_if);

  char * prefix = get_unique();
  char * if_scope = malloc(sizeof(char) * (strlen(prefix) + 3)); // u_id + if\0
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

  char * prefix = get_unique();
  char * else_scope = malloc(sizeof(char) * (strlen(prefix) + 5)); // u_id + else\0
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

  char * prefix = get_unique();
  char * for_scope = malloc(sizeof(char) * (strlen(prefix) + 4)); // u_id + for\0
  strcpy(for_scope, prefix);
  strcat(for_scope, "for");
  scope_push(for_scope);
}


// func functions
void func_add_param() {
  if (last_func == NULL || last_elem == NULL)
    error(99, "parser", "type", "Failed to access function's parameters");

  sym_var_list_t ** params = &(last_func->symbol.sym_func->params);
  if (params == NULL)
    error(99, "parser", "type", "Failed to access function's parameters");
  if (*params == NULL)  // param list empty
    *params = sym_var_list_init();

  // add to param list
  sym_var_list_add(*params, last_elem->symbol.sym_var_item);
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
  new->func = func;
  new->next = func_defs.first;
  func_defs.first = new;
}

void func_defs_check() {
  printf("\n\n\n===============================\n\n");
  func_def_t * tmp = func_defs.first;
  char * name;
  while (tmp != NULL) {
    name = tmp->func->symbol.sym_func->name;
    symtable_iterator_t it = symtable_find(symtable, name);
    if (symtable_iterator_valid(it)) {
      printf("FUNC\t%s\n", name);
      // TODO



      printf("\n");
    } else {
      error(3, "parser", "func_check", "Function '%s' called but not defined", name);
    }
    tmp = tmp->next;
  }
}


bool built_in() {
  instr_t * instr = instr_create();
  char * func = to_string(&next);
  int type;
  if (
    !strcmp(func, "inputs") ||  // string
    !strcmp(func, "inputi") ||  // int
    !strcmp(func, "inputf") ||  // float
    !strcmp(func, "inputb")     // bool
  ) {
    type = IC_READ_VAR;
    instr_add_dest(instr, last_elem);
  } else if (!strcmp(func, "print")) {
    type = IC_WRITE_VAR;
    // instr_add_dest(instr, ); // TODO ... somewhere
  } else {
    free(instr);
    return false;
  }
  token_cleanup(); // get rid of func name

  instr_set_type(instr, type);
  list_add(list, instr);
  return true;
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
  instr_add_func_def();
  printf("Added ---MAIN\n"); // TODO delete
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
  // skip all empty space to T_PACKAGE
  skip_empty();
  // match _prolog main_
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
  instr_add_func_def();
  // params
  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);
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
  if (eps) { // test of func_def
    eps = false;
    return;
  }
  // check for next func
  func_list();
}

void func_def() {
  match(T_FUNC);
  if (eps) return; // from func_list
  def = true; eps = false;
  match(T_FUNC_ID);
  instr_add_func_def();

  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);

  func_def_type();
  match(T_EOL);
  // end func def
  instr_add_func_end();
  scope_pop();
}

void param_list() {
  def = true; eps = true;
  match(T_VAR_ID);
  if (eps) {
    eps = false;
    return;
  }
  type();
  next_param();
}

void next_param() {
  def = true; eps = true;
  match(T_COMMA);
  if (eps) return;
  match(T_VAR_ID);
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
    ret_list_def();
    match(T_R_BRACKET);
    match(T_LEFT_BRACE);
    match(T_EOL);
    body();
    match(T_RIGHT_BRACE);
  }
}

void ret_list_def() {
  // adding rets of func
  type();
  next_ret_def();

  // TODO delete
  sym_var_list_t * rets = last_func->symbol.sym_func->returns;
  if (rets == NULL) return;
  printf("\t\tRETURNS:\n");
  printf("\t\t---");
  for (
    sym_var_item_t * tmp = sym_var_list_get_active(rets);
    tmp != NULL;
    tmp = sym_var_list_next(rets)
  ) {
    switch (tmp->type) {
      case VAR_INT:
        printf("INT, ");
        break;
      case VAR_FLOAT64:
        printf("FLOAT64, ");
        break;
      case VAR_STRING:
        printf("STRING, ");
        break;
      case VAR_BOOL:
        printf("BOOL, ");
        break;
      default:
        printf("Invalid return type");
        exit(1);
        break;
    }
  }
  printf("\n");
}

void next_ret_def() {
  eps = true;
  match(T_COMMA);
  if (eps) return;
  type();
  next_ret_def();
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
    case T_IDENTIFIER: {  // var_ OR func_call
      // identifier (current token) must be stashed
      char * id_stash = to_string(&next);

      get_next_token(&next);
      int type = next.token_type;
      next.token_type = T_IDENTIFIER;
      next.attr.str_lit.str = id_stash;

      switch (type) {
        case T_L_BRACKET:
          ungetc('(', source);
          func_call();
          break;

        case T_DEF_IDENT:
          ungetc('=', source);
          ungetc(':', source);
          var_def();
          break;

        case T_ASSIGNMENT:
          ungetc('=', source);
          var_move();
          break;

        case T_COMMA:
          ungetc(',', source);
          var_move();
          break;

        default:
          error(
            2, "parser", "match",
            "Expected: Function call or variable definition/assignment -- Got: '%s'",
            next.token_type
          );
          break;
      }
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
      eps = true;
      break;
  }
}

void var_def() {
  def = true; eps = false;
  match(T_VAR_ID);
  match(T_DEF_IDENT);
  parse_expression();
}

void var_move() {
  def = false; eps = false;
  match(T_VAR_ID);

  // instr_add_var_def();
  // instr_var_list_append_dest();

  // TODO sym_var_list

  next_id();
  if (next.token_type == T_DEF_IDENT)
    error(
      3, "parser", "var_move", "Redefinition of variable \"%s\"",
      last_elem->symbol.sym_var_item->name
    );
  match(T_ASSIGNMENT);
  expr_list();
}

void next_id() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  def = false; eps = false;
  match(T_VAR_ID);
  instr_var_list_append_dest();
  next_id();
}

void if_() {
  // if def
  match(T_IF);
  instr_add_if_def();
  // condition
  parse_expression(); // condition handling
  match(T_LEFT_BRACE);
  match(T_EOL);

    // if start instr
  instr_t * if_start = instr_create();
  instr_set_type(if_start, IC_IF_START);
  list_add(list, if_start);

  // commands
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
  parse_expression(); // expression parser call
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
  if (next.token_type == T_SEMICOLON) return;
  var_def();
}

void for_move() {
  if (next.token_type == T_LEFT_BRACE) return;
  // create instruction
  instr_t * for_step = instr_create();
  instr_set_type(for_step, IC_FOR_STEP);
  list_add(list, for_step);

  var_move();
}

void return_() {
  match(T_RETURN);
  return_list();
}

void return_list() {
  if (next.token_type == T_EOL) return;

  // TODO add type checking
  parse_expression();
  next_ret();
}

void next_ret() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  // TODO add type checking
  parse_expression();
  next_ret();
}

void func_call() {
  def = false; eps = false;
  match(T_FUNC_ID);
  match(T_L_BRACKET);
  func_args();
  match(T_R_BRACKET);
}

void func_args() {
  if (next.token_type == T_R_BRACKET) return;

  // TODO param assigning

  parse_expression();
  next_expr();
}

void expr_list() {
  // TODO comment this segment
  if (next.token_type == T_IDENTIFIER) { // check if func call
    // get next char
    int c;
    // TODO add skip comments
    while ((c = getc(source)) == ' ');
    ungetc(c, source);


    if (c == '(') {
      func_call();
      if (next.token_type != T_COMMA) { // x = foo()
        printf("\t\t\t\t\tsingle func\n\n");



      } else {
        printf("\t\t\t\t\tfunc as expr\n\n");
      }
    }
  }

  // eps = true;
  eps = false;  // TODO unfuck this

  // TODO check whether expr || func

  parse_expression(); // expression handling
  if (!eps) next_expr();  // expr matched
  else      func_call();
}

void next_expr() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  parse_expression(); // expression handling
  next_expr();
}

void type() {
  if (last_elem == NULL)
    error(99, "parser", "type", "Failed to access last element");

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
      error(1, "parser", "type", "Invalid variable type");
      break;
  }


  if (last_elem->sym_type == SYM_VAR_ITEM) {  // assign type to last element
    sym_var_item_set_type(last_elem->symbol.sym_var_item, type);
    if (type == VAR_STRING)
      last_elem->symbol.sym_var_item->data.string_t = NULL;
  }
  else { // return list of function
    if (last_func == NULL)
      error(99, "parser", "type", "Missing function to assign parameter to");

    // get returns
    sym_var_list_t ** rets = &(last_func->symbol.sym_func->returns);
    if (rets == NULL)
      error(99, "parser", "type", "Failed to access function's returns");
    if (*rets == NULL)  // ret list empty
      *rets = sym_var_list_init();

    //create item
    sym_var_item_t * new = sym_var_item_init(NULL);
    sym_var_item_set_type(new, type);
    if (type == VAR_STRING)
      new->data.string_t = NULL;
    // add to return list
    sym_var_list_add(*rets, new);
  }

  last_elem = last_func;
  get_next_token(&next);
}
