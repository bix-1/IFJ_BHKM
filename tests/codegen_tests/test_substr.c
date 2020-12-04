/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: test_substr.c
 * Brief: Test function substr
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

	elem_t *main_defs = create_var_list();

	variable_t chr_par1_data = {.string_t = "HelloWorld"};
	elem_t *chr_par1 = create_var_item("chr1par1", VAR_STRING, chr_par1_data, true, false);

	variable_t chr_par2_data = {.int_t = 5};
	elem_t *chr_par2 = create_var_item("chr1par2", VAR_INT, chr_par2_data, true, false);

	variable_t chr_par3_data = {.int_t = 40};
	elem_t *chr_par3 = create_var_item("chr1par3", VAR_INT, chr_par3_data, true, false);

	variable_t chr_ret1_data = {.string_t = "a"};
	elem_t *chr_ret1 = create_var_item("chr1", VAR_STRING, chr_ret1_data, false, false);

	variable_t chr_ret2_data = {.int_t = 0};
	elem_t *chr_ret2 = create_var_item("err", VAR_INT, chr_ret2_data, false, false);

	elem_t *params = create_var_list();
	var_list_add(params->symbol.sym_var_list, chr_par1->symbol.sym_var_item);
	var_list_add(params->symbol.sym_var_list, chr_par2->symbol.sym_var_item);
	var_list_add(params->symbol.sym_var_list, chr_par3->symbol.sym_var_item);

	elem_t *returns = create_var_list();
	var_list_add(returns->symbol.sym_var_list, chr_ret1->symbol.sym_var_item);
	var_list_add(returns->symbol.sym_var_list, chr_ret2->symbol.sym_var_item);

	elem_t *ord = create_function("substr", params->symbol.sym_var_list, returns->symbol.sym_var_list);

	var_list_add(main_defs->symbol.sym_var_list, chr_ret1->symbol.sym_var_item);
	var_list_add(main_defs->symbol.sym_var_list, chr_ret2->symbol.sym_var_item);

	// Display
	variable_t new_line_data = {.string_t = "\\010"};
	elem_t *new_line = create_var_item("new_line", VAR_STRING, new_line_data, true, false);

	variable_t print_dest_data = {.string_t = "Dest:\t"};
	elem_t *print_dest = create_var_item("print_dest", VAR_STRING, print_dest_data, true, false);

	variable_t print_err_data = {.string_t = "Error:\t"};
	elem_t *print_err = create_var_item("print_dest", VAR_STRING, print_err_data, true, false);

	elem_t *print = create_var_list();
	var_list_add(print->symbol.sym_var_list, print_dest->symbol.sym_var_item);
	var_list_add(print->symbol.sym_var_list, chr_ret1->symbol.sym_var_item);
	var_list_add(print->symbol.sym_var_list, new_line->symbol.sym_var_item);
	var_list_add(print->symbol.sym_var_list, print_err->symbol.sym_var_item);
	var_list_add(print->symbol.sym_var_list, chr_ret2->symbol.sym_var_item);
	var_list_add(print->symbol.sym_var_list, new_line->symbol.sym_var_item);

	elem_t *print_fun = create_function("Print1", print->symbol.sym_var_list, NULL);

	add_instruction(IC_DEF_FUN, main, main_defs, NULL);
	add_instruction(IC_SUBSTR_STR, ord, NULL, NULL);
	add_instruction(IC_WRITE_VAR, print_fun, NULL, NULL);
	add_instruction(IC_END_FUN, main, NULL, NULL);

	codegen();

	test_free_all();
	return EXIT_SUCCESS;
}