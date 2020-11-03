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


typedef struct instruction instr_t;
struct instruction {
  int type;
  const char * elem1;
  const char * elem2;
  instr_t * next;
};

typedef struct {
  instr_t * first;
  instr_t * active;
} list_t;


// List functions
list_t *  list_create   ();
void    list_destroy  ( list_t ** );
int     list_size     ( list_t * );
void    list_add      ( list_t *, instr_t * );
instr_t * list_get_active ( list_t * );
instr_t * list_next ( list_t * );

// Instruction functions
instr_t * instr_create  ();
void  instr_set_type ( instr_t *, int );
int   instr_get_type ( instr_t * );
void instr_add_elem ( instr_t *, const char * );
const char * instr_get_elem1 ( instr_t * );
const char * instr_get_elem2 ( instr_t * );




// Global variables
list_t * list;


#endif
