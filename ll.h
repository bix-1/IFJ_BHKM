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


typedef struct Instruction Instruction;
struct Instruction {
  int type;
  char * elem1;
  char * elem2;
  Instruction * next;
};

typedef struct {
  Instruction * first;
} List;


List *  list_create       ();
void    list_destroy      ( List * );




#endif
