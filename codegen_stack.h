/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: codegen_stack.h
 * Brief: codegen_stack.c header file.
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#ifndef CODEGEN_STACK_H
#define CODEGEN_STACK_H

#include <stdlib.h>

typedef struct jmp_label_stack jmp_label_stack_t;
struct jmp_label_stack {
	jmp_label_stack_t *prev;
	jmp_label_stack_t *next;
	int value;
};

typedef struct jmp_label_stack_top jmp_label_stack_top_t;
struct jmp_label_stack_top {
	jmp_label_stack_t *top;
};

void jmp_label_stack_init();
void jmp_label_stack_push(jmp_label_stack_top_t *top, int value);
int jmp_label_stack_pop(jmp_label_stack_t *stack, jmp_label_stack_top_t *top);
int jmp_label_stack_top(jmp_label_stack_top_t *top);
void jmp_label_stack_clear(jmp_label_stack_t *stack, jmp_label_stack_top_t *top);
void jmp_label_stack_free(jmp_label_stack_t *stack, jmp_label_stack_top_t *top);

// global jump label stack
jmp_label_stack_t *skip_labels_bottom = NULL;
jmp_label_stack_top_t *skip_labels_top = NULL;

jmp_label_stack_t *end_labels_bottom = NULL;
jmp_label_stack_top_t *end_labels_top = NULL;

#endif // CODEGEN_STACK_H
