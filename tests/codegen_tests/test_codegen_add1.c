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

	variable_t x_data = {.int_t = 0};
	elem_t *x = create_var_item("x", VAR_INT, x_data, false, false);

	variable_t y_data = {.int_t = 5};
	elem_t *y = create_var_item("y", VAR_INT, y_data, true, false);

	variable_t z_data = {.int_t = 10};
	elem_t *z = create_var_item("z", VAR_INT, z_data, true, false);

	add_instruction(IC_DEF_FUN, main, NULL, NULL);
	add_instruction(IC_ADD_VAR, x, y, z);
	add_instruction(IC_END_FUN, main, NULL, NULL);

	codegen();

	test_free_all();
	return EXIT_SUCCESS;
}
