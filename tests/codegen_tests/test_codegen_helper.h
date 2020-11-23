/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: test_codegen_helper.h
 * Brief: Header file for file test_codegen_helper.c
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */
#ifndef TEST_CODEGEN_HELPER_H
#define TEST_CODEGEN_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../../codegen.h"
#include "../../ll.h"
#include "../../error.h"

void test_init();
void test_free_all();
void append_to_free(char *key);

elem_t *create_function(char *name, sym_var_list_t *params, sym_var_list_t *returns);
elem_t *create_var_item(char *name, var_type_t var_type, variable_t data, bool is_const, bool is_global);
elem_t *create_var_list();

void var_list_add(sym_var_list_t *sym_var_list, sym_var_item_t *sym_var_item);
void add_instruction(instr_type_t type, elem_t *elem_dest, elem_t *elem1, elem_t *elem2);

#endif //TEST_CODEGEN_HELPER_H
