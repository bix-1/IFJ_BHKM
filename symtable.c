/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: htab.c
 * Brief: Implementation of hash table functions
 * Contribution: Used from IJC 2nd project, implemented by xhladk15
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "symtable.h"

extern bool htab_iterator_valid(htab_iterator_t it);
extern bool htab_iterator_equal(htab_iterator_t it1, htab_iterator_t it2);

htab_t *htab_init(size_t n) {
	assert(n > 0);

	htab_t *htab = malloc(sizeof(htab_t) + sizeof(struct htab_item) * n);

	if(htab == NULL)
		return NULL;

	memset(htab, 0, sizeof(htab_t) + sizeof(struct htab_item) * n);

	htab->arr_size = n;
	htab->size = 0;

	return htab;
}

size_t htab_size(const htab_t *t) {
	if(t == NULL)
		return 0;

	return t->size;
}

size_t htab_bucket_count(const htab_t *t) {
	if(t == NULL)
		return 0;

	return t->arr_size;
}

size_t htab_hash_fun(const char *str) {
	uint32_t h = 0;
	const unsigned char *p;
	for (p = (const unsigned char *) str; *p != '\0'; p++)
		h = 65599 * h + *p;
	return h;
}

htab_iterator_t htab_find(htab_t *t, htab_key_t key) {

	if(t == NULL)
		return htab_end(t);

	if(key == NULL)
		return htab_end(t);

	struct htab_iterator iterator;

	// Get index from hash function
	size_t index = (htab_hash_fun(key) % t->arr_size);

	iterator.t = t;
	iterator.idx = index;

	struct htab_item *next = t->item[index];

	// Try to find existing
	if(next != NULL) {
		do {
			if(strcmp(next->key, key) == 0){
				iterator.ptr = next;
				return iterator;
			}
			else {
				next = next->next;
			}
		}while (next != NULL);
	}

	iterator = htab_end(t);
	return iterator;
}

htab_iterator_t htab_lookup_add(htab_t *t, htab_key_t key) {

	if(t == NULL)
		return htab_end(t);

	if(key == NULL)
		return htab_end(t);

	struct htab_iterator iterator;

	// Get index from hash function
	size_t index = (htab_hash_fun(key) % t->arr_size);

	iterator.t = t;
	iterator.idx = index;

	struct htab_item *next = t->item[index];

	// Try to find existing entry
	if(next != NULL) {
		do {
			if(strcmp(next->key, key) == 0){
				iterator.ptr = next;
				return iterator;
			}
			else {
				next = next->next;
			}
		}while (next != NULL);
	}

	// Create new entry
	iterator.ptr = malloc(sizeof(struct htab_item));

	if(iterator.ptr == NULL){
		return htab_end(t);
	}

	iterator.ptr->key = malloc(strlen(key) + 1);

	if(iterator.ptr->key == NULL){
		return htab_end(t);
	}

	strcpy((char *) iterator.ptr->key, key);
	htab_iterator_set_value(iterator, 0);

	// New entry will in start of the current bucket (list)
	iterator.ptr->next = t->item[index];

	t->item[index] = iterator.ptr;
	t->size++;

	return iterator;
}

void htab_erase(htab_t *t, htab_iterator_t it) {
	if (t == NULL)
		return;

	if (!htab_iterator_valid(it))
		return;

	// Check if entry is on start of bucket
	struct htab_iterator start;
	start.t = t;
	start.ptr = t->item[it.idx];
	start.idx = it.idx;

	if (htab_iterator_equal(it, start)) {

		// Check if entry is alone in bucket
		if (start.ptr->next == NULL) {
			t->item[it.idx] = NULL;
		}
		else {
			t->item[it.idx] = start.ptr->next;
		}
	}
	else {
		// Find previous entry in the same bucket
		while (start.ptr->next != it.ptr) {
			if (start.ptr->next == NULL)
				return;
			start.ptr = start.ptr->next;
		}

		// Check if entry has next entry in same bucket
		if (it.ptr->next != NULL) {
			start.ptr->next = it.ptr->next;
		}
		else {
			start.ptr->next = NULL;
		}
	}
	t->size--;
	free((char *) it.ptr->key);
	free(it.ptr);
}

htab_iterator_t htab_begin(const htab_t *t) {
	struct htab_iterator iterator;
	iterator.t = t;
	iterator.ptr = NULL;
	iterator.idx = 0;

	if(t == NULL){
		return iterator;
	}

	for (size_t i = 0; i < t->arr_size; i++) {
		if(t->item[i] != NULL){
			iterator.ptr = t->item[i];
			iterator.idx = i;
			break;
		}
	}

	return iterator;
}

htab_iterator_t htab_end(const htab_t *t) {
	struct htab_iterator iterator;
	iterator.t = t;
	iterator.ptr = NULL;
	iterator.idx = 0;

	if(t == NULL){
		return iterator;
	}

	for (size_t i = t->arr_size - 1; i > 0; i--) {
		if(t->item[i] != NULL){
			iterator.idx = i + 1;
			break;
		}
	}

	return iterator;
}

htab_iterator_t htab_iterator_next(htab_iterator_t it) {
	struct htab_iterator iterator;
	iterator.t = NULL;
	iterator.ptr = NULL;
	iterator.idx = 0;

	if (!htab_iterator_valid(it))
		return iterator;

	iterator.t = it.t;
	iterator.idx = it.idx;

	// Try to get next entry on same index
	if (it.ptr->next != NULL) {
		iterator.ptr = it.ptr->next;
		return iterator;
	}
	else {
		// Else get next entry from array
		if (iterator.idx + 1 < iterator.t->arr_size) {
			for (size_t i = iterator.idx + 1; i < iterator.t->arr_size; i++) {
				if (it.t->item[i] != NULL) {
					iterator.ptr = it.t->item[i];
					iterator.idx = i;
					return iterator;
				}
			}
		}
	}

	iterator = htab_end(it.t);
	return iterator;
}

htab_key_t htab_iterator_get_key(htab_iterator_t it) {
	assert(htab_iterator_valid(it));

	return it.ptr->key;
}

htab_value_t htab_iterator_get_value(htab_iterator_t it) {
	assert(htab_iterator_valid(it));

	return it.ptr->data;
}

htab_value_t htab_iterator_set_value(htab_iterator_t it, htab_value_t val) {
	assert(htab_iterator_valid(it));

	it.ptr->data = val;

	return val;
}

void htab_clear(htab_t *t) {
	if(t == NULL)
		return;

	struct htab_item *next;
	for (size_t i = 0; i < t->arr_size; ++i) {

		while (t->item[i] != NULL) {
			free((char *) t->item[i]->key);
			next = t->item[i]->next;
			free(t->item[i]);
			t->item[i] = next;
		}
	}
	t->size = 0;
}

void htab_free(htab_t *t) {
	htab_clear(t);
	free(t);
}
