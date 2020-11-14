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
	sym_type_t t1symt = SYM_FUNC;
	symbol_t t1symbol_func;
	symbol_t t1symbol_arr_ret;
	symbol_t t1symbol_arr_param;
	symbol_t t1symbol_item0;
	symbol_t t1symbol_item1;


	t1symbol_item0.sym_var_item = malloc(sizeof(sym_var_item_t));
	if (t1symbol_item0.sym_var_item == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	t1symbol_item0.sym_var_item->name = malloc(strlen(t1fun_name) + 1);
	if (t1symbol_item0.sym_var_item->name == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	strcpy(t1symbol_item0.sym_var_item->name, t1fun_name);
	t1symbol_item0.sym_var_item->type = VAR_INT;
	t1symbol_item0.sym_var_item->data.int_t = 0;
	t1symbol_item0.sym_var_item->default_data.int_t = 0;

	t1symbol_item1.sym_var_item = malloc(sizeof(sym_var_item_t));
	if (t1symbol_item1.sym_var_item == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	t1symbol_item1.sym_var_item->name = malloc(strlen(t1param_name) + 1);
	if (t1symbol_item1.sym_var_item->name == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	strcpy(t1symbol_item1.sym_var_item->name, t1param_name);
	t1symbol_item1.sym_var_item->type = VAR_INT;
	t1symbol_item1.sym_var_item->data.int_t = 42;
	t1symbol_item1.sym_var_item->default_data.int_t = 42;

	t1symbol_arr_ret.sym_var_list = malloc(sizeof(sym_var_list_t));
	if(t1symbol_arr_ret.sym_var_list == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	t1symbol_arr_param.sym_var_list = malloc(sizeof(sym_var_list_t));
	if(t1symbol_arr_param.sym_var_list == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	t1symbol_arr_ret.sym_var_list->first = t1symbol_item0.sym_var_item;
	t1symbol_arr_param.sym_var_list->first = t1symbol_item1.sym_var_item;

	t1symbol_func.sym_func = malloc(sizeof(sym_func_t));
	if(t1symbol_func.sym_func == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	t1symbol_func.sym_func->name = malloc(strlen(t1key) + 1);
	if(t1symbol_func.sym_func->name == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	strcpy(t1symbol_func.sym_func->name, t1key);
	t1symbol_func.sym_func->params = t1symbol_arr_param.sym_var_list;
	t1symbol_func.sym_func->returns = t1symbol_arr_ret.sym_var_list;

	elem_t *t1e = malloc(sizeof(elem_t));

	if (t1e == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	t1e->key = malloc(strlen(t1key) + 1);

	if (t1e->key == NULL) {
		printf("Error: Memory allocation\n");
		return EXIT_FAILURE;
	}

	strcpy(t1e->key, t1key);
	t1e->sym_type = t1symt;
	t1e->symbol = t1symbol_func;

	symtable_lookup_add(symtable, t1key, *t1e);
	iterator = symtable_find(symtable, t1key);

	printf("Size of symbtable: %zu\n", symtable->arr_size);
	printf("Find inserted element by %s: %zu (index)\n", t1key, iterator.idx);
	printf("Inserted element key: %s\n", iterator.ptr->data.key);
	printf("Inserted element symbol, function name: %s\n", iterator.ptr->data.symbol.sym_func->name);
	printf("Inserted element symbol, 1st parm name: %s\n", iterator.ptr->data.symbol.sym_func->params->first->name);

	// Clean up test1
	free(t1symbol_func.sym_func->name);
	free(t1symbol_func.sym_func);
	free(t1symbol_item0.sym_var_item->name);
	free(t1symbol_item1.sym_var_item->name);
	free(t1symbol_item0.sym_var_item);
	free(t1symbol_item1.sym_var_item);
	free(t1symbol_arr_ret.sym_var_list);
	free(t1symbol_arr_param.sym_var_list);
	free(t1e->key);
	free(t1e);

	symtable_free(symtable);

	return EXIT_SUCCESS;
}
