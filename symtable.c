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
#include "error.h"

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

// If already exists return existing, else create new
symtable_iterator_t symtable_insert(symtable_t *t, symtable_key_t key, symtable_value_t data) {

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

	iterator.ptr->key = key;

	if (iterator.ptr->key == NULL) {
		return symtable_end(t);
	}

	// Assign element key by pointer
	data->key = malloc(sizeof(char**));
	*(data->key) = (char *)key;

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

	// Free key
	free((char *) it.ptr->key);

	switch (it.ptr->data->sym_type) {
		case SYM_FUNC:
			sym_func_free(it.ptr->data->symbol.sym_func);
			break;
		case SYM_VAR_ITEM:
			sym_var_item_free(it.ptr->data->symbol.sym_var_item);
			break;
		case SYM_VAR_LIST:
			sym_var_list_free(it.ptr->data->symbol.sym_var_list);
			break;
		default:
			break;
	}

	// Free element
	free(it.ptr->data);

	// Free item
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
			// Free key
			free((char *) t->item[i]->key);
			next = t->item[i]->next;
			// Free value
			switch (t->item[i]->data->sym_type) {
				case SYM_FUNC:
					sym_func_free(t->item[i]->data->symbol.sym_func);
					break;
				case SYM_VAR_ITEM:
					sym_var_item_free(t->item[i]->data->symbol.sym_var_item);
					break;
				case SYM_VAR_LIST:
					sym_var_list_free(t->item[i]->data->symbol.sym_var_list);
					break;
				default:
					break;
			}

			// Free element's key
			free(t->item[i]->data->key);
			// Free element
			free(t->item[i]->data);

			// Free item
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

elem_t *elem_init(sym_type_t sym_type, symbol_t symbol) {
	elem_t *elem = malloc(sizeof(elem_t));

	if (elem == NULL) {
		error(99, "symtable.c", "elem_init", "Failed to create element");
	}

	// key is private, is created as pointer in table assign
	elem->key = NULL;
	elem->sym_type = sym_type;
	elem->symbol = symbol;

	return elem;
}

sym_func_t *sym_func_init(char *name, sym_var_list_t *params, sym_var_list_t *returns) {
	sym_func_t *sym_func = malloc(sizeof(sym_func_t));

	if (sym_func == NULL) {
		error(99, "symtable.c", "sym_func_init", "Failed to create symbol func");
	}

	sym_func->name = name;
	sym_func->params = params;
	sym_func->returns = returns;

	return sym_func;
}

sym_var_item_t *sym_var_item_init(char *name) {
	sym_var_item_t *sym_var_item = malloc(sizeof(sym_var_item_t));

	if (sym_var_item == NULL) {
		error(99, "symtable.c", "sym_var_item_init", "Failed to create symbol var item");
	}

	sym_var_item->name = name;

	sym_var_item->type = VAR_UNDEFINED;
	sym_var_item->is_const = false;
	sym_var_item->is_global = false;

	return sym_var_item;
}

sym_var_list_t *sym_var_list_init() {
	sym_var_list_t *sym_var_list = malloc(sizeof(sym_var_list_t));

	if (sym_var_list == NULL) {
		error(99, "symtable.c", "sym_var_list_init", "Failed to create symbol var list");
	}

	sym_var_list->first = NULL;
	sym_var_list->active = NULL;

	return sym_var_list;
}

list_item_t *list_item_init(sym_var_item_t *sym_var_item) {
	list_item_t *list_item = malloc(sizeof(list_item_t));

	if (list_item == NULL) {
		error(99, "symtable.c", "list_item_init", "Failed to create list item");
	}

	if (sym_var_item == NULL) {
		error(99, "symtable.c", "list_item_init", "Failed to create list item");
	}

	list_item->item = sym_var_item;
	list_item->next = NULL;
	list_item->prev = NULL;

	return list_item;
}

void sym_func_free(sym_func_t *sym_func) {
	if (sym_func == NULL) {
		error(99, "symtable.c", "sym_func_free", "Failed to free symbol func");
	}

	sym_var_list_t *sym_func_params = sym_func->params;
	sym_var_list_t *sym_func_returns = sym_func->returns;

	if (sym_func_params != NULL) {
		free(sym_func_params);
	}

	if (sym_func_returns != NULL) {
		sym_var_list_free(sym_func_returns);
	}

	free(sym_func);
}

void sym_var_item_free(sym_var_item_t *sym_var_item) {
	if (sym_var_item == NULL) {
		error(99, "symtable.c", "sym_var_item_free", "Failed to free symbol var item");
	}

	if (sym_var_item->name != NULL)
		free(sym_var_item->name);

	if (sym_var_item->type == VAR_STRING) {
		if (sym_var_item->data.string_t != NULL) {
			free(sym_var_item->data.string_t);
			free(sym_var_item->default_data.string_t);
		}
	}

	free(sym_var_item);
}

void sym_var_list_free(sym_var_list_t *sym_var_list) {
	if (sym_var_list == NULL) {
		error(99, "symtable.c", "sym_var_list_free", "Failed to free symbol list");
	}

	// content of sym_var_list is stored in symtable
	// and is freed separately as elem_ts
	list_item_t *current = sym_var_list->first;
	list_item_t *next = NULL;

	while (current != NULL) {
		next = current->next;
		list_item_free(current);
		current = next;
	}

	free(sym_var_list);
}

void list_item_free(list_item_t *list_item) {
	if (list_item == NULL) {
		error(99, "symtable.c", "list_item_free", "Failed to free list item");
	}

	free(list_item);
}

void sym_var_item_set_type(sym_var_item_t *sym_var_item, var_type_t type) {
	if (sym_var_item == NULL) {
		error(99, "symtable.c", "sym_var_item_set_type", "Failed to set type for var item");
	}

	sym_var_item->type = type;
}

void sym_var_item_set_data(sym_var_item_t *sym_var_item, variable_t data) {
	if (sym_var_item == NULL) {
		error(99, "symtable.c", "sym_var_item_set_data", "Failed to set data for var item");
	}

	if (sym_var_item->type == VAR_UNDEFINED) {
		error(99, "symtable.c", "sym_var_item_set_data", "Cannot assign data to undefined variable");
	}
	else if (sym_var_item->type == VAR_STRING) {
		sym_var_item->data.string_t = malloc(strlen(data.string_t) + 1);

		if (sym_var_item->data.string_t == NULL) {
			error(99, "symtable.c", "sym_var_item_init", "Failed to create symbol var item");
		}

		strcpy(sym_var_item->data.string_t, data.string_t);
	}
	else {
		sym_var_item->data = data;
	}

	sym_var_item->default_data = data;
}

void sym_var_item_set_const(sym_var_item_t *sym_var_item, bool is_const) {
	if (sym_var_item == NULL) {
		error(99, "symtable.c", "sym_var_item_set_const", "Failed to set is_const for var item");
	}

	sym_var_item->is_const = is_const;
}

void sym_var_item_set_global(sym_var_item_t *sym_var_item, bool is_global) {
	if (sym_var_item == NULL) {
		error(99, "symtable.c", "sym_var_item_set_global", "Failed to set is_global for var item");
	}

	sym_var_item->is_global = is_global;
}

void list_item_set_next(list_item_t *list_item, list_item_t *next) {
	if (list_item == NULL) {
		error(99, "symtable.c", "sym_var_item_set_next", "Failed to set next for var item");
	}

	list_item->next = next;
}

void list_item_set_prev(list_item_t *list_item, list_item_t *prev) {
	if (list_item == NULL) {
		error(99, "symtable.c", "sym_var_item_set_prev", "Failed to set prev for var item");
	}

	list_item->prev = prev;
}

symtable_key_t elem_key(elem_t *elem) {
	if (elem == NULL) {
		error(99, "symtable.c", "elem_key", "Failed to get key of element");
	}

	return *(elem->key);
}

void sym_var_list_add(sym_var_list_t *sym_var_list, list_item_t *list_item) {
	if (sym_var_list == NULL || list_item == NULL) {
		error(99, "symtable.c", "sym_var_list_add", "Failed to add symbol item");
	}

	if (sym_var_list->first == NULL) {
		sym_var_list->first = list_item;
		sym_var_list->active = list_item;
		list_item_set_next(list_item, NULL);
		list_item_set_prev(list_item, NULL);
	}
	else {
		list_item_t *current = sym_var_list->first;

		while (current->next != NULL) {
			current = current->next;
		}

		list_item_set_next(current, list_item);
		list_item_set_prev(list_item, current);

		list_item_set_next(list_item, NULL);
	}
}

size_t sym_var_list_size(sym_var_list_t *sym_var_list) {
	if (sym_var_list == NULL) {
		error(99, "symtable.c", "sym_var_list_size", "Failed to get size of sym list");
	}

	size_t size = 0;

	list_item_t *current = sym_var_list->first;

	while (current != NULL) {
		size++;
		current = current->next;
	}

	return size;
}

void sym_var_list_pop(sym_var_list_t *sym_var_list) {
	if (sym_var_list == NULL) {
		error(99, "symtable.c", "sym_var_list_pop", "Failed to pop sym var item in sym list");
	}

	if (sym_var_list->first == NULL) {
		return;
	}
	else {
		list_item_t *prev = NULL;
		list_item_t *current = sym_var_list->first;

		while (current->next != NULL) {
			prev = current;
			current = current->next;
		}

		if (current == sym_var_list->active) {
			// active item lost
			sym_var_list->active = NULL;
		}

		if (prev != NULL) {
			prev->next = NULL;
		}

		list_item_free(current);
	}
}

sym_var_item_t *sym_var_list_next(sym_var_list_t *sym_var_list) {
	if (sym_var_list == NULL) {
		error(99, "symtable.c", "sym_var_list_next", "Failed to get next sym var item in sym list");
	}

	if (sym_var_list->active == NULL) {
		sym_var_list->active = sym_var_list->first;
	}
	else {
		list_item_t * next = sym_var_list->active->next;
		if (next == NULL) return NULL;
		sym_var_list->active = next;
	}

	return sym_var_list->active->item;
}

sym_var_item_t *sym_var_list_prev(sym_var_list_t *sym_var_list) {
	if (sym_var_list == NULL) {
		error(99, "symtable.c", "sym_var_list_inv_next", "Failed to get prev sym var item in sym list");
	}

	if (sym_var_list->active == NULL) {
		if (sym_var_list->first == NULL) {
			return NULL;
		}

		list_item_t *current = sym_var_list->first;

		while (current->next != NULL) {
			current = current->next;
		}

		sym_var_list->active = current;
	}
	else {
		sym_var_list->active = sym_var_list->active->prev;
	}

	return sym_var_list->active->item;
}

sym_var_item_t *sym_var_list_get_active(sym_var_list_t *sym_var_list) {
	if (sym_var_list == NULL) {
		error(99, "symtable.c", "sym_var_list_get_active", "Failed to get active sym var item in sym list");
	}

	return sym_var_list->active->item;
}

void sym_var_list_clear(sym_var_list_t *sym_var_list) {
	if (sym_var_list == NULL) {
		error(99, "symtable.c", "sym_var_list_clear", "Failed to clear sym var items in sym list");
	}

	list_item_t *current = sym_var_list->first;
	list_item_t *next = NULL;

	while (current != NULL) {
		next = current->next;
		if (current->item != NULL) {
			sym_var_item_free(current->item);
		}
		list_item_free(current);
		current = next;
	}

	sym_var_list->first = NULL;
	sym_var_list->active = NULL;
}

void sym_var_item_change_data(sym_var_item_t *sym_var_item, variable_t data) {
	if (sym_var_item == NULL) {
		error(99, "symtable.c", "sym_var_item_change_data", "Failed to set sym var item to new value");
	}

	if (sym_var_item->type == VAR_STRING) {
		free(sym_var_item->default_data.string_t);
		sym_var_item->data.string_t = data.string_t;
	}
	else {
		sym_var_item->data = data;
	}
}

void sym_var_item_reset(sym_var_item_t *sym_var_item) {
	if (sym_var_item == NULL) {
		error(99, "symtable.c", "sym_var_item_reset", "Failed to reset sym var item to default value");
	}

	if (sym_var_item->is_const) {
		error(99, "symtable.c", "sym_var_item_reset", "Cannot reset value to const");
	}

	if (sym_var_item->type == VAR_STRING) {
		sym_var_item->data.string_t = realloc(sym_var_item->data.string_t,
		                                      strlen(sym_var_item->default_data.string_t) + 1);

		if (sym_var_item->data.string_t == NULL) {
			error(99, "symtable.c", "sym_var_item_reset", "Failed to reset sym var item to default value");
		}

		strcpy(sym_var_item->data.string_t, sym_var_item->default_data.string_t);
	}
	else {
		sym_var_item->data = sym_var_item->default_data;
	}
}
