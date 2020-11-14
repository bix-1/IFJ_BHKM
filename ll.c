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

  return l;
}


void list_destroy (list_t ** l)
{
  if (l == NULL || *l == NULL) return;

  instr_t * curr = (*l)->first,
          * next;
  while (curr != NULL) {
    next = curr->next;
    free( curr );
    curr = next;
  }

  free(*l);
  *l = NULL;
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
    l->first = i; l->active = i;
    return;
  }
  // get to last element
  instr_t * tmp = l->first;
  while (tmp->next != NULL)
    tmp = tmp->next;
  tmp->next = i;  // append to end of list
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
  i->elem_dest_key = NULL;
  i->elem1_key = NULL;
  i->elem2_key = NULL;
  i->next = NULL;

  return i;
}

void instr_set_type (instr_t * i, instr_type_t t)
{
  if (i != NULL)  i->type = t;
}

int instr_get_type (instr_t *i)
{
  if (i == NULL) return -1;
  return i->type;
}

void instr_add_dest (instr_t *i, const char *dest) {
  if (i != NULL)
    i->elem_dest_key = (char *)dest;
}

void instr_add_elem1 (instr_t * i, const char * e)
{
  if (i != NULL) i->elem1_key = (char *)e;
}

void instr_add_elem2 (instr_t * i, const char * e)
{
  if (i != NULL) i->elem2_key = (char *)e;
}

elem_t instr_get_dest (instr_t * i)
{
  if (i == NULL || i->elem_dest_key == NULL || symtable == NULL) return NULL;
  symtable_iterator_t tmp = symtable_find(symtable, i->elem_dest_key);
  if (tmp.ptr == NULL) return NULL;
  return tmp.ptr->data;
}

elem_t instr_get_elem1 (instr_t * i)
{
  if (i == NULL || i->elem1_key == NULL || symtable == NULL) return NULL;
  symtable_iterator_t tmp = symtable_find(symtable, i->elem1_key);
  if (tmp.ptr == NULL) return NULL;
  return symtable_iterator_get_value(tmp);
}

elem_t instr_get_elem2 (instr_t * i)
{
  if (i == NULL || i->elem2_key == NULL || symtable == NULL) return NULL;
  symtable_iterator_t tmp = symtable_find(symtable, i->elem2_key);
  if (tmp.ptr == NULL) return NULL;
  return tmp.ptr->data;
}
