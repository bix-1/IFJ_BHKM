/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: symtable.c
 * Brief: Implementation of hash table functions for symtable
 * Contribution: Used from IJC 2nd project, implemented by xhladk15
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "symtable.h"

extern bool symtable_iterator_valid(symtable_iterator_t it);

extern bool symtable_iterator_equal(symtable_iterator_t it1, symtable_iterator_t it2);

symtable_t *symtable_init(size_t n) {
	assert(n > 0);

	symtable_t *symtable = malloc(sizeof(symtable_t) + sizeof(struct symtable_item) * n);

	if (symtable == NULL)
		return NULL;

	memset(symtable, 0, sizeof(symtable_t) + sizeof(struct symtable_item) * n);

	symtable->arr_size = n;
	symtable->size = 0;

	return symtable;
}

size_t symtable_size(const symtable_t *t) {
	if (t == NULL)
		return 0;

	return t->size;
}

size_t symtable_bucket_count(const symtable_t *t) {
	if (t == NULL)
		return 0;

	return t->arr_size;
}

size_t symtable_hash_fun(const char *str) {
	uint32_t h = 0;
	const unsigned char *p;
	for (p = (const unsigned char *) str; *p != '\0'; p++)
		h = 65599 * h + *p;
	return h;
}

symtable_iterator_t symtable_find(symtable_t *t, symtable_key_t key) {

	if (t == NULL)
		return symtable_end(t);

	if (key == NULL)
		return symtable_end(t);

	struct symtable_iterator iterator;

	// Get index from hash function
	size_t index = (symtable_hash_fun(key) % t->arr_size);

	iterator.t = t;
	iterator.idx = index;

	struct symtable_item *next = t->item[index];

	// Try to find existing
	if (next != NULL) {
		do {
			if (strcmp(next->key, key) == 0) {
				iterator.ptr = next;
				return iterator;
			}
			else {
				next = next->next;
			}
		} while (next != NULL);
	}

	iterator = symtable_end(t);
	return iterator;
}

// Todo: change to insert
symtable_iterator_t symtable_lookup_add(symtable_t *t, symtable_key_t key, symtable_value_t data) {

	if (t == NULL)
		return symtable_end(t);

	if (key == NULL)
		return symtable_end(t);

	struct symtable_iterator iterator;

	// Get index from hash function
	size_t index = (symtable_hash_fun(key) % t->arr_size);

	iterator.t = t;
	iterator.idx = index;

	struct symtable_item *next = t->item[index];

	// Try to find existing entry
	if (next != NULL) {
		do {
			if (strcmp(next->key, key) == 0) {
				iterator.ptr = next;
				return iterator;
			}
			else {
				next = next->next;
			}
		} while (next != NULL);
	}

	// Create new entry
	iterator.ptr = malloc(sizeof(struct symtable_item));

	if (iterator.ptr == NULL) {
		return symtable_end(t);
	}

	iterator.ptr->key = malloc(strlen(key) + 1);

	if (iterator.ptr->key == NULL) {
		return symtable_end(t);
	}

	/*
	elem_t *elem = malloc(sizeof(struct elem));

	if (elem == NULL) {
	   return symtable(t);
	}

	elem->key = malloc(strlen(key) + 1);

	if (elem->key == NULL) {
	   return symtable_end(t);
	}

	elem->sym_type = undefined
	elem->symbol = undefined
	strcpy((char *) elem->key, key);
	*/
	//elem_t *elem = NULL;

	strcpy((char *) iterator.ptr->key, key);

	symtable_iterator_set_value(iterator, data);

	// New entry will in start of the current bucket (list)
	iterator.ptr->next = t->item[index];

	t->item[index] = iterator.ptr;
	t->size++;

	return iterator;
}

void symtable_erase(symtable_t *t, symtable_iterator_t it) {
	if (t == NULL)
		return;

	if (!symtable_iterator_valid(it))
		return;

	// Check if entry is on start of bucket
	struct symtable_iterator start;
	start.t = t;
	start.ptr = t->item[it.idx];
	start.idx = it.idx;

	if (symtable_iterator_equal(it, start)) {

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

symtable_iterator_t symtable_begin(const symtable_t *t) {
	struct symtable_iterator iterator;
	iterator.t = t;
	iterator.ptr = NULL;
	iterator.idx = 0;

	if (t == NULL) {
		return iterator;
	}

	for (size_t i = 0; i < t->arr_size; i++) {
		if (t->item[i] != NULL) {
			iterator.ptr = t->item[i];
			iterator.idx = i;
			break;
		}
	}

	return iterator;
}

symtable_iterator_t symtable_end(const symtable_t *t) {
	struct symtable_iterator iterator;
	iterator.t = t;
	iterator.ptr = NULL;
	iterator.idx = 0;

	if (t == NULL) {
		return iterator;
	}

	for (size_t i = t->arr_size - 1; i > 0; i--) {
		if (t->item[i] != NULL) {
			iterator.idx = i + 1;
			break;
		}
	}

	return iterator;
}

symtable_iterator_t symtable_iterator_next(symtable_iterator_t it) {
	struct symtable_iterator iterator;
	iterator.t = NULL;
	iterator.ptr = NULL;
	iterator.idx = 0;

	if (!symtable_iterator_valid(it))
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

	iterator = symtable_end(it.t);
	return iterator;
}

symtable_key_t symtable_iterator_get_key(symtable_iterator_t it) {
	assert(symtable_iterator_valid(it));

	return it.ptr->key;
}

symtable_value_t symtable_iterator_get_value(symtable_iterator_t it) {
	assert(symtable_iterator_valid(it));

	return it.ptr->data;
}

symtable_value_t symtable_iterator_set_value(symtable_iterator_t it, symtable_value_t val) {
	assert(symtable_iterator_valid(it));

	it.ptr->data = val;

	return val;
}

void symtable_clear(symtable_t *t) {
	if (t == NULL)
		return;

	struct symtable_item *next;
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

void symtable_free(symtable_t *t) {
	symtable_clear(t);
	free(t);
}
