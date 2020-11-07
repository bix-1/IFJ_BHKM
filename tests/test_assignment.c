/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: test_assignment.c
 * Brief: Complex tests according to given project assignment
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */


#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "../scanner.h"

int64_t to_int(tToken *t) {
  return t->attr.int_lit;
}

double to_double(tToken *t) {
  return t->attr.dec_lit;
}

char * to_string(tToken *t) {
  return t->attr.str_lit.str;
}

int main()
{
  printf("#__________Assignment_tests__________#\n\n");

  // initialization
  FILE *input = fopen("tests/assignment_tests/test_input", "r");
  source_file_setup(input);
  tToken t;

  // get_next_token(&t);
  // printf("%d: %s\n", t.token_type, t.attr);

  printf("[TEST01] Checking validity of identifiers\n");
  printf("--- Last 4 shoud be invalid\n");
  bool stop = false;
  while (!stop) {
    get_next_token(&t);
    if (t.token_type == T_IDENTIFIER) {
      if (!strcmp(to_string(&t), "_end")) {
        stop = true;
        break;
      }
      printf("ID: ");
      printf("%s\n", to_string(&t));
    }
    else {
      printf("ERROR: Invalid identifier\n");
      // invalid ids in testing input are separated by white space
      for (int c; !isspace(c = fgetc(source)); ){}
    }
  }
  printf("\n");


  printf("[TEST02] Checking keywords\n");
  printf("--- no Warning given\n");
  // else, float64, for, func, if, int, package, return, string
  while (1) {
    get_next_token(&t);
    if (t.token_type == T_IDENTIFIER ) break;
    switch (t.token_type) {
      case T_ELSE:
        printf("ELSE\n");
        break;
      case T_FLOAT64:
        printf("FLOAT64\n");
        break;
      case T_FOR:
        printf("FOR\n");
        break;
      case T_FUNC:
        printf("FUNC\n");
        break;
      case T_IF:
        printf("IF\n");
        break;
      case T_INT:
        printf("INT\n");
        break;
      case T_PACKAGE:
        printf("PACKAGE\n");
        break;
      case T_RETURN:
        printf("RETURN\n");
        break;
      case T_STRING:
        printf("STRING\n");
        break;
      default:
        printf("Something surely went wrong...\n");
        break;
    }
  }
  printf("\n");

  printf("[TEST03] Checking ints\n");
  while (1) {
    get_next_token(&t);
    if (t.token_type != T_INT_VALUE ) break;
    printf("int: %ld\n", to_int(&t));
  }
  printf("\n");

  printf("[TEST04] Checking floats\n");
  printf("--- Inputs: 0.5, 1.5\n");
  while (1) {
    get_next_token(&t);
    if (t.token_type == T_IDENTIFIER ) break;
    if (t.token_type == T_DEC_VALUE)
      printf("float: %f\n", to_double(&t));
    else if (t.token_type == T_INT_VALUE)
      printf("ERROR: read as int -- %d\n", to_int(&t));
  }
  printf("\n");

  printf("#_______END of Assignment_tests______#\n");
  return 0;
}
