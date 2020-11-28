/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: symtable.h
 * Brief: Header file for symtable.c
 * Contribution: Used from IJC 2nd project, implemented by xhladk15
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "codegen.h"    // instr_type

// Table:
struct symtable {
	// content is private
	size_t arr_size;
	size_t size;
	struct symtable_item *item[];
};
typedef struct symtable symtable_t;

// Types:
typedef enum sym_type {
	SYM_FUNC,
	SYM_VAR_ITEM,
	SYM_VAR_LIST
} sym_type_t;

typedef enum var_type {
	VAR_INT,
	VAR_FLOAT64,
	VAR_STRING,
	VAR_BOOL,
	VAR_NIL,
	VAR_UNDEFINED
} var_type_t;

typedef union variable {
	int int_t;
	double float64_t;
	char *string_t;
	bool bool_t;
	void *nil;
} variable_t;

typedef struct sym_var_item sym_var_item_t;
struct sym_var_item {
	char *name;
	bool is_const;
	bool is_global;
	bool is_defined;
	bool is_formatted;
	var_type_t type;
	variable_t data;
	variable_t default_data;
};

typedef struct list_item list_item_t;
struct list_item {
	sym_var_item_t *item;
	list_item_t *next;
	list_item_t *prev;
};

typedef struct sym_var_list {
	list_item_t *first;
	list_item_t *active;
} sym_var_list_t;

typedef struct sym_func {
	char *name;
	sym_var_list_t *params;
	sym_var_list_t *returns;
} sym_func_t;

typedef union symbol {
	sym_func_t *sym_func;
	sym_var_item_t *sym_var_item;
	sym_var_list_t *sym_var_list;
} symbol_t;

typedef struct elem {
	// Key is auto assigned on creating table entry
	char **key;
	sym_type_t sym_type;
	symbol_t symbol;
} elem_t;

// key
typedef const char *symtable_key_t;

// value
typedef elem_t *symtable_value_t;

// Iterator to table:
struct symtable_item {
	// content is private
	symtable_key_t key;
	symtable_value_t data;
	struct symtable_item *next;
};

// Iterator:
typedef struct symtable_iterator {
	struct symtable_item *ptr;      // item pointer
	const symtable_t *t;            // in which table
	size_t idx;                 // in which bucket
} symtable_iterator_t;

// hash function
size_t symtable_hash_fun(symtable_key_t str);

// constructor
symtable_t *symtable_init(size_t n);

// number of items in table
size_t symtable_size(const symtable_t *t);

// number of items in bucket
size_t symtable_bucket_count(const symtable_t *t);

// search
symtable_iterator_t symtable_find(symtable_t *t, symtable_key_t key);

// search and add
symtable_iterator_t symtable_insert(symtable_t *t, symtable_key_t key, symtable_value_t data);

// delete specified entry
void symtable_erase(symtable_t *t, symtable_iterator_t it);

// iterator TO first entry
symtable_iterator_t symtable_begin(const symtable_t *t);

// iterator AFTER last entry
symtable_iterator_t symtable_end(const symtable_t *t);

// iterator++
symtable_iterator_t symtable_iterator_next(symtable_iterator_t it);

// test: iterator != end()
inline bool symtable_iterator_valid(symtable_iterator_t it) { return it.ptr != NULL; }

// test: iterator1 == iterator2
inline bool symtable_iterator_equal(symtable_iterator_t it1, symtable_iterator_t it2) {
	return it1.ptr == it2.ptr && it1.t == it2.t;
}

// read and write through iterator
symtable_key_t symtable_iterator_get_key(symtable_iterator_t it);

symtable_value_t symtable_iterator_get_value(symtable_iterator_t it);

symtable_value_t symtable_iterator_set_value(symtable_iterator_t it, symtable_value_t val);

// clear all entries
void symtable_clear(symtable_t *t);

// destructor
void symtable_free(symtable_t *t);

// Symbols functions:

// constructor of whole element
elem_t *elem_init(sym_type_t sym_type, symbol_t symbol);

// constructor of symbol func
sym_func_t *sym_func_init(char *name, sym_var_list_t *params, sym_var_list_t *returns);

// constructor of symbol var item
sym_var_item_t *sym_var_item_init(char *name);

// constructor of symbol var list
sym_var_list_t *sym_var_list_init();

// constructor of list item
list_item_t *list_item_init(sym_var_item_t *sym_var_item);

// destructor of symbol func
void sym_func_free(sym_func_t *sym_func);

// destructor of symbol var item
void sym_var_item_free(sym_var_item_t *sym_var_item);

// destructor of symbol var list
void sym_var_list_free(sym_var_list_t *sym_var_list);

// destructor of list item
void list_item_free(list_item_t *list_item);

// setter for symbol var variable type
void sym_var_item_set_type(sym_var_item_t *sym_var_item, var_type_t type);

// setter for symbol var variable data
void sym_var_item_set_data(sym_var_item_t *sym_var_item, variable_t data);

// setter for symbol var is const
void sym_var_item_set_const(sym_var_item_t *sym_var_item, bool is_const);

// setter for symbol var is global
void sym_var_item_set_global(sym_var_item_t *sym_var_item, bool is_global);

// setter for symbol var has been formatted
void sym_var_item_set_formatted(sym_var_item_t *sym_var_item, bool is_formatted);

// setter for next item in list
void list_item_set_next(list_item_t *list_item, list_item_t *next);

// setter for prev item in list
void list_item_set_prev(list_item_t *list_item, list_item_t *next);

// get element key
symtable_key_t elem_key(elem_t *elem);

// append symbol var item to list
void sym_var_list_add(sym_var_list_t *sym_var_list, list_item_t *list_item);

// size of symbol var list
size_t sym_var_list_size(sym_var_list_t *sym_var_list);

// remove symbol var item from end of the list
void sym_var_list_pop(sym_var_list_t *sym_var_list);

// get item after active and change it to new active
sym_var_item_t *sym_var_list_next(sym_var_list_t *sym_var_list);

// get item before active and change it to new active, goes from end. Usage in stack
sym_var_item_t *sym_var_list_prev(sym_var_list_t *sym_var_list);

// get active item
sym_var_item_t *sym_var_list_get_active(sym_var_list_t *sym_var_list);

// clear all items in list
void sym_var_list_clear(sym_var_list_t *sym_var_list);

// set item variable value, cant change type
void sym_var_item_change_data(sym_var_item_t *sym_var_item, variable_t data);

// reset data value to default data
void sym_var_item_reset(sym_var_item_t *sym_var_item);

// Global variables
symtable_t *symtable;

#endif // SYMTABLE_H
