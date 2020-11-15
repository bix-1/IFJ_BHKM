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
	char *t1param_name = "myparam1";

	variable_t t1var_param = {.int_t = 42};
	variable_t t1var_ret = {.int_t = 1};

	sym_type_t t1symt = SYM_FUNC;
	sym_var_item_t *t1symbol_item0 = sym_var_item_init(t1param_name, VAR_INT, t1var_param, true, true);
	sym_var_item_t *t1symbol_item1 = sym_var_item_init(t1param_name, VAR_INT, t1var_ret, true, true);

	sym_var_list_t *t1symbol_arr_param = sym_var_list_init();
	sym_var_list_add(t1symbol_arr_param, t1symbol_item0);

	sym_var_list_t *t1symbol_arr_ret = sym_var_list_init();
	sym_var_list_add(t1symbol_arr_ret, t1symbol_item1);

	symbol_t t1symbol_func;
	t1symbol_func.sym_func = sym_func_init(t1fun_name, t1symbol_arr_param, t1symbol_arr_ret);

	elem_t *t1e = elem_init(t1symt, t1symbol_func);

	symtable_insert(symtable, t1key, t1e);
	iterator = symtable_find(symtable, t1key);

	printf("Size of symbtable: %zu\n", symtable->arr_size);
	printf("Find inserted element by %s: %zu (index)\n", t1key, iterator.idx);
	printf("Inserted element key: %s\n", elem_key(iterator.ptr->data));
	printf("Inserted element symbol, function name: %s\n", iterator.ptr->data->symbol.sym_func->name);
	printf("Inserted element symbol, 1st param name: %s\n", iterator.ptr->data->symbol.sym_func->params->first->name);
	printf("Inserted element symbol, 1st param value : %d\n", iterator.ptr->data->symbol.sym_func->params->first->data.int_t);
	printf("Inserted element symbol, function return value : %d\n", iterator.ptr->data->symbol.sym_func->returns->first->data.int_t);

	// Clean up test1
	symtable_free(symtable);

	return EXIT_SUCCESS;
}
