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
  finalize try_var_or_func
  --& all its occurences



  mem leaks with undefined vars for func_call
  func call as return

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

// for checking whether function with defined return types
// contains at least a single return call
bool has_ret = false;


/*_______________MAIN_FUNCTION______________*/
void parse() {
  // initialization
  source_file_setup(stdin);
  get_next_token(&next);
  list = list_create();
  symtable = symtable_init(100);
  scope_init();
  func_defs_init();

  add_built_in();

  printf("\n\n___________SYMTABLE:____________\n");
  printf("________________________________\n");

  // parsing
  program();


  // TODO delete
  // debugging instructions
  printf("\n\n_________INSTRUCTIONS:__________\n");
  printf("________________________________\n");
  for (
    instr_t * tmp = list_get_active(list);
    tmp != NULL;
    tmp = list_get_next(list)
  ) {
    switch (instr_get_type(tmp)) {
      case IC_DEF_FUN: {
        printf("DEF_FUN\t  %s\n", *(instr_get_dest(tmp)->key));
        sym_var_list_t * params = instr_get_dest(tmp)->symbol.sym_func->params;
        if (params != NULL) {
          printf("\t\t__PARAMS:\n");
          for (
            list_item_t * tmp = params->first;
            tmp != NULL;
            tmp = tmp->next
          ) {
            printf("\t\t  %s\n", tmp->item->name);
          }
        }

        sym_var_list_t * rets = instr_get_dest(tmp)->symbol.sym_func->returns;
        if (rets != NULL) {
          printf("\t\t__RETURNS:\n");
          sym_var_item_t * tmp;
          for (
            list_item_t * it = rets->first;
            it != NULL;
            it = it->next
          ) {
          tmp = it->item;
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
        break; }
      case IC_END_FUN:
        printf("_________________________END_FUN: ");
        printf("%s\n", *(instr_get_dest(tmp)->key));
        break;
      case IC_DECL_VAR:
        printf("\tDECL_VAR");
        printf("\t  %s    ===    ", *(instr_get_dest(tmp)->key));
        // NOTE doesn't work for indirect assignment
        /*
        if (
          (instr_get_type(tmp->next) >= IC_ADD_VAR &&
          instr_get_type(tmp->next) <= IC_STR2INT_VAR) ||
          (instr_get_type(tmp->next) >= IC_CONCAT_STR &&
          instr_get_type(tmp->next) <= IC_SETCHAR_STR)
        ) break;
        sym_var_item_t * item = instr_get_elem1(tmp)->symbol.sym_var_item;
        switch(item->type) {
          case VAR_INT:
            printf("%d", item->default_data.int_t);
          break;
          case VAR_FLOAT64:
            printf("%f", item->default_data.float64_t);
          break;
          case VAR_STRING:
            printf("%s", item->default_data.string_t);
          break;
          case VAR_BOOL:
            if (item->default_data.bool_t == true) printf("true");
            else printf("false");
          break;
          default:
          break;
        }
        */
        break;
      case IC_DEF_VAR:
        printf("\tDEF_VAR\t\t");
        sym_var_list_t * L = instr_get_dest(tmp)->symbol.sym_var_list;
        for (
          list_item_t * tmp = L->first;
          tmp != NULL;
          tmp = tmp->next
        ) {
          printf("%s, ", tmp->item->name);
          // printf("%d\n\n", instr_get_elem1(tmp)->symbol.sym_var_list->first->item->default_data.int_t);
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
        printf("\tFUNC CALL\t%s\n", instr_get_dest(tmp)->symbol.sym_func->name);
        // printf("%s\n", instr_get_dest(tmp)->symbol.sym_func->returns->first->item->name);
        break;
      case IC_RET_FUN: {
        printf("\tRET %s    ", instr_get_dest(tmp)->symbol.sym_func->name);
        sym_var_list_t * rets = instr_get_dest(tmp)->symbol.sym_func->returns;
        if (rets == NULL) {
          printf("void");
          break;
        }
        for (
          list_item_t * tmp = rets->first;
          tmp != NULL;
          tmp = tmp->next
        ) {
          switch (tmp->item->type) {
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
          printf(", ");
        }
        break; }
      case IC_READ_VAR:
        printf("\tINPUT_FUNC\n");
        break;
      case IC_ADD_VAR:
        printf("\t++ADD");
        break;
      case IC_SUB_VAR:
        printf("\t--SUB");
        break;
      case IC_MUL_VAR:
        printf("\t**MUL");
        break;
      case IC_DIV_VAR:
        printf("\t//DIV");
        break;
      case IC_LT_VAR:
        printf("\t<< LT");
        break;
      case IC_GT_VAR:
        printf("\t>> GT");
        break;
      case IC_EQ_VAR:
        printf("\t== EQ");
        break;
      case IC_AND_VAR:
        printf("\t&& AND");
        break;
      case IC_OR_VAR:
        printf("\t|| OR");
        break;
      case IC_NOT_VAR:
        printf("\t!! NOT");
        break;
      case IC_WRITE_VAR:
        printf("\tWRITE_VAR");
        // printf("\t\t\t%d\n", instr_get_dest(tmp)->symbol.sym_var_list->first->item->default_data.int_t);
        break;
      case IC_INT2FLOAT_VAR:
        printf("\tINT2FLOAT");
        // printf("\t%d --> ", instr_get_elem1(tmp)->symbol.sym_var_list->first->item->default_data.int_t);
        break;
      case IC_FLOAT2INT_VAR:
        printf("\tFLOAT2INT");
        break;

      default:
        printf("_instr type [%d] not implemented_", instr_get_type(tmp));
      break;
    }
    printf("\n");
  }

  func_defs_check();

  printf("\n\n");

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
      char * name_old = to_string(&next);
      char * name_new = malloc(sizeof(char) * (strlen(name_old) + 1));
      if (name_new == NULL) error(99, "parser", NULL, "Memory allocation failed");
      strcpy(name_new, name_old);
      sym_func_t * func_sym = sym_func_init(name_new, NULL, NULL);
      symbol_t func = {.sym_func = func_sym};
      elem_t * func_data = elem_init(SYM_FUNC, func);
      // add to symtable
      symtable_insert(symtable, name_old, func_data);
      last_func = func_data;
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
          char * name_old = to_string(&next);
          char * name_new = malloc(sizeof(char) * (strlen(name_old) + 1));
          if (name_new == NULL) error(99, "parser", NULL, "Memory allocation failed");
          strcpy(name_new, name_old);
          sym_func_t * func_sym = sym_func_init(name_new, NULL, NULL);
          symbol_t func = {.sym_func = func_sym};
          elem_t * func_data = elem_init(SYM_FUNC, func);
          // add to symtable
          symtable_insert(symtable, name_old, func_data);
          last_func = func_data;
          // add to scope
          char * new_scope = malloc(sizeof(char) * (strlen(to_string(&next))+1) );
          if (new_scope == NULL) error(99, "parser", NULL, "Memory allocation failed");
          strcpy(new_scope, to_string(&next));
          U_ID = 0; // new founc -- can reset counter
          scope_push(new_scope);
          last_elem = last_func;

          printf("Added ---%s\n", to_string(&next));
        }
      }
      else { // func call expected
        instr_type_t type = get_func_instr_type(to_string(&next));
        instr_add_func_call(type);

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
          if (last_elem != NULL && last_elem->sym_type == SYM_FUNC) {
            func_add_param(last_elem, var->symbol.sym_var_item);
            last_elem = var;
          } else {
            last_elem = var;
            instr_add_var_decl();
          }
        }
      }
      else {  // var_call expected
        elem_t * elem = id_find(scope_get_head(), to_string(&next));
        token_cleanup(); // elem found & token.attr no longer needed

        if (last_elem->sym_type == SYM_VAR_LIST) {
          list_item_t * list_item = list_item_init(elem->symbol.sym_var_item);
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
  if (tmp_scope == NULL) {
    char * new_id = malloc(sizeof(char) * (strlen(old_id) + 1));
    if (new_id == NULL) error(99, "parser", NULL, "Memory allocation failed");
    strcpy(new_id, old_id);
    return new_id;
  }

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
      else error(3, "parser", "match", "Variable \"%s\" undefined", old_id);
    }
    // get variable's contextual ID
    id = id_add_scope(tmp_scope, old_id);
    // look for in symtable
    it = symtable_find(symtable, id);
    free(id);
    if (symtable_iterator_valid(it)) {  // found in symtable
      eps = false;
      return symtable_iterator_get_value(it);
    } else {  // move to the next
      tmp_scope = tmp_scope->next;
    }
  }
}

char * get_unique() {
  sprintf(u_id, "%ld", U_ID++);
  return u_id;
}

char * make_unique(elem_t * elem) {
  char type[5] = "";  // TODO const
  switch(elem->sym_type) {
    case SYM_VAR_LIST:
      strcpy(type, "list");
      break;
    default:
      return NULL;
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

void instr_add_func_call(instr_type_t type) {
  // create function element
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
  instr_set_type(func_call, type);
  instr_add_dest(func_call, func_elem);
  list_add(list, func_call);
  last_elem = func_elem;

  printf("\t---func call:\t%s\t\t\n", func_name);
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

void instr_add_ret() {
  // create return instruction
  instr_t * func_ret = instr_create();
  instr_set_type(func_ret, IC_RET_FUN);

  // create function element
  char * func_name = malloc(sizeof(char) * (strlen(*(last_func->key)) + 1));
  strcpy(func_name, *(last_func->key));
  sym_func_t * func_sym = sym_func_init(func_name, NULL, NULL);
  symbol_t func = {.sym_func = func_sym};
  elem_t * func_elem = elem_init(SYM_FUNC, func);
  // add to symtable -- for cleanup
  char * index = get_unique();
  char * name = func_sym->name;
  char * id = malloc(sizeof(char) * (strlen(index) + strlen(name) + 1));
  strcpy(id, index);
  strcat(id, name);
  symtable_insert(symtable, id, func_elem);
  // add to instruction
  instr_add_dest(func_ret, func_elem);
  last_elem = func_elem;

  list_add(list, func_ret);
}


void check_var_def_types(elem_t * dest_elem, elem_t * src_elem) {
  // check whether expression types match
  // their corresponding variables

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
        7, "parser", "check_var_def_types",
        "Failed to assign [%d] expressions to [%d] variables",
        n_src, n_dest
      );
    }

    dest = dest_item->item;
    src = src_item->item;

    if (dest->type != src->type)
      error(
        7, "parser", "check_var_def_types",
        "Variable [%s] and Expression [n.%d] are of different data types",
        dest->name, n_dest
      );

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
      7, "parser", "check_var_def_types",
      "Failed to assign [%d] expressions to [%d] variables",
      n_src, n_dest
    );
  }
}

void add_next_expr() {
  elem_t * expr = parse_expression();
  list_item_t * list_item = list_item_init(expr->symbol.sym_var_item);
  sym_var_list_add(last_elem->symbol.sym_var_list, list_item);
}


#define NAME_MAX_L 8 // strlen("FLOAT64") + \0
void check_ret(elem_t * func_rets) {
  // check whether return types match the definition

  // returns setup
  sym_var_list_t * ret_list = func_rets->symbol.sym_func->returns;

  // definitions setup
  elem_t * func_defs = symtable_iterator_get_value(
    symtable_find(symtable, func_rets->symbol.sym_func->name)
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

    if (def->item->type != ret->item->type) {
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

void check_func_call_args(elem_t * def_e, elem_t * call_e) {
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

    if (def->item->type != call->item->type) {
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

    if (def->item->type != call->item->type) {
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

      // sym_var_list_free(call_params);
      error(
        6, "parser", "function call assignment",
        "Assignment to [%s] where [%s] was defined for return [%d] of function [%s]",
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
      6, "parser", "check_func_call_rets",
      "Number of func_call destinations [%d] differs from defined [%d] returns of function [%s]",
      n_call, n_def, *(def_e->key)
    );
  }
}

sym_var_list_t * new_sym_var_list() {
  sym_var_list_t * var_list = sym_var_list_init();
  symbol_t var_list_sym = {.sym_var_list = var_list};
  elem_t * dest = elem_init(SYM_VAR_LIST, var_list_sym);
  symtable_insert(symtable, make_unique(dest), dest);
  return var_list;
}

// func functions
void func_add_param(elem_t *func, sym_var_item_t *param) {
  if (func == NULL || param == NULL)
    error(99, "parser", "type", "Failed to access function's parameters");

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

  sym_var_list_t ** rets = &(func->symbol.sym_func->returns);
  if (rets == NULL)
    error(99, "parser", "func_add_ret", "Failed to access function's returns");
  if (*rets == NULL)  // param list empty
    *rets = new_sym_var_list();

  // add to ret list
  list_item_t * list_item = list_item_init(ret);
  sym_var_list_add(*rets, list_item);
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
  printf("\n\n__________FUNC_CALLS:___________\n");
  printf("________________________________\n");
  func_def_t * func_call = func_defs.first;
  char * name;
  while (func_call != NULL) {
    name = func_call->func->symbol.sym_func->name;
    symtable_iterator_t it = symtable_find(symtable, name);
    if (symtable_iterator_valid(it)) {
      printf("FUNC\t%s\n\n", name);
      elem_t * elem = symtable_iterator_get_value(it);
      if (!strcmp(name, "print")) {
        // no need to check arguments -- print can take any ammount of anything
        check_func_call_rets(elem, func_call->func);
        break;
      }
      check_func_call_args(elem, func_call->func);
      check_func_call_rets(elem, func_call->func);
    } else {
      error(3, "parser", "func_check", "Function '%s' called but not defined", name);
    }
    func_call = func_call->next;
  }
}


enum built_in_names {
  INPUTS, INPUTI, INPUTF, INPUTB,
  PRINT,
  INT2FLOAT, FLOAT2INT,
  LEN, SUBSTR, ORD, CHR
};

void func_def_add_ret(elem_t * func, var_type_t type) {
  sym_var_item_t * ret = sym_var_item_init(NULL);
  sym_var_item_set_type(ret, type);
  if (type == VAR_STRING) ret->data.string_t = NULL;

  // add to symtable for cleanup
  symbol_t ret_sym = {.sym_var_item = ret};
  elem_t * ret_elem = elem_init(SYM_VAR_ITEM, ret_sym);
  char * func_name = *(func->key);
  char * ret_id = malloc(sizeof(char) * (strlen(func_name) + 6));
  strcpy(ret_id, func_name);
  strcat(ret_id, ":");
  if (func->symbol.sym_func->returns == NULL) strcat(ret_id, "0ret");
  else strcat(ret_id, "1ret");

  symtable_insert(symtable, ret_id, ret_elem);

  func_add_ret(func, ret);
}

void func_def_add_param(elem_t * func, var_type_t type) {
  sym_var_item_t * param = sym_var_item_init(NULL);
  sym_var_item_set_type(param, type);
  if (type == VAR_STRING) param->data.string_t = NULL;

  // add to symtable for cleanup
  symbol_t param_sym = {.sym_var_item = param};
  elem_t * param_elem = elem_init(SYM_VAR_ITEM, param_sym);
  char * func_name = *(func->key);
  char * param_id = malloc(sizeof(char) * (strlen(func_name) + 6));
  strcpy(param_id, func_name);
  strcat(param_id, ":");

  // TODO add real cycle
  if (func->symbol.sym_func->params == NULL) strcat(param_id, "0prm");
  else if (func->symbol.sym_func->params->first->next == NULL) strcat(param_id, "1prm");
  else strcat(param_id, "2prm");

  symtable_insert(symtable, param_id, param_elem);

  func_add_param(func, param);
}

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
        func_def_add_param(func, VAR_STRING);
        func_def_add_ret(func, VAR_INT);
        break;
      default:
        break;
    }
  }
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
  // if (!strcmp(func, "SUBSTR"))    return ;
  // if (!strcmp(func, "ORD"))       return ;
  // if (!strcmp(func, "CHR"))       return ;
  // TODO add instructions

  return IC_CALL_FUN;
}

elem_t * try_var_or_func(tToken * token) {
  // control flag: DEF
  /* RETURNS
    sym_func: func_call
    var:      found var
    NULL:     undefined var || other
  */
  if (token->token_type != T_IDENTIFIER) return NULL;
  char * id = to_string(token);
  if (def) {
    symtable_iterator_t it = symtable_find(symtable, id_add_scope(scope_get_head, id));
    if (symtable_iterator_valid(it)) { // variable found
      return symtable_iterator_get_value(it);
    } else return NULL;
  } else {
    eps = true;
    elem_t * var = id_find(scope_get_head(), id);
    if (var != NULL) return var;  // variable found
  }
  // check for function call
  get_next_token(token);
  if (token->token_type == T_L_BRACKET) {
    token->token_type = T_IDENTIFIER;
    token->attr.str_lit.str = id;
    return func_call(id);
  }
  else {
    if (token->token_type != T_DEF_IDENT) {
      // for token_cleanup
      token->token_type = T_IDENTIFIER;
      token->attr.str_lit.str = id;
      error(3, "parser", "match", "Variable \"%s\" undefined", to_string(token));
    }
    token->token_type = T_IDENTIFIER;
    token->attr.str_lit.str = id;
    return NULL;
  }
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
  printf("Added ---MAIN\n");
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

  // each function definition ends here
  // check whether defined return_list was matched by a return_call
  if (last_func->symbol.sym_func->returns != NULL) {
    if (!has_ret)
      error(
        7, "parser", "func_def_type",
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

  sym_var_list_t * rets = last_func->symbol.sym_func->returns;
  if (rets == NULL) return;
  printf("\t\tRETURNS:\n");
  printf("\t\t---");
  for (
    list_item_t * tmp = rets->first;
    tmp != NULL;
    tmp = tmp->next
  ) {
    switch (tmp->item->type) {
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
    case T_IDENTIFIER: {
      elem_t * tmp = try_var_or_func(&next);
      if (tmp == NULL) {
        var_def(); // not defined var nor func call
      }
      else if (tmp->sym_type == SYM_VAR_ITEM)
        var_move(); // found defined var
      else if (tmp->sym_type == SYM_FUNC)
        break;  // func_call handled in try_var_or_func
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
      eps = true;
      break;
  }
}

void var_() {

}

// expects ID in [next] & ':=' to be matched
void var_def() {
  def = true; eps = false;
  match(T_VAR_ID);
  // match(T_DEF_IDENT);  // matched in try_var_or_func
  // TODO all should be handled withing expr_parser
  elem_t * tmp = try_var_or_func(&next);
  if (tmp == NULL || tmp->sym_type == SYM_VAR_ITEM) {
    elem_t * expr = parse_expression();
    // set type
    last_elem->symbol.sym_var_item->type = expr->symbol.sym_var_item->type;
    instr_add_elem1(list->last, expr);
  } else if (tmp->sym_type == SYM_FUNC) {
    func_add_ret(tmp, last_elem->symbol.sym_var_item);
  }
  else error(99, "parser", NULL, "INVALID STATE");
}

void var_move() {
  // create dest sym_var_list
  sym_var_list_t * var_list = sym_var_list_init();
  symbol_t var_list_sym = {.sym_var_list = var_list};
  elem_t * dest = elem_init(SYM_VAR_LIST, var_list_sym);
  symtable_insert(symtable, make_unique(dest), dest);
  last_elem = dest;

  // get dest variables
  def = false; eps = false;
  match(T_VAR_ID);
  next_id();
  if (next.token_type == T_DEF_IDENT)
    error(
      3, "parser", "var_move", "Redefinition of variable \"%s\"",
      last_elem->symbol.sym_var_list->first->item->name
    );
  // '='
  match(T_ASSIGNMENT);

  // get src
  if (next.token_type != T_IDENTIFIER) {
    expr_list();
    return;
  }
  elem_t * tmp = try_var_or_func(&next);
  if (tmp == NULL) {
    // if (last_elem->sym_type == SYM_VAR_ITEM)
    error(3, "parser", "match", "Variable \"%s\" undefined", last_elem->symbol.sym_var_item->name);
    // expr_list();
  } else if (tmp->sym_type == SYM_VAR_ITEM) {
    expr_list();
  } else if (tmp->sym_type == SYM_FUNC) {
    tmp->symbol.sym_func->returns = var_list;
  }
  else error(99, "parser", NULL, "INVALID STATE");



  // last_elem = expr_list_elem;  // (src) now in last_elem
  // expr_list();

  /*
  // create instruction
  instr_t * new_def = instr_create();

  // create dest sym_var_list
  sym_var_list_t * var_list = sym_var_list_init();
  symbol_t var_list_sym = {.sym_var_list = var_list};
  elem_t * var_list_elem = elem_init(SYM_VAR_LIST, var_list_sym);
  // append dest list
  instr_add_dest(new_def, var_list_elem);

  // create src sym_var_list
  sym_var_list_t * exprs_list = sym_var_list_init();
  symbol_t expr_list_sym = {.sym_var_list = exprs_list};
  elem_t * expr_list_elem = elem_init(SYM_VAR_LIST, expr_list_sym);
  // append src list
  instr_add_elem1(new_def, expr_list_elem);

  //======================
  list_add(list, new_def);
  last_elem = var_list_elem;  // (dest) now in last_elem

  // get variables (dest)
  def = false; eps = false;
  match(T_VAR_ID);
  next_id();
  if (next.token_type == T_DEF_IDENT)
    error(
      3, "parser", "var_move", "Redefinition of variable \"%s\"",
      last_elem->symbol.sym_var_item->name
    );
  // '='
  match(T_ASSIGNMENT);
  // get expressions (src)
  last_elem = expr_list_elem;  // (src) now in last_elem
  expr_list();

  // types checked in other places
  if (new_def->type == IC_DEF_VAR)
    check_var_def_types(instr_get_dest(new_def), instr_get_elem1(new_def));
  */
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

  next_id();
}

void if_() {
  // if def
  match(T_IF);
  instr_add_if_def();
  // condition
  parse_expression(); // condition handling
  if (!(instr_get_type(list->last) >= IC_LT_VAR && instr_get_type(list->last) <= IC_NOT_VAR))
    error(2, "parser", NULL, "Invalid condition");
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
  if (!(instr_get_type(list->last) >= IC_LT_VAR && instr_get_type(list->last) <= IC_NOT_VAR))
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
  elem_t * tmp = try_var_or_func(&next);
  if (tmp != NULL) error(2, "parser", NULL, "Invalid definition");
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

  instr_add_ret();

  return_list();

  check_ret(instr_get_dest(list->last));
}

void return_list() {
  if (next.token_type == T_EOL) return;

  elem_t * expr = parse_expression();
  func_add_ret(last_elem, expr->symbol.sym_var_item);

  next_ret();
}

void next_ret() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }

  elem_t * expr = parse_expression();
  func_add_ret(last_elem, expr->symbol.sym_var_item);

  next_ret();
}

elem_t * func_call(char * name) {
  next.token_type = T_IDENTIFIER;
  next.attr.str_lit.str = name;
  def = false; eps = false;
  match(T_FUNC_ID);
  elem_t * func = last_elem;

  // '(' already checked for in try_var_or_func
  func_args();
  match(T_R_BRACKET);

  // sym_var_list_t * ret_list = sym_var_list_init();
  // func->symbol.sym_func->returns = ret_list;
  // symbol_t ret_list_sym = {.sym_var_list = ret_list};
  // elem_t * rets = elem_init(SYM_VAR_LIST, ret_list_sym);
  // symtable_insert(symtable, make_unique(rets), rets);

  return func;

  // // func_call instructions' element assigning
  // switch(get_func_instr_type(last_elem->symbol.sym_func->name)) {
  //   case IC_CALL_FUN: {
  //     break; }
  //   case IC_WRITE_VAR: {
  //     symbol_t dest_list_sym = {.sym_var_list = last_elem->symbol.sym_func->params};
  //     elem_t * dest = elem_init(SYM_VAR_LIST, dest_list_sym);
  //     instr_add_dest(list->last, dest);
  //     break; }
  //   case IC_INT2FLOAT_VAR: {
  //     symbol_t src_list_sym = {.sym_var_list = last_elem->symbol.sym_func->params};
  //     elem_t * src = elem_init(SYM_VAR_LIST, src_list_sym);
  //     instr_add_elem1(list->last, src);
  //     break; }
  //   case FLOAT2INT:
  //     break;
  //   default:
  //     break;
  // }
}

void func_args() {
  if (next.token_type == T_R_BRACKET) return;

  elem_t * arg = parse_expression();
  func_add_param(last_elem, arg->symbol.sym_var_item);

  next_arg();
}

void next_arg() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  elem_t * arg = parse_expression();
  func_add_param(last_elem, arg->symbol.sym_var_item);

  next_arg();
}

void expr_list() {
  // create instruction
  instr_t * new_def = instr_create();
  instr_set_type(new_def, IC_DEF_VAR);
  instr_add_dest(new_def, last_elem);
  list_add(list, new_def);
  // create src sym_var_list
  sym_var_list_t * var_list = sym_var_list_init();
  symbol_t var_list_sym = {.sym_var_list = var_list};
  elem_t * src = elem_init(SYM_VAR_LIST, var_list_sym);
  symtable_insert(symtable, make_unique(src), src);
  instr_add_elem1(new_def, src);
  last_elem = src;

  add_next_expr();
  next_expr();


  check_var_def_types(instr_get_dest(new_def), instr_get_elem1(new_def));

/*
  if (next.token_type == T_IDENTIFIER) { // check if func call
    eps = true;
    elem_t * var = id_find(scope_get_head(), to_string(&next));
    if (eps || var->sym_type == SYM_FUNC) {
      eps = false;

      instr_t * def_instr = list->last;

      func_call();

      instr_t * func_instr = def_instr->next;

      // TODO FUNCEXP: func as expr -- check && handle
      // NOTE move the whole func checking to expr_parser ??
      if (next.token_type != T_EOL) {
        error(
          2, "parser", "FUNCEXP: NOT IMPLEMENTED YET",
          "Expected: '%s' -- Got: '%s'", T_EOL, next.token_type
        );
      }
      // =============================================

      // handling assignment of functions returns

      elem_t * func = last_elem;
      sym_var_list_t * dest = instr_get_dest(def_instr)->symbol.sym_var_list;

        // type
      instr_set_type(def_instr, instr_get_type(func_instr));
      // elem1 -- func
      free(instr_get_elem1(def_instr)->symbol.sym_var_list);
      free(instr_get_elem1(def_instr));
      instr_add_elem1(def_instr, func);
      // linking
      def_instr->next = func_instr->next;
      if (list->last == func_instr) list->last = def_instr;
      instr_add_dest(func_instr, NULL);
      instr_add_elem1(func_instr, NULL);
      instr_delete(func_instr);

      list_item_t * ret = dest->first;
      if (func == NULL) return;
      while (ret != NULL) {
        func_add_ret(func, ret->item);
        ret = ret->next;
      }
      return;
    }
  } // handling expressions

  add_next_expr();

  next_expr();
  */
}

void next_expr() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  add_next_expr();
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

  if (last_elem->sym_type == SYM_VAR_ITEM) {  // assign type to function parameter
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
    list_item_t * list_item = list_item_init(new);
    sym_var_list_add(*rets, list_item);
  }

  last_elem = last_func;
  get_next_token(&next);
}
