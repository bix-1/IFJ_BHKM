/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: codegen_stack.h
 * Brief: Stack that holds up unique labels for jumps in if/else and for
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "codegen_stack.h"
#include "error.h"
#include <stdio.h>

void jmp_label_stack_init() {
	jmp_label_stack_bottom = malloc(sizeof(jmp_label_stack_t));

	if (jmp_label_stack_bottom == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_init", "NULL pointer");
	}

	// Most bottom of stack must be always defined, its not used for data
	jmp_label_stack_bottom->value = 0;
	jmp_label_stack_bottom->next = NULL;
	jmp_label_stack_bottom->prev = NULL;

	jmp_label_stack_top = malloc(sizeof(jmp_label_stack_top_t));

	if (jmp_label_stack_top == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_init", "NULL pointer");
	}

	// Top points to bottom of stack
	jmp_label_stack_top->top = jmp_label_stack_bottom;
}

void jmp_label_stack_push(int value) {
	if (jmp_label_stack_top == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	jmp_label_stack_t *new_item = malloc(sizeof(jmp_label_stack_t));

	if (new_item == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	new_item->value = value;
	new_item->next = NULL;
	new_item->prev = jmp_label_stack_top->top;

	jmp_label_stack_top->top->next = new_item;
	jmp_label_stack_top->top = new_item;
}

int jmp_label_stack_pop() {
	if (jmp_label_stack_bottom == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	if (jmp_label_stack_top == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	if (jmp_label_stack_top->top == jmp_label_stack_bottom) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "Cannot pop bottom");
	}

	jmp_label_stack_t *temp = jmp_label_stack_top->top;
	int pop_value = temp->value;

	jmp_label_stack_top->top = temp->prev;
	jmp_label_stack_top->top->next = NULL;

	free(temp);
	return pop_value;
}

void jmp_label_stack_empty() {
	if (jmp_label_stack_bottom == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	if (jmp_label_stack_top == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	while (jmp_label_stack_bottom != jmp_label_stack_top->top) {
		jmp_label_stack_pop();
	}
}

void jmp_label_stack_free() {
	if (jmp_label_stack_bottom == NULL && jmp_label_stack_top == NULL) {
		return;
	}

	if (jmp_label_stack_bottom != NULL && jmp_label_stack_top != NULL) {
		jmp_label_stack_empty();

		free(jmp_label_stack_bottom);
		free(jmp_label_stack_top);
	}
	else {
		if (jmp_label_stack_bottom != NULL) {
			free(jmp_label_stack_bottom);
		}

		if (jmp_label_stack_top != NULL) {
			free(jmp_label_stack_top);
		}
	}
}