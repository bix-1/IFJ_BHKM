/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: test_codegen_add1.c
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
	test_init();

	//main func
	elem_t *main = create_function("main", NULL, NULL);

	variable_t i_data = {.int_t = 0};
	elem_t *i = create_var_item("i", VAR_INT, i_data, false, false);

	variable_t b_data = {.bool_t = false};
	elem_t *b = create_var_item("b", VAR_BOOL, b_data, false, false);

	elem_t *print_list = create_var_list();
	var_list_add(print_list->symbol.sym_var_list, i->symbol.sym_var_item);
	elem_t *print_fu = create_function("printf", print_list->symbol.sym_var_list, NULL);

	variable_t c_10_data = {.int_t = 10};
	elem_t *c_10 = create_var_item("c10", VAR_INT, c_10_data, true, false);

	variable_t c_1_data = {.int_t = 1};
	elem_t *c_1 = create_var_item("c1", VAR_INT, c_1_data, true, false);

	add_instruction(IC_DEF_FUN, main, NULL, NULL);
	add_instruction(IC_FOR_DEF,NULL, NULL, NULL);
	add_instruction(IC_DECL_VAR, i, NULL, NULL);
	add_instruction(IC_DECL_VAR, b, NULL, NULL);
	add_instruction(IC_FOR_COND, NULL, NULL, NULL);
	add_instruction(IC_LT_VAR, b, i, c_10);
	add_instruction(IC_FOR_STEP, NULL, NULL, NULL);
	add_instruction(IC_ADD_VAR, i, i, c_1);
	add_instruction(IC_FOR_BODY_START, NULL, NULL, NULL);
	add_instruction(IC_WRITE_VAR, print_fu, NULL, NULL);
	add_instruction(IC_FOR_BODY_END, NULL, NULL, NULL);
	add_instruction(IC_END_FUN, main, NULL, NULL);

	codegen();

	test_free_all();
	return EXIT_SUCCESS;
}
