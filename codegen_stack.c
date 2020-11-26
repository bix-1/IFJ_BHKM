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

#include <stdio.h>
#include "error.h"
#include "codegen_stack.h"

jmp_label_stack_t *skip_labels_bottom = NULL;
jmp_label_stack_top_t *skip_labels_top = NULL;

jmp_label_stack_t *end_labels_bottom = NULL;
jmp_label_stack_top_t *end_labels_top = NULL;

void jmp_label_stack_init() {
	// Skip labels
	skip_labels_bottom = malloc(sizeof(jmp_label_stack_t));

	if (skip_labels_bottom == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_init", "NULL pointer");
	}

	// Most bottom of stack must be always defined, its not used for data
	skip_labels_bottom->value = 0;
	skip_labels_bottom->next = NULL;
	skip_labels_bottom->prev = NULL;

	skip_labels_top = malloc(sizeof(jmp_label_stack_top_t));

	if (skip_labels_top == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_init", "NULL pointer");
	}

	// Top points to bottom of stack
	skip_labels_top->top = skip_labels_bottom;

	// End labels
	end_labels_bottom = malloc(sizeof(jmp_label_stack_t));

	if (end_labels_bottom == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_init", "NULL pointer");
	}

	// Most bottom of stack must be always defined, its not used for data
	end_labels_bottom->value = 0;
	end_labels_bottom->next = NULL;
	end_labels_bottom->prev = NULL;

	end_labels_top = malloc(sizeof(jmp_label_stack_top_t));

	if (end_labels_top == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_init", "NULL pointer");
	}

	// Top points to bottom of stack
	end_labels_top->top = end_labels_bottom;
}

void jmp_label_stack_push(jmp_label_stack_top_t *top, int value) {
	if (top == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	jmp_label_stack_t *new_item = malloc(sizeof(jmp_label_stack_t));

	if (new_item == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	new_item->value = value;
	new_item->next = NULL;
	new_item->prev = top->top;

	top->top->next = new_item;
	top->top = new_item;
}

int jmp_label_stack_pop(jmp_label_stack_t *stack, jmp_label_stack_top_t *top) {
	if (stack == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	if (top == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	if (top->top == stack) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "Cannot pop bottom");
	}

	jmp_label_stack_t *temp = top->top;
	int pop_value = temp->value;

	top->top = temp->prev;
	top->top->next = NULL;

	free(temp);
	return pop_value;
}

int jmp_label_stack_top(jmp_label_stack_top_t *top) {
	return top->top->value;
}

void jmp_label_stack_clear(jmp_label_stack_t *stack, jmp_label_stack_top_t *top) {
	if (stack == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	if (top == NULL) {
		error(99, "codegen_stack.c", "jmp_label_stack_push", "NULL pointer");
	}

	while (stack != top->top) {
		jmp_label_stack_pop(stack, top);
	}
}

void jmp_label_stack_free_all() {
	jmp_label_stack_free(skip_labels_bottom, skip_labels_top);
	jmp_label_stack_free(end_labels_bottom, end_labels_top);
}

void jmp_label_stack_free(jmp_label_stack_t *stack, jmp_label_stack_top_t *top) {
	if (stack == NULL && top == NULL) {
		return;
	}

	if (stack != NULL && top != NULL) {
		jmp_label_stack_clear(stack, top);

		free(stack);
		free(top);
	}
	else {
		if (stack != NULL) {
			free(stack);
		}

		if (top != NULL) {
			free(top);
		}
	}
}