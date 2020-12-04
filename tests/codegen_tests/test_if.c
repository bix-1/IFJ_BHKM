/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: test_if.c
 * Brief: Test if
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include <stdlib.h>
#include <string.h>

#include "../../symtable.h"
#include "../../ll.h"
#include "test_codegen_helper.h"

int main() {
	test_init();

	//main func
	elem_t *main = create_function("main", NULL, NULL);

	variable_t i_data = {.int_t = 5};
	elem_t *i = create_var_item("i", VAR_INT, i_data, false, false);

	variable_t j_data = {.int_t = 5};
	elem_t *j = create_var_item("j", VAR_INT, j_data, false, false);

	variable_t b1_data = {.bool_t = false};
	elem_t *b1 = create_var_item("b1", VAR_BOOL, b1_data, false, false);

	variable_t b2_data = {.bool_t = false};
	elem_t *b2 = create_var_item("b2", VAR_BOOL, b2_data, false, false);

	variable_t new_line_data = {.string_t = "\\010"};
	elem_t *new_line = create_var_item("new_line", VAR_STRING, new_line_data, true, false);

	variable_t a1_data = {.string_t = "Condition-1"};
	elem_t *a1 = create_var_item("a1", VAR_STRING, a1_data, true, false);

	variable_t a2_data = {.string_t = "Condition-2"};
	elem_t *a2 = create_var_item("a2", VAR_STRING, a2_data, true, false);

	variable_t a3_data = {.string_t = "Condition-3"};
	elem_t *a3 = create_var_item("a3", VAR_STRING, a3_data, true, false);

	elem_t *print_list = create_var_list();
	var_list_add(print_list->symbol.sym_var_list, a1->symbol.sym_var_item);
	var_list_add(print_list->symbol.sym_var_list, new_line->symbol.sym_var_item);
	elem_t *print_fu1 = create_function("printf1", print_list->symbol.sym_var_list, NULL);

	elem_t *print_list2 = create_var_list();
	var_list_add(print_list2->symbol.sym_var_list, a2->symbol.sym_var_item);
	var_list_add(print_list2->symbol.sym_var_list, new_line->symbol.sym_var_item);
	elem_t *print_fu2 = create_function("printf2", print_list2->symbol.sym_var_list, NULL);

	elem_t *print_list3 = create_var_list();
	var_list_add(print_list3->symbol.sym_var_list, a3->symbol.sym_var_item);
	var_list_add(print_list3->symbol.sym_var_list, new_line->symbol.sym_var_item);
	elem_t *print_fu3 = create_function("printf3", print_list3->symbol.sym_var_list, NULL);

	elem_t *print_new_line_list = create_var_list();
	var_list_add(print_new_line_list->symbol.sym_var_list, new_line->symbol.sym_var_item);
	elem_t *print_new_line_fu = create_function("print_new_line", print_new_line_list->symbol.sym_var_list, NULL);

	variable_t c_10_data = {.int_t = 10};
	elem_t *c_10 = create_var_item("c10", VAR_INT, c_10_data, true, false);

	variable_t c_1_data = {.int_t = 1};
	elem_t *c_1 = create_var_item("c1", VAR_INT, c_1_data, true, false);

	variable_t i_default_data = {.int_t = 10};
	elem_t *i_default = create_var_item("expr-0", VAR_INT, i_default_data, true, false);

	variable_t j_default_data = {.int_t = 5};
	elem_t *j_default = create_var_item("expr-022", VAR_INT, j_default_data, true, false);

	variable_t b_default_data = {.bool_t = false};
	elem_t *b_default = create_var_item("expr-1", VAR_BOOL, b_default_data, true, false);

	elem_t *list_def0 = create_var_list();
	elem_t *list_def1 = create_var_list();

	elem_t *list_def2 = create_var_list();
	elem_t *list_def3 = create_var_list();

	var_list_add(list_def0->symbol.sym_var_list, i->symbol.sym_var_item);
	var_list_add(list_def0->symbol.sym_var_list, b1->symbol.sym_var_item);

	var_list_add(list_def1->symbol.sym_var_list, i_default->symbol.sym_var_item);
	var_list_add(list_def1->symbol.sym_var_list, b_default->symbol.sym_var_item);

	var_list_add(list_def2->symbol.sym_var_list, j->symbol.sym_var_item);
	var_list_add(list_def2->symbol.sym_var_list, b2->symbol.sym_var_item);

	var_list_add(list_def3->symbol.sym_var_list, j_default->symbol.sym_var_item);
	var_list_add(list_def3->symbol.sym_var_list, b_default->symbol.sym_var_item);

	add_instruction(IC_DEF_FUN, main, NULL, NULL);
	add_instruction(IC_DECL_VAR, i, NULL, NULL);
	add_instruction(IC_DECL_VAR, b1, NULL, NULL);
	add_instruction(IC_DEF_VAR, list_def0, list_def1, NULL);
	add_instruction(IC_DECL_VAR, j, NULL, NULL);
	add_instruction(IC_DECL_VAR, b2, NULL, NULL);
	add_instruction(IC_DEF_VAR, list_def2, list_def3, NULL);

	add_instruction(IC_IF_DEF, NULL, NULL, NULL);
	add_instruction(IC_GT_VAR, b1, i, j);
	add_instruction(IC_IF_START, NULL, NULL, NULL);

	add_instruction(IC_IF_DEF, NULL, NULL, NULL);
	add_instruction(IC_GT_VAR, b1, i, j);
	add_instruction(IC_IF_START, NULL, NULL, NULL);

	add_instruction(IC_IF_DEF, NULL, NULL, NULL);
	add_instruction(IC_GT_VAR, b1, i, j);
	add_instruction(IC_IF_START, NULL, NULL, NULL);
	add_instruction(IC_WRITE_VAR, print_fu1, NULL, NULL);
	add_instruction(IC_IF_END, NULL, NULL, NULL);

	add_instruction(IC_WRITE_VAR, print_fu1, NULL, NULL);
	add_instruction(IC_IF_END, NULL, NULL, NULL);

	add_instruction(IC_WRITE_VAR, print_fu1, NULL, NULL);
	add_instruction(IC_IF_END, NULL, NULL, NULL);
	add_instruction(IC_ELSEIF_DEF, NULL, NULL, NULL);
	add_instruction(IC_EQ_VAR, b2, i, j);
	add_instruction(IC_ELSEIF_START, NULL, NULL, NULL);
	add_instruction(IC_WRITE_VAR, print_fu2, NULL, NULL);
	add_instruction(IC_ELSEIF_END, NULL, NULL, NULL);
	add_instruction(IC_ELSE_START, NULL, NULL, NULL);
	add_instruction(IC_WRITE_VAR, print_fu3, NULL, NULL);
	add_instruction(IC_ELSE_END, NULL, NULL, NULL);

	add_instruction(IC_END_FUN, main, NULL, NULL);

	codegen();

	test_free_all();
	return EXIT_SUCCESS;
}
