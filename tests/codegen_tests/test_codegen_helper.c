/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: test_codegen_helper.c
 * Brief: Helper module for codegen testing
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "test_codegen_helper.h"

#define ARR_SIZE 1061

int symtable_key = 0;

// Linked list struct to easily free strings
typedef struct keys_to_free keys_to_free_t;
struct keys_to_free  {
	char *ptr;
	keys_to_free_t *next;
};

keys_to_free_t *free_list;

void test_init() {
	// Init symtable
	symtable = symtable_init(ARR_SIZE);
	if (symtable == NULL) {
		error(99, "test_codegen_helper.c", "init", "NULL element");
	}

	// Init list
	list = list_create();
	if (list == NULL) {
		error(99, "test_codegen_helper.c", "init", "NULL element");
	}

	free_list = malloc(sizeof(keys_to_free_t));
	if (free_list == NULL) {
		error(99, "test_codegen_helper.c", "init", "NULL element");
	}
	free_list->ptr = NULL;
	free_list->next = NULL;
}

// Destructor
void test_free_all() {
	/*keys_to_free_t *current = free_list;

	while (current != NULL) {
		keys_to_free_t *temp = current->next;
		free(current->ptr);
		free(current);
		current = temp->next;
	}*/
	free(free_list);

	symtable_free(symtable);
	list_destroy(&list);
}

void append_to_free(char *key) {
	keys_to_free_t *current = free_list;
	keys_to_free_t *prev = NULL;

	while (current != NULL) {
		prev = current;
		current = current->next;
	}

	if (prev == NULL) {
		current->ptr = key;
		current->next = NULL;
	}
	else {
		keys_to_free_t *new = malloc(sizeof(keys_to_free_t));
		if (new == NULL) {
			error(99, "test_codegen_helper.c", "append_to_free", "NULL element");
		}
		new->ptr = key;
		new->next = NULL;
		prev->next = new;
	}
}

elem_t *create_function(char *name, sym_var_list_t *params, sym_var_list_t *returns) {
	// key
	// 6 chars + 1 \0
	char *sym_key = malloc(sizeof(char) * 7);
	if (sym_key == NULL) {
		error(99, "test_codegen_helper.c", "create_var_item", "NULL element");
	}
	sprintf(sym_key, "%06d", symtable_key);
	//append_to_free(sym_key);

	// name
	char *name_str = malloc(strlen(name) + 1);
	if (name_str == NULL) {
		error(99, "test_codegen_helper.c", "create_var_item", "NULL element");
	}
	strcpy(name_str, name);
	//append_to_free(name_str);

	sym_func_t *sym_func = sym_func_init(name_str, params, returns);

	symbol_t symbol;
	symbol.sym_func = sym_func;

	elem_t *elem = elem_init(SYM_FUNC, symbol);

	symtable_insert(symtable, sym_key, elem);
	symtable_key++;

	return elem;
}

elem_t *create_var_item(char *name, var_type_t var_type, variable_t data, bool is_const, bool is_global) {
	// key
	// 6 chars + 1 \0
	char *sym_key = malloc(sizeof(char) * 7);
	if (sym_key == NULL) {
		error(99, "test_codegen_helper.c", "create_var_item", "NULL element");
	}
	sprintf(sym_key, "%06d", symtable_key);
	//append_to_free(sym_key);

	// name
	char *name_str = malloc(strlen(name) + 1);
	if (name_str == NULL) {
		error(99, "test_codegen_helper.c", "create_var_item", "NULL element");
	}
	strcpy(name_str, name);
	//append_to_free(name_str);

	sym_var_item_t *sym_var_item = sym_var_item_init(name_str);
	sym_var_item_set_type(sym_var_item, var_type);

	if (var_type == VAR_STRING) {
		char *data_str = malloc(strlen(data.string_t) + 1);
		if (data_str == NULL) {
			error(99, "test_codegen_helper.c", "create_var_item", "NULL element");
		}
		strcpy(data_str, data.string_t);
		append_to_free(data_str);
		variable_t var = {.string_t = data_str};
		sym_var_item_set_data(sym_var_item, var);
	}
	else {
		sym_var_item_set_data(sym_var_item, data);
	}

	sym_var_item_set_const(sym_var_item, is_const);
	sym_var_item_set_global(sym_var_item, is_global);
	sym_var_item_set_formatted(sym_var_item, false);

	symbol_t symbol;
	symbol.sym_var_item = sym_var_item;

	elem_t *elem = elem_init(SYM_VAR_ITEM, symbol);

	symtable_insert(symtable, sym_key, elem);
	symtable_key++;

	return elem;
}

elem_t *create_var_list() {
	// key
	// 6 chars + 1 \0
	char *sym_key = malloc(sizeof(char) * 7);
	if (sym_key == NULL) {
		error(99, "test_codegen_helper.c", "create_var_item", "NULL element");
	}
	sprintf(sym_key, "%06d", symtable_key);
	//append_to_free(sym_key);

	sym_var_list_t *sym_var_list = sym_var_list_init();

	symbol_t symbol;
	symbol.sym_var_list = sym_var_list;

	elem_t *elem = elem_init(SYM_VAR_LIST, symbol);

	symtable_insert(symtable, sym_key, elem);
	symtable_key++;

	return elem;
}

void var_list_add(sym_var_list_t *sym_var_list, sym_var_item_t *sym_var_item) {
	list_item_t *list_item = list_item_init();

	list_item->item = sym_var_item;
	sym_var_list_add(sym_var_list, list_item);
}

void add_instruction(instr_type_t type, elem_t *elem_dest, elem_t *elem1, elem_t *elem2) {
	instr_t *instr = malloc(sizeof(instr_t));

	instr->type = type;
	instr->elem_dest_ptr = elem_dest;
	instr->elem1_ptr = elem1;
	instr->elem2_ptr = elem2;

	list_add(list, instr);
}
