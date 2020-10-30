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

#ifndef HTAB_H
#define HTAB_H

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// Table:
struct htab {
	// content is private
	size_t arr_size;
	size_t size;
	struct htab_item *item[];
};
typedef struct htab htab_t;

// Types:
typedef const char *htab_key_t;        // key
typedef int htab_value_t;              // value

// Iterator to table:
struct htab_item {
	// content is private
	htab_key_t key;
	htab_value_t data;
	struct htab_item *next;
};

// Iterator:
typedef struct htab_iterator {
	struct htab_item *ptr;      // item pointer
	const htab_t *t;            // in which table
	size_t idx;                 // in which bucket
} htab_iterator_t;

//hash function
size_t htab_hash_fun(htab_key_t str);

// basic functions to work with table
htab_t *htab_init(size_t n);                   // constructor
size_t htab_size(const htab_t *t);             // number of items in table
size_t htab_bucket_count(const htab_t *t);     // number of items in bucket

htab_iterator_t htab_find(htab_t *t, htab_key_t key);           // search
htab_iterator_t htab_lookup_add(htab_t *t, htab_key_t key);     // search and add

void htab_erase(htab_t *t, htab_iterator_t it);  // delete specified entry

htab_iterator_t htab_begin(const htab_t *t);   // iterator TO first entry
htab_iterator_t htab_end(const htab_t *t);     // iterator AFTER last entry

htab_iterator_t htab_iterator_next(htab_iterator_t it); // iterator++

// test: iterator != end()
inline bool htab_iterator_valid(htab_iterator_t it) { return it.ptr != NULL; }

// test: iterator1 == iterator2
inline bool htab_iterator_equal(htab_iterator_t it1, htab_iterator_t it2) {
	return it1.ptr == it2.ptr && it1.t == it2.t;
}

// read and write through iterator
htab_key_t htab_iterator_get_key(htab_iterator_t it);

htab_value_t htab_iterator_get_value(htab_iterator_t it);
htab_value_t htab_iterator_set_value(htab_iterator_t it, htab_value_t val);

void htab_clear(htab_t *t);    // clear all entries
void htab_free(htab_t *t);     // destructor

// Global variables
htab_t *symtable;

#endif // HTAB_H
