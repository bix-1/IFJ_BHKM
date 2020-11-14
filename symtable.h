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

#include "codegen.h"

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
	SYM_CONST,
	SYM_VAR
} sym_type_t;

typedef enum var_type {
	VAR_INT,
	VAR_FLOAT64,
	VAR_STRING,
	VAR_BOOL
} var_type_t;

typedef union variable {
	int int_t;
	double float64_t;
	char *string_t;
} variable_t;

typedef struct sym_var_item {
	char *name;
	var_type_t type;
	variable_t data;
	variable_t default_data;
} sym_var_item_t;

typedef struct sym_var_list {
	sym_var_item_t *first;
	sym_var_item_t *active;
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

typedef struct instruction {
	instr_type_t type;
	char *elem_dest_key;
	char *elem1_key;
	char *elem2_key;
} instruction_t;

typedef struct elem {
	char *key;
	sym_type_t sym_type;
	symbol_t symbol;
} *elem_t;

typedef const char *symtable_key_t;        // key
typedef elem_t symtable_value_t;           // value

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

//hash function
size_t symtable_hash_fun(symtable_key_t str);

// basic functions to work with table
symtable_t *symtable_init(size_t n);                   // constructor
size_t symtable_size(const symtable_t *t);             // number of items in table
size_t symtable_bucket_count(const symtable_t *t);     // number of items in bucket

symtable_iterator_t symtable_find(symtable_t *t, symtable_key_t key);           // search
symtable_iterator_t symtable_lookup_add(symtable_t *t, symtable_key_t key, symtable_value_t data);     // search and add

void symtable_erase(symtable_t *t, symtable_iterator_t it);  // delete specified entry

symtable_iterator_t symtable_begin(const symtable_t *t);   // iterator TO first entry
symtable_iterator_t symtable_end(const symtable_t *t);     // iterator AFTER last entry

symtable_iterator_t symtable_iterator_next(symtable_iterator_t it); // iterator++

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

void symtable_clear(symtable_t *t);    // clear all entries
void symtable_free(symtable_t *t);     // destructor

// Global variables
symtable_t *symtable;

#endif // SYMTABLE_H
