/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: test_symtable.c
 * Brief: Symtable tests
 *
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../symtable.h"

#define ARR_SIZE 1061
#define KEY_LIMIT 127

int main() {
	//initialization
	symtable = symtable_init(ARR_SIZE);
	symtable_iterator_t iterator;

	if (symtable == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	assert(KEY_LIMIT > 1);

	//Test1
	printf("[TEST01]\n");
	printf("Add 1st element to table\n");
	printf("~~~~~~~~~~~~~~~~~~~~\n");

	//def
	char *t1key = "main";
	char *t1fun_name = "main:return";

	char *symtable_key1_const = "key1";
	char *symtable_key2_const = "key2";
	char *symtable_key3_const = "key3";
	char *sym_var_key0_const = "item0";
	char *sym_var_key1_const = "item1";

	char *symtable_key1 = malloc(strlen(symtable_key1_const) + 1);
	char *symtable_key2 = malloc(strlen(symtable_key2_const) + 1);
	char *symtable_key3 = malloc(strlen(symtable_key3_const) + 1);
	char *sym_var_key0 = malloc(strlen(sym_var_key0_const) + 1);
	char *sym_var_key1 = malloc(strlen(sym_var_key1_const) + 1);

	if (symtable_key1 == NULL || symtable_key2 == NULL || symtable_key3 == NULL ||
	sym_var_key0 == NULL || sym_var_key1 == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	strcpy(symtable_key1, symtable_key1_const);
	strcpy(symtable_key2, symtable_key2_const);
	strcpy(symtable_key3, symtable_key3_const);
	strcpy(sym_var_key0, sym_var_key0_const);
	strcpy(sym_var_key1, sym_var_key1_const);

	variable_t t1var_param = {.int_t = 42};
	variable_t t1var_ret = {.int_t = 1};

	sym_type_t t1symt = SYM_FUNC;
	sym_var_item_t *t1symbol_item0 = sym_var_item_init(sym_var_key0);
	sym_var_item_t *t1symbol_item1 = sym_var_item_init(sym_var_key1);

	sym_var_item_set_type(t1symbol_item0, VAR_INT);
	sym_var_item_set_type(t1symbol_item1, VAR_INT);

	sym_var_item_set_data(t1symbol_item0, t1var_param);
	sym_var_item_set_data(t1symbol_item1, t1var_ret);

	sym_var_item_set_const(t1symbol_item0, true);
	sym_var_item_set_const(t1symbol_item1, true);

	sym_var_item_set_global(t1symbol_item0, true);
	sym_var_item_set_global(t1symbol_item1, true);

	sym_var_list_t *t1symbol_arr_param = sym_var_list_init();
	list_item_t *l_t1symbol_arr_param = list_item_init(t1symbol_item0);
	sym_var_list_add(t1symbol_arr_param, l_t1symbol_arr_param);

	sym_var_list_t *t1symbol_arr_ret = sym_var_list_init();
	list_item_t *l_t1symbol_item1 = list_item_init(t1symbol_item1);
	sym_var_list_add(t1symbol_arr_ret, l_t1symbol_item1);

	symbol_t t1symbol_func;
	t1symbol_func.sym_func = sym_func_init(t1fun_name, t1symbol_arr_param, t1symbol_arr_ret);

	symbol_t t1sym_item0;
	t1sym_item0.sym_var_item = t1symbol_item0;

	symbol_t t1sym_item1;
	t1sym_item1.sym_var_item = t1symbol_item1;

	elem_t *t1e = elem_init(t1symt, t1symbol_func);
	elem_t *t1e_s0 = elem_init(SYM_VAR_ITEM, t1sym_item0);
	elem_t *t1e_s1 = elem_init(SYM_VAR_ITEM, t1sym_item1);

	symtable_insert(symtable, symtable_key1, t1e_s0);
	symtable_insert(symtable, symtable_key2, t1e_s1);
	symtable_insert(symtable, symtable_key3, t1e);
	iterator = symtable_find(symtable, symtable_key3);

	printf("Size of symbtable: %zu\n", symtable->arr_size);
	printf("Find inserted element by %s: %zu (index)\n", t1key, iterator.idx);
	printf("Inserted element key: %s\n", elem_key(iterator.ptr->data));
	printf("Inserted element symbol, function name: %s\n", iterator.ptr->data->symbol.sym_func->name);
	printf("Inserted element symbol, 1st param name: %s\n", iterator.ptr->data->symbol.sym_func->params->first->item->name);
	printf("Inserted element symbol, 1st param value : %d\n", iterator.ptr->data->symbol.sym_func->params->first->item->data.int_t);
	printf("Inserted element symbol, function return value : %d\n", iterator.ptr->data->symbol.sym_func->returns->first->item->data.int_t);

	// Clean up test1
	list_item_free(l_t1symbol_arr_param);
	symtable_free(symtable);

	return EXIT_SUCCESS;
}
