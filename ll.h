/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: ll.h
 * Brief: Header file for ll.h
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */


#ifndef LL_H
#define LL_H

#include "symtable.h" // instr_t
#include <stdbool.h>  // list_is_empty

typedef struct instruction instr_t;
struct instruction {
	instr_type_t type;
	elem_t *elem_dest_ptr;
	elem_t *elem1_ptr;
	elem_t *elem2_ptr;
	instr_t *next;
};

typedef struct {
  instr_t * first;
  instr_t * active;
  instr_t * last;
} list_t;

// List functions
list_t * list_create();
void list_destroy (list_t **);
bool list_is_empty(list_t *);
int list_size (list_t *);
void list_add (list_t *, instr_t *);
instr_t * list_get_active (list_t *);
instr_t * list_get_next (list_t *);

// Instruction functions
instr_t * instr_create(); // creates empty instr
// if exists -- init; else create && init
void instr_init(instr_t **, instr_type_t);
void instr_set_type (instr_t *, instr_type_t);
int instr_get_type (instr_t *);
void instr_add_dest (instr_t *, elem_t *);
void instr_add_elem1 (instr_t *, elem_t *);
void instr_add_elem2 (instr_t *, elem_t *);
elem_t * instr_get_dest (instr_t *);
elem_t * instr_get_elem1 (instr_t *);
elem_t * instr_get_elem2 (instr_t *);

// Global variables
list_t * list;
extern symtable_t * symtable;


#endif
