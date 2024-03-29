/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: ll.c
 * Brief: Implementations of linked list functions
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */


#include "ll.h"
#include "error.h"
#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>

/*_________________________________________________________*/
/*_____________________List functions______________________*/

list_t * list_create()
{
  // create list
  list_t * l = malloc(sizeof(list_t));
  if (l == NULL)
    error(99, "ll", "list_create", "Failed to create instr_list");
  // init list
  l->first = NULL;
  l->active = NULL;
  l->last = NULL;

  return l;
}


void list_destroy (list_t ** l)
{
  if (l == NULL || *l == NULL) return;

  instr_t * curr = (*l)->first,
          * next;
  while (curr != NULL) {
    next = curr->next;
    free(curr);
    curr = next;
  }

  free(*l);
  *l = NULL;
}


bool list_is_empty(list_t *l)
{
  if (l == NULL || l->first == NULL) return true;
  return false;
}


int list_size (list_t * l)
{
  if (l == NULL) return -1;
  int n = 0;
  instr_t * tmp = l->first;
  while (tmp != NULL) {
    tmp = tmp->next;
    n++;
  }
  return n;
}


void list_add (list_t *l, instr_t * i)
{
  if (l == NULL || i == NULL) return;

  if (l->first == NULL) {   // list empty
    l->first = i;
    l->active = i;
    l->last = i;
    return;
  }

  if (l->last == NULL) return;
  l->last->next = i;
  l->last = i;
}


instr_t * list_get_active (list_t * l)
{
  if (l == NULL) return NULL;
  return l->active;
}


instr_t * list_get_next (list_t * l)
{
  if (l == NULL || l->active == NULL) return NULL;
  l->active = l->active->next;
  return l->active;
}
/*******************END of List functions*******************/
/***********************************************************/


/*_________________________________________________________*/
/*__________________Instruction functions__________________*/

instr_t * instr_create()
{
  // create instruction
  instr_t * i = malloc(sizeof(instr_t));
  if (i == NULL)
    error(99, "ll", "instr_create", "Failed to create instruction");
  // initialize instruction
  i->type = -1;
  i->elem_dest_ptr = NULL;
  i->elem1_ptr = NULL;
  i->elem2_ptr = NULL;
  i->next = NULL;

  return i;
}

void instr_init(instr_t **i, instr_type_t t)
{
  if (i == NULL) return;
  if (*i == NULL) {
    *i = malloc(sizeof(instr_t));
    if (*i == NULL)
      error(99, "ll", "instr_init", "Failed to create instruction");
  }
  // initialize instruction
  (*i)->type = t;
  (*i)->elem_dest_ptr = NULL;
  (*i)->elem1_ptr = NULL;
  (*i)->elem2_ptr = NULL;
  (*i)->next = NULL;
}

void instr_set_type (instr_t *i, instr_type_t t)
{
  if (i != NULL) i->type = t;
}

int instr_get_type (instr_t *i)
{
  if (i == NULL) return -1;
  return i->type;
}

void instr_add_dest (instr_t *i, elem_t *dest) {
  if (i != NULL)
    i->elem_dest_ptr = dest;
}

void instr_add_elem1 (instr_t * i, elem_t * e)
{
  if (i != NULL) i->elem1_ptr = e;
}

void instr_add_elem2 (instr_t * i, elem_t * e)
{
  if (i != NULL) i->elem2_ptr = e;
}

elem_t * instr_get_dest (instr_t * i)
{
  if (i == NULL) return NULL;
  return i->elem_dest_ptr;
}

elem_t * instr_get_elem1 (instr_t * i)
{
  if (i == NULL) return NULL;
  return i->elem1_ptr;
}

elem_t * instr_get_elem2 (instr_t * i)
{
  if (i == NULL) return NULL;
  return i->elem2_ptr;
}
