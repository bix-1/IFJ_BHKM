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
  List * l = malloc( sizeof(List) );
  if ( l == NULL )  error( 99, "ll.c: list_create: Failed to create list" );
  l->first = NULL;
  return l;
}

void list_destroy ( List * l )
{
  if ( l == NULL ) return;

  Instruction * curr = l->first,
              * next;
  while ( curr != NULL )
  {
    // free elems
    if ( curr->elem1 != NULL ) free( curr->elem1 );
    if ( curr->elem2 != NULL ) free( curr->elem2 );
    // free instr
    next = curr->next;
    free( curr );
    curr = next;
  }

  free( l );
}
