/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: codegen_test.c
 * Brief: Test with multiple functions, params, returns, add, print
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../symtable.h"
#include "../../ll.h"
#include "test_codegen_helper.h"

int main() {

	// Test with multiple functions, params, returns, add, print
	test_init();

	variable_t new_line_data = {.string_t = "\n"};
	elem_t *new_line = create_var_item("new_line", VAR_STRING, new_line_data, true, false);

	variable_t bar_var_y_data = {.int_t = 0};
	elem_t *bar_var_y = create_var_item("y", VAR_INT, bar_var_y_data, false, false);

	variable_t bar_const_24_data = {.int_t = 24};
	elem_t *bar_const_24 = create_var_item("bar-24", VAR_INT, bar_const_24_data, true, false);

	variable_t main_var_a_data = {.int_t = 42};
	elem_t *main_var_a = create_var_item("a", VAR_INT, main_var_a_data, false, false);

	variable_t main_var_b_data = {.int_t = 0};
	elem_t *main_var_b = create_var_item("b", VAR_INT, main_var_b_data, false, false);

	variable_t main_var_c_data = {.string_t = "Hello\n"};
	elem_t *main_var_c = create_var_item("c", VAR_STRING, main_var_c_data, false, false);

	elem_t *foo_params = create_var_list();
	elem_t *bar_params = create_var_list();
	elem_t *bar_returns = create_var_list();

	elem_t *foo_print1 = create_var_list();
	elem_t *main_print1 = create_var_list();
	elem_t *main_print2 = create_var_list();

	elem_t *main_b_list = create_var_list();

	var_list_add(foo_params->symbol.sym_var_list, main_var_a->symbol.sym_var_item);
	var_list_add(bar_params->symbol.sym_var_list, main_var_a->symbol.sym_var_item);
	var_list_add(bar_returns->symbol.sym_var_list, bar_var_y->symbol.sym_var_item);

	var_list_add(foo_print1->symbol.sym_var_list, main_var_a->symbol.sym_var_item);
	var_list_add(foo_print1->symbol.sym_var_list, new_line->symbol.sym_var_item);

	var_list_add(main_print1->symbol.sym_var_list, main_var_b->symbol.sym_var_item);
	var_list_add(main_print1->symbol.sym_var_list, new_line->symbol.sym_var_item);

	var_list_add(main_print2->symbol.sym_var_list, main_var_c->symbol.sym_var_item);

	var_list_add(main_b_list->symbol.sym_var_list, main_var_b->symbol.sym_var_item);

	elem_t *foo = create_function("foo", foo_params->symbol.sym_var_list, NULL);
	elem_t *bar = create_function("bar", bar_params->symbol.sym_var_list, bar_returns->symbol.sym_var_list);
	elem_t *main = create_function("main", NULL, NULL);

	add_instruction(IC_DEF_FUN, foo, NULL, NULL);
	add_instruction(IC_DECL_VAR, new_line, NULL, NULL);
	add_instruction(IC_WRITE_VAR, foo_print1, NULL, NULL);
	add_instruction(IC_END_FUN, foo, NULL, NULL);
	add_instruction(IC_DEF_FUN, bar, NULL, NULL);
	add_instruction(IC_DECL_VAR, bar_var_y, NULL, NULL);
	add_instruction(IC_DECL_VAR, bar_const_24, NULL, NULL);
	add_instruction(IC_ADD_VAR, bar_var_y, main_var_a, bar_const_24);
	add_instruction(IC_RET_FUN, bar, NULL, NULL);
	add_instruction(IC_END_FUN, bar, NULL, NULL);
	add_instruction(IC_DEF_FUN, main, NULL, NULL);
	add_instruction(IC_DECL_VAR, main_var_a, NULL, NULL);
	add_instruction(IC_CALL_FUN, NULL, foo, NULL);
	add_instruction(IC_DECL_VAR, main_var_b, NULL, NULL);
	add_instruction(IC_CALL_FUN, main_b_list, bar, NULL);
	add_instruction(IC_DECL_VAR, new_line, NULL, NULL);
	add_instruction(IC_WRITE_VAR, main_print1, NULL, NULL);
	add_instruction(IC_DECL_VAR, main_var_c, NULL, NULL);
	add_instruction(IC_WRITE_VAR, main_print2, NULL, NULL);
	add_instruction(IC_RET_FUN, main, NULL, NULL);
	add_instruction(IC_END_FUN, main, NULL, NULL);

	codegen();

	// FIXME : fix free issues
	test_free_all();
	return EXIT_SUCCESS;
}
