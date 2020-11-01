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


List * list_create ()
{
  // create list
  List * l = malloc( sizeof(List) );
  if ( l == NULL )
    error( 99, "ll.c", "list_create", "Failed to create list" );
  // init list
  l->first = NULL;
  l->active = NULL;

  return l;
}


void list_destroy ( List ** l )
{
  if ( l == NULL || *l == NULL ) return;

  instr_t * curr = (*l)->first,
              * next;
  while ( curr != NULL )
  {
    // free instr
    next = curr->next;
    free( curr );
    curr = next;
  }

  free( *l );
  *l = NULL;
}


int list_size ( List * l )
{
  if ( l == NULL ) return -1;
  if ( l->first == NULL ) return 0;
  int n = 0;
  instr_t * tmp = l->first;
  while ( tmp != NULL )
  {
    tmp = tmp->next;
    n++;
  }
  return n;
}


void list_add ( List *l, instr_t * i )
{
  if ( l == NULL || i == NULL ) return;
  // list empty
  if ( l->first == NULL )
  {
    l->first = i; l->active = i;
    return;
  }
  // get to last element
  instr_t * tmp = l->first;
  while ( tmp->next != NULL )
    tmp = tmp->next;
  // append to the end of list
  tmp->next = i;
}


instr_t * list_get_active ( List * l )
{
  if ( l == NULL ) return NULL;
  return l->active;
}


instr_t * list_next ( List * l )
{
  if ( l == NULL || l->active == NULL ) return NULL;
  l->active = l->active->next;
  return l->active;
}



instr_t * instr_create ()
{
  // create instruction
  instr_t * i = malloc( sizeof(instr_t) );
  if ( i == NULL )
    error( 99, "ll.c", "instr_create", "Failed to create instruction" );
  // initialize instruction
  i->type   = -1;
  i->elem1  = NULL;
  i->elem2  = NULL;
  i->next   = NULL;

  return i;
}

void instr_set_type ( instr_t * i, int t )
{
  if ( i != NULL )  i->type = t;
}

int instr_get_type ( instr_t *i )
{
  if ( i == NULL ) return -1;
  return i->type;
}


void instr_add_elem ( instr_t * i, const char * e )
{
  // invalid or full
  if ( i == NULL || i->elem2 != NULL ) return;
  if ( i->elem1 == NULL ) // first free
    i->elem1 = e;
  else
    i->elem2 = e;
}

const char * instr_get_elem1 ( instr_t * i )
{
  if ( i != NULL ) return i->elem1;
  return NULL;
}

const char * instr_get_elem2 ( instr_t * i )
{
  if ( i != NULL ) return i->elem2;
  return NULL;
}