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

/*
  Functions for printing attributes of tokens
  TODO: remove for final implementation
*/
int64_t to_int(tToken *t) {
  return t->attr.int_lit;
}

double to_double(tToken *t) {
  return t->attr.dec_lit;
}

char * to_string(tToken *t) {
  return t->attr.str_lit.str;
}
//-------------------------------------------

// TODO remove
// placeholder for expression parser
void expr() {
  while (
    next.token_type != T_EOL &&
    next.token_type != T_LEFT_BRACE &&  // cond in if
    next.token_type != T_SEMICOLON &&   // for cycle
    next.token_type != T_R_BRACKET      // func_call
  ) {

    token_cleanup();

    get_next_token(&next);
  }
  eps = false;
}

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
      token_cleanup();
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

      // check for duplicate
      if (symtable_iterator_valid(symtable_find(symtable, to_string(&next)))) {
        if (def && !eps)
          error(3, "parser", "match", "Redefinition of function \"%s\"", to_string(&next));
        if (!def) {
          // TODO check for type
          printf("func call: %s\n", to_string(&next));
        }
        else return;
      }
      else { // first definition
        if (!def) {
          if (eps) return;
          error(3, "parser", "match", "Function \"%s\" undefined", to_string(&next));
        }

        // create func
        sym_func_t * func_sym = sym_func_init(to_string(&next), NULL, NULL);
        symbol_t func = {.sym_func = func_sym};
        elem_t * func_data = elem_init(SYM_FUNC, func);
        // add to symtable
        symtable_iterator_t tmp =
          symtable_insert(symtable, to_string(&next), func_data);
        last_func = symtable_iterator_get_value(tmp);
        printf("Added ---%s\n", to_string(&next));
        // token_cleanup();
      }
      break;

    case T_VAR_ID:
      if (next.token_type != T_IDENTIFIER) {
        if (eps) {
          def = false;
          return;
        }
        printf("got here\n");
        goto default_err;
      }

      // construct variable's contextual ID
      char * id = malloc(
        sizeof(char) *  // prefix + : + name + \0
        (strlen(*(last_func->key)) + strlen(to_string(&next)) + 2)
      );
      if (id == NULL) error(99, "parser", "", "Memory allocation failed");;
      id[0] = '\0';
      strcat(id, *(last_func->key)); // prefix
      strcat(id, ":");
      strcat(id, to_string(&next));

      // check for duplicate
      if (symtable_iterator_valid(symtable_find(symtable, id))) {
        free(id); // no longer needed
        if (def && !eps)
          error(3, "parser", "match", "Redefinition of variable \"%s\"", to_string(&next));
        if (!def) {
          // TODO
          printf("var call: %s\n", id);
        }
        else return;
      }
      else { // first definition
        if (!def) {
          free(id);
          if (eps) return;
          error(3, "parser", "match", "Variable \"%s\" undefined", to_string(&next));
        }

        // create var
        sym_var_item_t * var_item = sym_var_item_init(to_string(&next));
        symbol_t var_sym = {.sym_var_item = var_item};
        elem_t * var = elem_init(SYM_VAR_ITEM, var_sym);
        // add to symtable
        symtable_insert(symtable, id, var);
        // free(id);
        last_elem = var;

        printf("\t---%s\n", id);
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

void skip_empty() {
  eps = true;
  match(T_EOL);
  eps = false;
}

void parse() {
  // initialization
  source_file_setup(stdin);
  get_next_token(&next);
  list = list_create();
  symtable = symtable_init(100);

  // parsing
  program();


  // TODO delete
  // debugging instructions
  printf("\n\n\tFUNCTIONS:\n");
  for (
    instr_t * tmp = list_get_active(list);
    tmp != NULL;
    tmp = list_get_next(list)
  ) {
    switch (instr_get_type(tmp)) {
      case IC_DEF_FUN:
        printf("IC_DEF_FUN");
      break;

      default:
        printf("type not implemented");
      break;
    }
    printf("\n");
  }

  // termination
  list_destroy(&list);
  symtable_free(symtable);
}

// functions representing LL grammar nonterminals

void program() {
  prolog();
  // printf("Prolog matched\n");

  if (next.token_type == T_EOF)
    error(3, "parser", "program", "Missing function main");

  match(T_FUNC);
  func_list_pre();

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

  match(T_LEFT_BRACE);
  match(T_EOL);
  body();
  match(T_RIGHT_BRACE);
  match(T_EOL);

  // for debugging purposes TODO delete
  // printf("Main matched\n");

  func_list();
  skip_empty();
  match(T_EOF);
  // printf("Program matched\n");
}

void prolog() {
  // skip all empty space to T_PACKAGE
  skip_empty();

  match(T_PACKAGE);
  skip_empty();
  if (next.token_type != T_IDENTIFIER || strcmp(to_string(&next), "main"))
    error(
      2, "parser.c", "match",
      "Expected: '%s' -- Got: '%s'", T_MAIN, next.token_type
    );
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
  // for debugging purposes TODO delete
  // char s[50] = "";
  // strcat(s, to_string(&next));
  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);
  func_def_type();
  match(T_EOL);
  match(T_FUNC);
  // for debugging purposes TODO delete
  // printf("---Func %s matched\n", s);
  func_list_pre();
}

void func_list() {
  eps = true;
  func_def();
  if (eps) { // test of func_def
    eps = false;
    return;
  }
  func_list();
}

void func_def() {
  match(T_FUNC);
  if (eps) return; // from func_list
  def = true; eps = false;
  match(T_FUNC_ID);

  match(T_L_BRACKET);
  param_list();
  match(T_R_BRACKET);
  func_def_type();
  match(T_EOL);
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
  eps = true;
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

    default:
      match(T_LEFT_BRACE);
      break;
  }
  // each function definition ends here

  // add func def instr
  instr_t * new_func = instr_create();
  instr_set_type(new_func, IC_DEF_FUN);
  instr_add_dest(new_func, last_func);
  list_add(list, new_func);
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
  // adding parameters of func
  last_elem = last_func;
  type();
  next_ret_def();

  // TODO delete
  sym_var_list_t * rets = last_elem->symbol.sym_func->returns;
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
    case T_IDENTIFIER:  // var_ OR func_call
      // def = false; eps = true; // TODO
      match(T_FUNC_ID);
      if (!def) func_call();
      else {
        eps = false;
        var_();
      }
      break;

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
  printf("var_ called\n");
  eps = true;
  match(T_VAR_ID);
  if (next.token_type == T_DEF_IDENT) var_def();
  else var_move();
}

void var_def() {
  match(T_DEF_IDENT);
  // printf("matched := \n");
  parse_expression();
}

void var_move() {
  next_id();
  match(T_ASSIGNMENT);
  // printf("matched = \n");
  expr_list();
}

void next_id() {
  eps = true;
  match(T_COMMA);
  if (eps) {
    eps = false;
    return;
  }
  match(T_VAR_ID);
  next_id();
}

void if_() {
  match(T_IF);
  // printf("matched IF\t%d\n", get_err_line()+1);
  parse_expression(); // condition handling
  match(T_LEFT_BRACE);
  match(T_EOL);
  body();
  match(T_RIGHT_BRACE);
  // printf("matched ENDIF\t%d\t%d\n", get_err_line()+1, next.token_type);
  if_cont();
  // printf("if matched\n");
}

void if_cont() {
  if (next.token_type != T_ELSE) return;
  match(T_ELSE);
  // printf("matched else\n");
  else_();
}

void else_() {
  if (next.token_type != T_LEFT_BRACE) {
    if_();
    return;
  }
  match(T_LEFT_BRACE);
  match(T_EOL);
  body();
  match(T_RIGHT_BRACE);
}

void cycle() {
  match(T_FOR);
  for_def();
  match(T_SEMICOLON);
  parse_expression(); // expression parser call
  match(T_SEMICOLON);
  for_move();
  match(T_LEFT_BRACE);
  match(T_EOL);
  body();
  match(T_RIGHT_BRACE);
}

void for_def() {
  eps = true;
  match(T_VAR_ID);
  if (eps) {
    eps = false;
    return;
  }
  var_def();
}

void for_move() {
  eps = true;
  match(T_VAR_ID);
  if (eps) {
    eps = false;
    return;
  }
  var_move();
}

void return_() {
  match(T_RETURN);
  return_list();
}

void return_list() {
  if (next.token_type == T_EOL) return;

  // TODO add type checking
  expr_parser();
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
  expr_parser();
  next_ret();
}


void func_call() {
  match(T_FUNC_ID);
  match(T_L_BRACKET);
  func_args();
  match(T_R_BRACKET);
}

void func_args() {
  if (
    next.token_type == T_L_BRACKET ||
    next.token_type == T_IDENTIFIER ||
    next.token_type == T_INT_VALUE ||
    next.token_type == T_DEC_VALUE ||
    next.token_type == T_STRING_VALUE
  ) {
    parse_expression();
    next_expr();
  }
}

void expr_list() {
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
    error(99, "parser", "type", "Missing variable to assign type to");

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
    // TODO add after implementation in scanner
    // case T_BOOL:
    //   type = VAR_BOOL;
    //   break;
    default:
      error(1, "parser", "type", "Invalid variable type");
      break;
  }

  if (last_elem->sym_type == SYM_VAR_ITEM) {
    sym_var_item_set_type(last_elem->symbol.sym_var_item, type);
    if (type == VAR_STRING)
      last_elem->symbol.sym_var_item->data.string_t = NULL;
  }
  else if (last_elem->sym_type == SYM_FUNC) {
    // get returns
    sym_var_list_t ** rets = &(last_elem->symbol.sym_func->returns);
    if (rets == NULL)
      error(99, "parser", "type", "Failed to access function's returns");
    if (*rets == NULL)  // param list empty
      *rets = sym_var_list_init();

    //create item
    sym_var_item_t * new = sym_var_item_init(NULL);
    sym_var_item_set_type(new, type);
    if (type == VAR_STRING)
      new->data.string_t = NULL;
    // add to return list
    sym_var_list_add(*rets, new);
  }
  else error(99, "parser", "type", "Unknown element type");

  get_next_token(&next);
}
