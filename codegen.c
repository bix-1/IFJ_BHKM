/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: codegen.c
 * Brief: Generates final output for IFJ20 interpreter
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "codegen.h"
#include "symtable.h"
#include "ll.h"
#include "codegen_stack.h"
#include "error.h"
#include "escape_format.h"

#define OUTPUT stdout
#define GF "GF"
#define LF "LF"
#define TF "TF"

#define IF_SKIP "if-skip-"
#define IF_END "if-end-"
#define FOR_COND "for-cond-"
#define FOR_STEP "for-step-"
#define FOR_BODY "for-body-"
#define FOR_END "for-end-"

int main_fun_deep = 1;      // main function is called once on start
instr_t *last_instr = NULL;
int skip_index = 0;
int end_index = 0;
int for_cond_index = 0;
int for_step_index = 0;
int for_body_index = 0;
int for_end_index = 0;

void codegen_init() {
	jmp_label_stack_init();

	fprintf(OUTPUT, ".IFJcode20\n\n");
	fprintf(OUTPUT, "JUMP main\n\n");
}

char *get_frame(sym_var_item_t *sym_var_item) {
	if (sym_var_item->is_global) {
		return GF;
	}
	else {
		return LF;
	}
}

// Compare current and next instruction and create jump if conditions are met
void try_create_jump(instr_t instr) {
	if (instr.next == NULL) {
		error(99, "codegen.c", "try_create_jump", "Unexpected end of condition");
	}

	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "try_create_jump", "NULL pointer");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;

	char *frame_dest = get_frame(sym_dest);

	if (instr.next->type == IC_IF_START || instr.next->type == IC_ELSEIF_START) {
		jmp_label_stack_push(skip_labels_top, skip_index);
		fprintf(OUTPUT, "JUMPIFNEQ %s%d %s@%s bool@true\n", IF_SKIP, skip_index, frame_dest, sym_dest->name);
		skip_index++;
	}
	else if (instr.next->type == IC_FOR_STEP) {
		jmp_label_stack_push(for_body_top, for_body_index);
		jmp_label_stack_push(for_end_top, for_end_index);
		fprintf(OUTPUT, "JUMPIFEQ %s%d %s@%s bool@true", FOR_BODY, for_body_index, frame_dest, sym_dest->name);
		fprintf(OUTPUT, "JUMPINFEQ %s%d %s@%s bool@true", FOR_END, for_end_index, frame_dest, sym_dest->name);
		for_body_index++;
		for_end_index++;
	}
}

void declr_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest->sym_type != SYM_VAR_ITEM) {
		error(99, "codegen.c", "declr_var", "Invalid symbol type");
	}

	sym_var_item_t *sym_dest = instr.elem_dest_ptr->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "declr_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);

	// Declare variable
	fprintf(OUTPUT, "DEFVAR %s@%s\n", frame_dest, sym_dest->name);

	// Set variable init value
	switch (elem_dest->symbol.sym_var_item->type) {
		case VAR_INT:
			fprintf(OUTPUT, "MOVE %s@%s int@%d\n", frame_dest, sym_dest->name, sym_dest->data.int_t);
			break;
		case VAR_FLOAT64:
			fprintf(OUTPUT, "MOVE %s@%s float64@%a\n", frame_dest, sym_dest->name, sym_dest->data.float64_t);
			break;
		case VAR_STRING: {
			if (!sym_dest->is_formatted) {
				char *new_seq = escape_reformat(sym_dest->data.string_t);
				variable_t new_data = {.string_t = new_seq};
				sym_var_item_change_data(sym_dest, new_data);
				sym_var_item_set_formatted(sym_dest, true);
			}

			fprintf(OUTPUT, "MOVE %s@%s string@%s\n", frame_dest, sym_dest->name, sym_dest->data.string_t);
			break;
		}
		case VAR_BOOL:
			if (sym_dest->data.bool_t) {
				fprintf(OUTPUT, "MOVE %s@%s bool@true\n", frame_dest, sym_dest->name);
			}
			else {
				fprintf(OUTPUT, "MOVE %s@%s bool@false\n", frame_dest, sym_dest->name);
			}
			break;
		case VAR_NIL:
			fprintf(OUTPUT, "MOVE %s@%s nil@nil\n", frame_dest, sym_dest->name);
			break;
		case VAR_UNDEFINED:
			error(99, "codegen.c", "declr_var", "Undefined data type");
			break;
		default:
			error(99, "codegen.c", "declr_var", "Wrong data type");
			break;
	}

}

void def_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "def_var", "NULL element");
	}

	elem_t *elem_1 = instr.elem1_ptr;

	if (elem_1 == NULL) {
		error(99, "codegen.c", "def_var", "NULL element");
	}

	if (elem_dest->sym_type != SYM_VAR_LIST) {
		error(99, "codegen.c", "def_var", "Unexpected symbol");
	}

	if (elem_1->sym_type != SYM_VAR_LIST) {
		error(99, "codegen.c", "def_var", "Unexpected symbol");
	}

	size_t dest_size = sym_var_list_size(elem_dest->symbol.sym_var_list);
	size_t elem1_size = sym_var_list_size(elem_1->symbol.sym_var_list);

	if (dest_size != elem1_size) {
		error(99, "codegen.c", "def_var", "Different var list sizes");
	}

	sym_var_item_t *sym_dest = sym_var_list_next(elem_dest->symbol.sym_var_list);
	sym_var_item_t *sym_elem1 = sym_var_list_next(elem_1->symbol.sym_var_list);

	while (sym_dest != NULL && sym_elem1 != NULL) {

		char *frame_dest = get_frame(sym_dest);
		char *frame_elem1 = get_frame(sym_elem1);

		if (sym_dest->type != sym_elem1->type) {
			error(99, "codegen.c", "def_var", "Incompatible data types");
		}

		if (sym_dest->is_const) {
			switch (sym_dest->type) {
				case VAR_INT:
					fprintf(OUTPUT, "MOVE %s@%s int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.int_t);
					break;
				case VAR_FLOAT64:
					fprintf(OUTPUT, "MOVE %s@%s float@%a\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t);
					break;
				case VAR_STRING:
					fprintf(OUTPUT, "MOVE %s@%s string@%s\n", frame_dest, sym_dest->name, sym_elem1->data.string_t);
					break;
				case VAR_BOOL:
					if (sym_elem1->data.bool_t) {
						fprintf(OUTPUT, "MOVE %s@%s bool@true\n", frame_dest, sym_dest->name);
					}
					else {
						fprintf(OUTPUT, "MOVE %s@%s bool@false\n", frame_dest, sym_dest->name);
					}
					break;
				case VAR_NIL:
					fprintf(OUTPUT, "MOVE %s@%s nil@nil\n", frame_dest, sym_dest->name);
					break;
				case VAR_UNDEFINED:
					error(99, "codegen.c", "def_var", "Undefined data type");
					break;
				default:
					error(99, "codegen.c", "def_var", "Wrong data type");
					break;
			}
		}
		else {
			fprintf(OUTPUT, "MOVE %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name);
		}
	}

}

void def_fun(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "def_fun", "NULL element");
	}

	if (elem_dest->sym_type != SYM_FUNC) {
		error(99, "codegen.c", "def_fun", "Invalid symbol");
	}

	sym_func_t *sym_func = elem_dest->symbol.sym_func;

	if (sym_func == NULL) {
		error(99, "codegen.c", "def_fun", "NULL element");
	}

	fprintf(OUTPUT, "LABEL %s\n", sym_func->name);
	fprintf(OUTPUT, "CREATEFRAME\n");

	sym_var_list_t *params = sym_func->params;

	if (params != NULL) {
		sym_var_item_t *param_active = sym_var_list_prev(params);

		while (param_active != NULL) {
			char *frame = TF;
			fprintf(OUTPUT, "DEFVAR %s@%s\n", frame, param_active->name);
			fprintf(OUTPUT, "POPS %s@%s\n", frame, param_active->name);
			param_active = sym_var_list_prev(params);
		}
	}

	fprintf(OUTPUT, "PUSHFRAME\n");
}

void call_fun(instr_t instr) {
	elem_t *elem1 = instr.elem_dest_ptr;

	if (elem1 == NULL) {
		error(99, "codegen.c", "call_fun", "NULL element");
	}

	if (elem1->sym_type != SYM_FUNC) {
		error(99, "codegen.c", "call_fun", "Invalid symbol, not func");
	}

	sym_func_t *sym_func = elem1->symbol.sym_func;

	if (sym_func == NULL) {
		error(99, "codegen.c", "call_fun", "NULL element");
	}

	sym_var_list_t *params = sym_func->params;

	if (params != NULL) {
		sym_var_item_t *param_active = sym_var_list_next(params);

		while (param_active != NULL) {
			char *frame = get_frame(param_active);
			if (param_active->is_const) {
				switch (param_active->type) {
					case VAR_INT:
						fprintf(OUTPUT, "PUSHS int@%d\n", param_active->data.int_t);
						break;
					case VAR_FLOAT64:
						fprintf(OUTPUT, "PUSHS float@%a\n", param_active->data.float64_t);
						break;
					case VAR_STRING:
						fprintf(OUTPUT, "PUSHS string@%s\n", param_active->data.string_t);
						break;
					case VAR_BOOL:
						if (param_active->data.bool_t) {
							fprintf(OUTPUT, "PUSHS bool@true\n");
						}
						else {
							fprintf(OUTPUT, "PUSHS bool@false\n");
						}
						break;
					case VAR_NIL:
						fprintf(OUTPUT, "PUSHS nil@nil\n");
						break;
					case VAR_UNDEFINED:
						error(99, "codegen.c", "call_fun", "Undefined data type");
						break;
					default:
						error(99, "codegen.c", "call_fun", "Wrong data type");
						break;
				}
			}
			else {
				fprintf(OUTPUT, "PUSHS %s@%s\n", frame, param_active->name);
			}
			param_active = sym_var_list_next(params);
		}
	}

	fprintf(OUTPUT, "CALL %s\n", sym_func->name);

	sym_var_list_t *returns = sym_func->returns;

	if (returns != NULL) {
		sym_var_item_t *return_active = sym_var_list_prev(returns);

		while (return_active != NULL) {
			char *frame = get_frame(return_active);
			fprintf(OUTPUT, "POPS %s@%s\n", frame, return_active->name);
			return_active = sym_var_list_prev(returns);
		}

		/*while (dest != NULL && return_active != NULL) {
			if (dest->type != return_active->type) {
				error(99, "codegen.c", "call_var", "Different data type");
			}
			char *frame_dest = LF;
			char *frame_input = TF;
			if (return_active->is_const) {
				switch (return_active->type) {
					case VAR_INT:
						fprintf(OUTPUT, "MOVE %s@%s int@%d\n", frame_dest, dest->name,
						        return_active->data.int_t);
						break;
					case VAR_FLOAT64:
						fprintf(OUTPUT, "MOVE %s@%s float@%a\n", frame_dest, dest->name,
						        return_active->data.float64_t);
						break;
					case VAR_STRING:
						fprintf(OUTPUT, "MOVE %s@%s string@%s\n", frame_dest, dest->name,
						        return_active->data.string_t);
						break;
					case VAR_BOOL:
						if (return_active->data.bool_t) {
							fprintf(OUTPUT, "MOVE %s@%s bool@true\n", frame_dest, dest->name);
						}
						else {
							fprintf(OUTPUT, "MOVE %s@%s bool@false\n", frame_dest, dest->name);
						}
						break;
					case VAR_NIL:
						fprintf(OUTPUT, "MOVE %s@%s nil@nil\n", frame_dest, dest->name);
						break;
					case VAR_UNDEFINED:
						error(99, "codegen.c", "call_fun", "Undefined data type");
						break;
					default:
						error(99, "codegen.c", "call_fun", "Wrong data type");
						break;
				}
			}
			else {
				fprintf(OUTPUT, "MOVE %s@%s %s@%s\n", frame_dest, dest->name, frame_input, return_active->name);
			}
			return_active = sym_var_list_next(returns);
		}*/
	}

	if (strcmp(elem1->symbol.sym_func->name, "main") == 0) {
		// main function
		main_fun_deep++;
	}
}

void ret_fun(instr_t instr) {
	// Same function just return and end
	if (last_instr != NULL && last_instr->type == IC_RET_FUN && instr.type == IC_END_FUN &&
	    *(last_instr->elem_dest_ptr->key) == *(instr.elem_dest_ptr->key)) {
		return;
	}

	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "ret_fun", "NULL element");
	}

	if (elem_dest->sym_type != SYM_FUNC) {
		error(99, "codegen.c", "ret_fun", "Invalid symbol");
	}

	sym_var_list_t *returns = elem_dest->symbol.sym_func->returns;
	if (returns != NULL) {
		sym_var_item_t *return_active = sym_var_list_next(returns);

		while (return_active != NULL) {
			char *frame = get_frame(return_active);
			fprintf(OUTPUT, "PUSHS %s@%s\n", frame, return_active->name);
			return_active = sym_var_list_next(returns);
		}
	}

	fprintf(OUTPUT, "POPFRAME\n");

	// check if function is main, if yes then EXIT may be called
	// check main_fun_deep (if its not 1, then EXIT instr cannot be called)
	if (strcmp(elem_dest->symbol.sym_func->name, "main") == 0) {
		// main function
		if (main_fun_deep == 1) {
			fprintf(OUTPUT, "EXIT int@0\n\n");
		}
		else {
			fprintf(OUTPUT, "RETURN\n\n");
		}
		main_fun_deep--;
	}
	else {
		fprintf(OUTPUT, "RETURN\n\n");
	}
}

void add_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "add_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "add_var", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "add_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "add_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "add_var", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "add_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	var_type_t type = -1;

	if (sym_dest->type == VAR_FLOAT64 && sym_elem1->type == VAR_FLOAT64 && sym_elem2->type == VAR_FLOAT64) {
		type = VAR_FLOAT64;
	}
	else if (sym_dest->type == VAR_INT && sym_elem1->type == VAR_INT && sym_elem2->type == VAR_INT) {
		type = VAR_INT;
	}
	else {
		error(99, "codegen.c", "add_var", "Incompatible data types");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "ADD %s@%s int@%d int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.int_t,
			        sym_elem2->data.int_t);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "ADD %s@%s float@%a float@%a\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t,
			        sym_elem2->data.float64_t);
		}
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "ADD %s@%s int@%d %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.int_t,
			        frame_elem2, sym_elem2->name);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "ADD %s@%s float@%a %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t,
			        frame_elem2, sym_elem2->name);
		}
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "ADD %s@%s %s@%s int@%d\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        sym_elem2->data.int_t);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "ADD %s@%s %s@%s float@%a\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        sym_elem2->data.float64_t);
		}
	}
	else {
		fprintf(OUTPUT, "ADD %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}
}

void sub_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "sub_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "sub_var", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "sub_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "sub_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "sub_var", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "sub_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	var_type_t type = -1;

	if (sym_dest->type == VAR_FLOAT64 && sym_elem1->type == VAR_FLOAT64 && sym_elem2->type == VAR_FLOAT64) {
		type = VAR_FLOAT64;
	}
	else if (sym_dest->type == VAR_INT && sym_elem1->type == VAR_INT && sym_elem2->type == VAR_INT) {
		type = VAR_INT;
	}
	else {
		error(99, "codegen.c", "sub_var", "Incompatible data types");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "SUB %s@%s int@%d int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.int_t,
			        sym_elem2->data.int_t);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "SUB %s@%s float@%a float@%a\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t,
			        sym_elem2->data.float64_t);
		}
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "SUB %s@%s int@%d %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.int_t,
			        frame_elem2, sym_elem2->name);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "SUB %s@%s float@%a %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t,
			        frame_elem2, sym_elem2->name);
		}
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "SUB %s@%s %s@%s int@%d\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        sym_elem2->data.int_t);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "SUB %s@%s %s@%s float@%a\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        sym_elem2->data.float64_t);
		}
	}
	else {
		fprintf(OUTPUT, "SUB %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}
}

void mul_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "mul_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "mul_var", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "mul_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "mul_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "mul_var", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "mul_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	var_type_t type = -1;

	if (sym_dest->type == VAR_FLOAT64 && sym_elem1->type == VAR_FLOAT64 && sym_elem2->type == VAR_FLOAT64) {
		type = VAR_FLOAT64;
	}
	else if (sym_dest->type == VAR_INT && sym_elem1->type == VAR_INT && sym_elem2->type == VAR_INT) {
		type = VAR_INT;
	}
	else {
		error(99, "codegen.c", "mul_var", "Incompatible data types");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "MUL %s@%s int@%d int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.int_t,
			        sym_elem2->data.int_t);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "MUL %s@%s float@%a float@%a\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t,
			        sym_elem2->data.float64_t);
		}
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "MUL %s@%s int@%d %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.int_t,
			        frame_elem2, sym_elem2->name);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "MUL %s@%s float@%a %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t,
			        frame_elem2, sym_elem2->name);
		}
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "MUL %s@%s %s@%s int@%d\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        sym_elem2->data.int_t);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "MUL %s@%s %s@%s float@%a\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        sym_elem2->data.float64_t);
		}
	}
	else {
		fprintf(OUTPUT, "MUL %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}
}

void div_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "div_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "div_var", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "div_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "div_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "div_var", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "div_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	var_type_t type = -1;

	if (sym_dest->type == VAR_FLOAT64 && sym_elem1->type == VAR_FLOAT64 && sym_elem2->type == VAR_FLOAT64) {
		type = VAR_FLOAT64;
	}
	else if (sym_dest->type == VAR_INT && sym_elem1->type == VAR_INT && sym_elem2->type == VAR_INT) {
		type = VAR_INT;
	}
	else {
		error(99, "codegen.c", "div_var", "Incompatible data types");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "IDIV %s@%s int@%d int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.int_t,
			        sym_elem2->data.int_t);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "DIV %s@%s float@%a float@%a\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t,
			        sym_elem2->data.float64_t);
		}
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "IDIV %s@%s int@%d %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.int_t,
			        frame_elem2, sym_elem2->name);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "DIV %s@%s float@%a %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t,
			        frame_elem2, sym_elem2->name);
		}
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "IDIV %s@%s %s@%s int@%d\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        sym_elem2->data.int_t);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "DIV %s@%s %s@%s float@%a\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        sym_elem2->data.float64_t);
		}
	}
	else {
		if (type == VAR_INT) {
			fprintf(OUTPUT, "IDIV %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        frame_elem2, sym_elem2->name);
		}
		else if (type == VAR_FLOAT64) {
			fprintf(OUTPUT, "DIV %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
			        frame_elem2, sym_elem2->name);
		}
	}
}

void lt_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "lt_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "lt_var", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "lt_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "lt_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "lt_var", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "lt_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	if (sym_dest->type != VAR_BOOL) {
		error(99, "codegen.c", "lt_var", "Incompatible data type");
	}

	var_type_t type = -1;

	if (sym_elem1->type == VAR_INT && sym_elem2->type == VAR_INT) {
		type = VAR_INT;
	}
	else if (sym_elem1->type == VAR_FLOAT64 && sym_elem2->type == VAR_FLOAT64) {
		type = VAR_FLOAT64;
	}
	else if (sym_elem1->type == VAR_BOOL && sym_elem2->type == VAR_BOOL) {
		type = VAR_BOOL;
	}
	else if (sym_elem1->type == VAR_STRING && sym_elem2->type == VAR_STRING) {
		type = VAR_STRING;
	}
	else {
		error(99, "codegen.c", "lt_var", "Incompatible data types");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		switch (type) {
			case VAR_INT:
				fprintf(OUTPUT, "LT %s@%s int@%d int@%d\n", frame_dest, sym_dest->name,
				        sym_elem1->data.int_t, sym_elem2->data.int_t);
				break;
			case VAR_FLOAT64:
				fprintf(OUTPUT, "LT %s@%s float@%a float@%a\n", frame_dest, sym_dest->name,
				        sym_elem1->data.float64_t, sym_elem2->data.float64_t);
				break;
			case VAR_STRING:
				fprintf(OUTPUT, "LT %s@%s string@%s string@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.string_t, sym_elem2->data.string_t);
				break;
			case VAR_BOOL:
				if (sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "LT %s@%s bool@true bool@true\n", frame_dest, sym_dest->name);
				}
				else if (sym_elem1->data.bool_t && !sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "LT %s@%s bool@true bool@false\n", frame_dest, sym_dest->name);
				}
				else if (!sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "LT %s@%s bool@false bool@true\n", frame_dest, sym_dest->name);
				}
				else {
					// both false
					fprintf(OUTPUT, "LT %s@%s bool@false bool@false\n", frame_dest, sym_dest->name);
				}
				break;
		}
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		switch (type) {
			case VAR_INT:
				fprintf(OUTPUT, "LT %s@%s int@%d %s@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.int_t, frame_elem2, sym_elem2->name);
				break;
			case VAR_FLOAT64:
				fprintf(OUTPUT, "LT %s@%s float@%a %s@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.float64_t, frame_elem2, sym_elem2->name);
				break;
			case VAR_STRING:
				fprintf(OUTPUT, "LT %s@%s string@%s %s@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.string_t, frame_elem2, sym_elem2->name);
				break;
			case VAR_BOOL:
				if (sym_elem1->data.bool_t) {
					fprintf(OUTPUT, "LT %s@%s bool@true %s@%s\n", frame_dest, sym_dest->name,
					        frame_elem2, sym_elem2->name);
				}
				else {
					fprintf(OUTPUT, "LT %s@%s bool@false %s@%s\n", frame_dest, sym_dest->name,
					        frame_elem2, sym_elem2->name);
				}
				break;
		}
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		switch (type) {
			case VAR_INT:
				fprintf(OUTPUT, "LT %s@%s %s@%s int@%d\n", frame_dest, sym_dest->name,
				        frame_elem1, sym_elem1->name, sym_elem2->data.int_t);
				break;
			case VAR_FLOAT64:
				fprintf(OUTPUT, "LT %s@%s %s@%s float@%a\n", frame_dest, sym_dest->name,
				        frame_elem1, sym_elem1->name, sym_elem2->data.float64_t);
				break;
			case VAR_STRING:
				fprintf(OUTPUT, "LT %s@%s %s@%s string@%s\n", frame_dest, sym_dest->name,
				        frame_elem1, sym_elem1->name, sym_elem2->data.string_t);
				break;
			case VAR_BOOL:
				if (sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "LT %s@%s %s@%s bool@true\n", frame_dest, sym_dest->name,
					        frame_elem1, sym_elem1->name);
				}
				else {
					fprintf(OUTPUT, "LT %s@%s %s@%s bool@true\n", frame_dest, sym_dest->name,
					        frame_elem1, sym_elem1->name);
				}
				break;
		}
	}
	else {
		fprintf(OUTPUT, "LT %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}

	try_create_jump(instr);
}

void gt_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "gt_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "gt_var", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "gt_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "gt_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "gt_var", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "gt_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	if (sym_dest->type != VAR_BOOL) {
		error(99, "codegen.c", "gt_var", "Incompatible data type");
	}

	var_type_t type = -1;

	if (sym_elem1->type == VAR_INT && sym_elem2->type == VAR_INT) {
		type = VAR_INT;
	}
	else if (sym_elem1->type == VAR_FLOAT64 && sym_elem2->type == VAR_FLOAT64) {
		type = VAR_FLOAT64;
	}
	else if (sym_elem1->type == VAR_BOOL && sym_elem2->type == VAR_BOOL) {
		type = VAR_BOOL;
	}
	else if (sym_elem1->type == VAR_STRING && sym_elem2->type == VAR_STRING) {
		type = VAR_STRING;
	}
	else {
		error(99, "codegen.c", "gt_var", "Incompatible data types");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		switch (type) {
			case VAR_INT:
				fprintf(OUTPUT, "GT %s@%s int@%d int@%d\n", frame_dest, sym_dest->name,
				        sym_elem1->data.int_t, sym_elem2->data.int_t);
				break;
			case VAR_FLOAT64:
				fprintf(OUTPUT, "GT %s@%s float@%a float@%a\n", frame_dest, sym_dest->name,
				        sym_elem1->data.float64_t, sym_elem2->data.float64_t);
				break;
			case VAR_STRING:
				fprintf(OUTPUT, "GT %s@%s string@%s string@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.string_t, sym_elem2->data.string_t);
				break;
			case VAR_BOOL:
				if (sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "GT %s@%s bool@true bool@true\n", frame_dest, sym_dest->name);
				}
				else if (sym_elem1->data.bool_t && !sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "GT %s@%s bool@true bool@false\n", frame_dest, sym_dest->name);
				}
				else if (!sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "GT %s@%s bool@false bool@true\n", frame_dest, sym_dest->name);
				}
				else {
					// both false
					fprintf(OUTPUT, "GT %s@%s bool@false bool@false\n", frame_dest, sym_dest->name);
				}
				break;
		}
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		switch (type) {
			case VAR_INT:
				fprintf(OUTPUT, "GT %s@%s int@%d %s@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.int_t, frame_elem2, sym_elem2->name);
				break;
			case VAR_FLOAT64:
				fprintf(OUTPUT, "GT %s@%s float@%a %s@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.float64_t, frame_elem2, sym_elem2->name);
				break;
			case VAR_STRING:
				fprintf(OUTPUT, "GT %s@%s string@%s %s@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.string_t, frame_elem2, sym_elem2->name);
				break;
			case VAR_BOOL:
				if (sym_elem1->data.bool_t) {
					fprintf(OUTPUT, "GT %s@%s bool@true %s@%s\n", frame_dest, sym_dest->name,
					        frame_elem2, sym_elem2->name);
				}
				else {
					fprintf(OUTPUT, "GT %s@%s bool@false %s@%s\n", frame_dest, sym_dest->name,
					        frame_elem2, sym_elem2->name);
				}
				break;
		}
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		switch (type) {
			case VAR_INT:
				fprintf(OUTPUT, "GT %s@%s %s@%s int@%d\n", frame_dest, sym_dest->name,
				        frame_elem1, sym_elem1->name, sym_elem2->data.int_t);
				break;
			case VAR_FLOAT64:
				fprintf(OUTPUT, "GT %s@%s %s@%s float@%a\n", frame_dest, sym_dest->name,
				        frame_elem1, sym_elem1->name, sym_elem2->data.float64_t);
				break;
			case VAR_STRING:
				fprintf(OUTPUT, "GT %s@%s %s@%s string@%s\n", frame_dest, sym_dest->name,
				        frame_elem1, sym_elem1->name, sym_elem2->data.string_t);
				break;
			case VAR_BOOL:
				if (sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "GT %s@%s %s@%s bool@true\n", frame_dest, sym_dest->name,
					        frame_elem1, sym_elem1->name);
				}
				else {
					fprintf(OUTPUT, "GT %s@%s %s@%s bool@true\n", frame_dest, sym_dest->name,
					        frame_elem1, sym_elem1->name);
				}
				break;
		}
	}
	else {
		fprintf(OUTPUT, "GT %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}

	try_create_jump(instr);
}

void eq_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "eq_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "eq_var", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "eq_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "eq_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "eq_var", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "eq_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	if (sym_dest->type != VAR_BOOL) {
		error(99, "codegen.c", "eq_var", "Incompatible data type");
	}

	var_type_t type = -1;

	if (sym_elem1->type == VAR_INT && sym_elem2->type == VAR_INT) {
		type = VAR_INT;
	}
	else if (sym_elem1->type == VAR_FLOAT64 && sym_elem2->type == VAR_FLOAT64) {
		type = VAR_FLOAT64;
	}
	else if (sym_elem1->type == VAR_BOOL && sym_elem2->type == VAR_BOOL) {
		type = VAR_BOOL;
	}
	else if (sym_elem1->type == VAR_STRING && sym_elem2->type == VAR_STRING) {
		type = VAR_STRING;
	}
	else {
		error(99, "codegen.c", "eq_var", "Incompatible data types");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		switch (type) {
			case VAR_INT:
				fprintf(OUTPUT, "EQ %s@%s int@%d int@%d\n", frame_dest, sym_dest->name,
				        sym_elem1->data.int_t, sym_elem2->data.int_t);
				break;
			case VAR_FLOAT64:
				fprintf(OUTPUT, "EQ %s@%s float@%a float@%a\n", frame_dest, sym_dest->name,
				        sym_elem1->data.float64_t, sym_elem2->data.float64_t);
				break;
			case VAR_STRING:
				fprintf(OUTPUT, "EQ %s@%s string@%s string@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.string_t, sym_elem2->data.string_t);
				break;
			case VAR_BOOL:
				if (sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "EQ %s@%s bool@true bool@true\n", frame_dest, sym_dest->name);
				}
				else if (sym_elem1->data.bool_t && !sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "EQ %s@%s bool@true bool@false\n", frame_dest, sym_dest->name);
				}
				else if (!sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "EQ %s@%s bool@false bool@true\n", frame_dest, sym_dest->name);
				}
				else {
					// both false
					fprintf(OUTPUT, "EQ %s@%s bool@false bool@false\n", frame_dest, sym_dest->name);
				}
				break;
		}
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		switch (type) {
			case VAR_INT:
				fprintf(OUTPUT, "EQ %s@%s int@%d %s@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.int_t, frame_elem2, sym_elem2->name);
				break;
			case VAR_FLOAT64:
				fprintf(OUTPUT, "EQ %s@%s float@%a %s@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.float64_t, frame_elem2, sym_elem2->name);
				break;
			case VAR_STRING:
				fprintf(OUTPUT, "EQ %s@%s string@%s %s@%s\n", frame_dest, sym_dest->name,
				        sym_elem1->data.string_t, frame_elem2, sym_elem2->name);
				break;
			case VAR_BOOL:
				if (sym_elem1->data.bool_t) {
					fprintf(OUTPUT, "EQ %s@%s bool@true %s@%s\n", frame_dest, sym_dest->name,
					        frame_elem2, sym_elem2->name);
				}
				else {
					fprintf(OUTPUT, "EQ %s@%s bool@false %s@%s\n", frame_dest, sym_dest->name,
					        frame_elem2, sym_elem2->name);
				}
				break;
		}
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		switch (type) {
			case VAR_INT:
				fprintf(OUTPUT, "EQ %s@%s %s@%s int@%d\n", frame_dest, sym_dest->name,
				        frame_elem1, sym_elem1->name, sym_elem2->data.int_t);
				break;
			case VAR_FLOAT64:
				fprintf(OUTPUT, "EQ %s@%s %s@%s float@%a\n", frame_dest, sym_dest->name,
				        frame_elem1, sym_elem1->name, sym_elem2->data.float64_t);
				break;
			case VAR_STRING:
				fprintf(OUTPUT, "EQ %s@%s %s@%s string@%s\n", frame_dest, sym_dest->name,
				        frame_elem1, sym_elem1->name, sym_elem2->data.string_t);
				break;
			case VAR_BOOL:
				if (sym_elem2->data.bool_t) {
					fprintf(OUTPUT, "EQ %s@%s %s@%s bool@true\n", frame_dest, sym_dest->name,
					        frame_elem1, sym_elem1->name);
				}
				else {
					fprintf(OUTPUT, "EQ %s@%s %s@%s bool@true\n", frame_dest, sym_dest->name,
					        frame_elem1, sym_elem1->name);
				}
				break;
		}
	}
	else {
		fprintf(OUTPUT, "EQ %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}

	try_create_jump(instr);
}

void and_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "and_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "and_var", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "and_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "and_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "and_var", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "and_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	if (sym_dest->type != VAR_BOOL || sym_elem1->type != VAR_BOOL || sym_elem2->type != VAR_BOOL) {
		error(99, "codegen.c", "and_var", "Incompatible data type");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		if (sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
			fprintf(OUTPUT, "AND %s@%s bool@true bool@true\n", frame_dest, sym_dest->name);
		}
		else if (sym_elem1->data.bool_t && !sym_elem2->data.bool_t) {
			fprintf(OUTPUT, "AND %s@%s bool@true bool@false\n", frame_dest, sym_dest->name);
		}
		else if (!sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
			fprintf(OUTPUT, "AND %s@%s bool@false bool@true\n", frame_dest, sym_dest->name);
		}
		else {
			fprintf(OUTPUT, "AND %s@%s bool@false bool@false\n", frame_dest, sym_dest->name);
		}
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		if (sym_elem1->data.bool_t) {
			fprintf(OUTPUT, "AND %s@%s bool@true %s@%s\n", frame_dest, sym_dest->name,
			        frame_elem2, sym_elem2->name);
		}
		else {
			fprintf(OUTPUT, "AND %s@%s bool@false %s@%s\n", frame_dest, sym_dest->name,
			        frame_elem2, sym_elem2->name);
		}
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		if (sym_elem2->data.bool_t) {
			fprintf(OUTPUT, "AND %s@%s %s@%s bool@true\n", frame_dest, sym_dest->name,
			        frame_elem1, sym_elem1->name);
		}
		else {
			fprintf(OUTPUT, "AND %s@%s %s@%s bool@false\n", frame_dest, sym_dest->name,
			        frame_elem1, sym_elem1->name);
		}
	}
	else {
		fprintf(OUTPUT, "AND %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}

	try_create_jump(instr);
}

void or_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "or_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "or_var", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "or_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "or_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "or_var", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "or_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	if (sym_dest->type != VAR_BOOL || sym_elem1->type != VAR_BOOL || sym_elem2->type != VAR_BOOL) {
		error(99, "codegen.c", "or_var", "Incompatible data type");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		if (sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
			fprintf(OUTPUT, "OR %s@%s bool@true bool@true\n", frame_dest, sym_dest->name);
		}
		else if (sym_elem1->data.bool_t && !sym_elem2->data.bool_t) {
			fprintf(OUTPUT, "OR %s@%s bool@true bool@false\n", frame_dest, sym_dest->name);
		}
		else if (!sym_elem1->data.bool_t && sym_elem2->data.bool_t) {
			fprintf(OUTPUT, "OR %s@%s bool@false bool@true\n", frame_dest, sym_dest->name);
		}
		else {
			fprintf(OUTPUT, "OR %s@%s bool@false bool@false\n", frame_dest, sym_dest->name);
		}
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		if (sym_elem1->data.bool_t) {
			fprintf(OUTPUT, "OR %s@%s bool@true %s@%s\n", frame_dest, sym_dest->name,
			        frame_elem2, sym_elem2->name);
		}
		else {
			fprintf(OUTPUT, "OR %s@%s bool@false %s@%s\n", frame_dest, sym_dest->name,
			        frame_elem2, sym_elem2->name);
		}
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		if (sym_elem2->data.bool_t) {
			fprintf(OUTPUT, "OR %s@%s %s@%s bool@true\n", frame_dest, sym_dest->name,
			        frame_elem1, sym_elem1->name);
		}
		else {
			fprintf(OUTPUT, "OR %s@%s %s@%s bool@false\n", frame_dest, sym_dest->name,
			        frame_elem1, sym_elem1->name);
		}
	}
	else {
		fprintf(OUTPUT, "OR %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}

	try_create_jump(instr);
}

void not_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "not_var", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "not_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "not_var", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "not_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);

	if (sym_dest->type != VAR_BOOL || sym_elem1->type != VAR_BOOL) {
		error(99, "codegen.c", "not_var", "Incompatible data type");
	}

	if (sym_elem1->is_const) {
		if (sym_elem1->data.bool_t) {
			fprintf(OUTPUT, "NOT %s@%s bool@true\n", frame_dest, sym_dest->name);
		}
		else {
			fprintf(OUTPUT, "NOT %s@%s bool@false\n", frame_dest, sym_dest->name);
		}
	}
	else {
		fprintf(OUTPUT, "NOT %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name);
	}

	try_create_jump(instr);
}

void int2float(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "int2float", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_elem1 = elem_dest->symbol.sym_func->params->first->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "int2float", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "int2float", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);

	if (sym_dest->type != VAR_FLOAT64) {
		error(99, "codegen.c", "int2float", "Incompatible data type");
	}

	if (sym_elem1->type != VAR_INT) {
		error(99, "codegen.c", "int2float", "Incompatible data type");
	}

	if (sym_elem1->is_const) {
		fprintf(OUTPUT, "INT2FLOAT %s@%s int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.int_t);
	}
	else {
		fprintf(OUTPUT, "INT2FLOAT %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name);
	}
}

void float2int(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "float2int", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_elem1 = elem_dest->symbol.sym_func->params->first->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "float2int", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "float2int", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);

	if (sym_dest->type != VAR_INT) {
		error(99, "codegen.c", "float2int", "Incompatible data type");
	}

	if (sym_elem1->type != VAR_FLOAT64) {
		error(99, "codegen.c", "float2int", "Incompatible data type");
	}

	if (sym_elem1->is_const) {
		fprintf(OUTPUT, "FLOAT2INT %s@%s float@%a\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t);
	}
	else {
		fprintf(OUTPUT, "FLOAT2INT %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name);
	}
}

void int2char(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "int2char", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_elem1 = elem_dest->symbol.sym_func->params->first->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "int2char", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "int2char", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);

	if (sym_dest->type != VAR_STRING) {
		error(99, "codegen.c", "int2char", "Incompatible data type");
	}

	if (sym_elem1->type != VAR_INT) {
		error(99, "codegen.c", "int2char", "Incompatible data type");
	}

	if (sym_elem1->is_const) {
		fprintf(OUTPUT, "INT2CHAR %s@%s int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.int_t);
	}
	else {
		fprintf(OUTPUT, "INT2CHAR %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name);
	}
}

void str2int(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "str2int", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_err = elem_dest->symbol.sym_func->returns->first->next->item;
	sym_var_item_t *sym_elem1 = elem_dest->symbol.sym_func->params->first->item;
	sym_var_item_t *sym_elem2 = elem_dest->symbol.sym_func->params->first->next->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "str2int", "NULL symbol");
	}

	if (sym_err == NULL) {
		error(99, "codegen.c", "str2int", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "str2int", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "str2int", "NULL symbol");
	}

	// TODO : create new scope to check sym_err error
	// ...
	// else valid :

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	if (sym_dest->type != VAR_INT) {
		error(99, "codegen.c", "str2int", "Incompatible data type");
	}

	if (sym_elem1->type != VAR_STRING) {
		error(99, "codegen.c", "str2int", "Incompatible data type");
	}

	if (sym_elem2->type != VAR_INT) {
		error(99, "codegen.c", "str2int", "Incompatible data type");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		fprintf(OUTPUT, "STR2INT %s@%s string@%s int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.string_t,
		        sym_elem2->data.int_t);
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		fprintf(OUTPUT, "STR2INT %s@%s string@%s %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.string_t,
		        frame_elem2, sym_elem2->name);
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		fprintf(OUTPUT, "STR2INT %s@%s %s@%s int@%d\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		  sym_elem2->data.int_t);
	}
	else {
		fprintf(OUTPUT, "STR2INT %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}
}

void read_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "read_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "read_var", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);

	if (sym_dest->type == VAR_INT) {
		fprintf(OUTPUT, "READ %s@%s int\n", frame_dest, sym_dest->name);
	}
	else if (sym_dest->type == VAR_FLOAT64) {
		fprintf(OUTPUT, "READ %s@%s float\n", frame_dest, sym_dest->name);
	}
	else if (sym_dest->type == VAR_STRING) {
		fprintf(OUTPUT, "READ %s@%s string\n", frame_dest, sym_dest->name);
	}
	else if (sym_dest->type == VAR_BOOL) {
		fprintf(OUTPUT, "READ %s@%s bool\n", frame_dest, sym_dest->name);
	}
	else {
		error(99, "codegen.c", "read_var", "Invalid data type");
	}
}

void write_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "write_var", "NULL element");
	}

	if (elem_dest->sym_type != SYM_FUNC) {
		error(99, "codegen.c", "write_var", "Invalid symbol");
	}

	sym_var_list_t *sym_var_list = instr.elem_dest_ptr->symbol.sym_func->params;

	if (sym_var_list == NULL) {
		error(99, "codegen.c", "write_var", "Invalid symbol");
	}

	sym_var_item_t *active = sym_var_list_next(sym_var_list);
	char *frame = NULL;

	while (active != NULL) {
		frame = get_frame(active);

		if (active->is_const) {
			switch (active->type) {
				case VAR_INT:
					fprintf(OUTPUT, "WRITE int@%d\n", active->data.int_t);
					break;
				case VAR_FLOAT64:
					fprintf(OUTPUT, "WRITE float@%a\n", active->data.float64_t);
					break;
				case VAR_STRING:
					fprintf(OUTPUT, "WRITE string@%s\n", active->data.string_t);
					break;
				case VAR_BOOL:
					if (active->data.bool_t) {
						fprintf(OUTPUT, "WRITE bool@true\n");
					}
					else {
						fprintf(OUTPUT, "WRITE bool@false\n");
					}
					break;
				case VAR_NIL:
					fprintf(OUTPUT, "WRITE nil@nil\n");
					break;
				case VAR_UNDEFINED:
					error(99, "codegen.c", "write_var", "Invalid data type");
					break;
				default:
					error(99, "codegen.c", "write_var", "Wrong data type");
					break;
			}
		}
		else {
			fprintf(OUTPUT, "WRITE %s@%s\n", frame, active->name);
		}

		active = sym_var_list_next(sym_var_list);
	}
}

void concat_str(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "concat_str", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_elem1 = elem_dest->symbol.sym_func->params->first->item;
	sym_var_item_t *sym_elem2 = elem_dest->symbol.sym_func->params->first->next->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "concat_str", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "concat_str", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "concat_str", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	if (sym_dest->type != VAR_STRING || sym_elem1->type != VAR_STRING || sym_elem2->type != VAR_STRING) {
		error(99, "codegen.c", "concat_str", "Incompatible data type");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		fprintf(OUTPUT, "CONCAT %s@%s string@%s string@%s\n", frame_dest, sym_dest->name, sym_elem1->data.string_t,
		        sym_elem2->data.string_t);
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		fprintf(OUTPUT, "CONCAT %s@%s string@%s %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.string_t,
		        frame_elem2, sym_elem2->name);
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		fprintf(OUTPUT, "CONCAT %s@%s %s@%s string@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        sym_elem2->data.string_t);
	}
	else {
		fprintf(OUTPUT, "CONCAT %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}
}

void strlen_str(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "strlen_str", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_elem1 = elem_dest->symbol.sym_func->params->first->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "strlen_str", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "strlen_str", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);

	if (sym_dest->type != VAR_INT || sym_elem1->type != VAR_STRING) {
		error(99, "codegen.c", "strlen_str", "Incompatible data type");
	}

	if (sym_elem1->is_const) {
		fprintf(OUTPUT, "STRLEN %s@%s string@%s\n", frame_dest, sym_dest->name, sym_elem1->data.string_t);
	}
	else {
		fprintf(OUTPUT, "STRLEN %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name);
	}
}

void getchar_str(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "getchar_str", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_err = elem_dest->symbol.sym_func->returns->first->next->item;
	sym_var_item_t *sym_elem1 = elem_dest->symbol.sym_func->params->first->item;
	sym_var_item_t *sym_elem2 = elem_dest->symbol.sym_func->params->first->next->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "getchar_str", "NULL symbol");
	}

	if (sym_err == NULL) {
		error(99, "codegen.c", "getchar_str", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "getchar_str", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "getchar_str", "NULL symbol");
	}

	// TODO : create new scope to check sym_err error
	// ...
	// valid:

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	if (sym_dest->type != VAR_STRING || sym_elem1->type != VAR_STRING || sym_elem2->type != VAR_INT) {
		error(99, "codegen.c", "getchar_str", "Incompatible data type");
	}

	if (sym_elem1->is_const && sym_elem2->is_const) {
		fprintf(OUTPUT, "GETCHAR %s@%s string@%s int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.string_t,
		        sym_elem2->data.int_t);
	}
	else if (sym_elem1->is_const && !sym_elem2->is_const) {
		fprintf(OUTPUT, "GETCHAR %s@%s string@%s %s@%s\n", frame_dest, sym_dest->name, sym_elem1->data.string_t,
		        frame_elem2, sym_elem2->name);
	}
	else if (!sym_elem1->is_const && sym_elem2->is_const) {
		fprintf(OUTPUT, "GETCHAR %s@%s %s@%s int@%d\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        sym_elem2->data.int_t);
	}
	else {
		fprintf(OUTPUT, "GETCHAR %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
		        frame_elem2, sym_elem2->name);
	}
}

void setchar_str(instr_t instr) {
	// TODO : not used directly, remove later
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;
	elem_t *elem2 = instr.elem2_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "setchar_str", "NULL element");
	}

	if (elem1 == NULL) {
		error(99, "codegen.c", "setchar_str", "NULL element");
	}

	if (elem2 == NULL) {
		error(99, "codegen.c", "setchar_str", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "setchar_str", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "setchar_str", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "setchar_str", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);

	if (sym_dest->type != VAR_STRING || sym_elem1->type != VAR_STRING || sym_elem2->type != VAR_INT) {
		error(99, "codegen.c", "setchar_str", "Incompatible data type");
	}

	fprintf(OUTPUT, "SETCHAR %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
	        frame_elem2, sym_elem2->name);
}

void substr_str(instr_t instr) {
	//
}

void if_def(instr_t instr) {
	jmp_label_stack_push(end_labels_top, end_index);
	end_index++;
}

void if_start(instr_t instr) {
	fprintf(OUTPUT, "CREATEFRAME\n");
	fprintf(OUTPUT, "PUSHFRAME\n");
}

void if_end(instr_t instr) {
	fprintf(OUTPUT, "POPFRAME\n");
	fprintf(OUTPUT, "JUMP %s%d\n", IF_END, jmp_label_stack_top(end_labels_top));
	fprintf(OUTPUT, "LABEL %s%d\n", IF_SKIP, jmp_label_stack_pop(skip_labels_bottom, skip_labels_top));

	if (instr.next->type != IC_ELSEIF_DEF && instr.next->type != IC_ELSE_START) {
		// if block end
		fprintf(OUTPUT, "LABEL %s%d\n", IF_END, jmp_label_stack_pop(end_labels_bottom, end_labels_top));
	}
}

void elseif_def(instr_t instr) {
	//
}

void elseif_start(instr_t instr) {
	fprintf(OUTPUT, "CREATEFRAME\n");
	fprintf(OUTPUT, "PUSHFRAME\n");
}

void elseif_end(instr_t instr) {
	fprintf(OUTPUT, "POPFRAME\n");
	fprintf(OUTPUT, "JUMP %s%d\n", IF_END, jmp_label_stack_top(end_labels_top));
	fprintf(OUTPUT, "LABEL %s%d\n", IF_SKIP, jmp_label_stack_pop(skip_labels_bottom, skip_labels_top));

	if (instr.next->type != IC_ELSEIF_DEF && instr.next->type != IC_ELSE_START) {
		// if block end
		fprintf(OUTPUT, "LABEL %s%d\n", IF_END, jmp_label_stack_pop(end_labels_bottom, end_labels_top));
	}
}

void else_start(instr_t instr) {
	fprintf(OUTPUT, "CREATEFRAME\n");
	fprintf(OUTPUT, "PUSHFRAME\n");
}

void else_end(instr_t instr) {
	fprintf(OUTPUT, "POPFRAME\n");
	fprintf(OUTPUT, "JUMP %s%d\n", IF_END, jmp_label_stack_top(end_labels_top));

	// if block end
	fprintf(OUTPUT, "LABEL %s%d\n", IF_END, jmp_label_stack_pop(end_labels_bottom, end_labels_top));
}

void for_def(instr_t instr) {
	//
}

void for_cond(instr_t instr) {
	jmp_label_stack_push(for_cond_top, for_cond_index);
	fprintf(OUTPUT, "LABEL %s%d\n", FOR_COND, for_cond_index);
	for_cond_index++;
}

void for_step(instr_t instr) {
	jmp_label_stack_push(for_step_top, for_step_index);
	fprintf(OUTPUT, "LABEL %s%d\n", FOR_STEP, for_step_index);
	for_step_index++;
}

void for_body_start(instr_t instr) {
	fprintf(OUTPUT, "JUMP %s%d\n", FOR_COND, jmp_label_stack_top(for_cond_top));
	fprintf(OUTPUT, "LABEL %s%d\n", FOR_BODY, jmp_label_stack_top(for_body_top));
}

void for_body_end(instr_t instr) {
	fprintf(OUTPUT, "JUMP %s%d\n", FOR_STEP, jmp_label_stack_top(for_step_top));
	fprintf(OUTPUT, "LABEL %s%d\n", FOR_END, jmp_label_stack_top(for_end_top));

	jmp_label_stack_pop(for_cond_bottom, for_cond_top);
	jmp_label_stack_pop(for_step_bottom, for_step_top);
	jmp_label_stack_pop(for_body_bottom, for_body_top);
	jmp_label_stack_pop(for_end_bottom, for_end_top);
}

void codegen_generate_instr() {
	instr_t *instr = list->active;

	if (instr == NULL) {
		error(99, "codegen.c", "codegen_generate_instr", "Empty instructions list");
	}

	while (instr != NULL) {
		switch (instr->type) {
			case IC_DECL_VAR:
				declr_var(*instr);
				break;
			case IC_DEF_VAR:
				def_var(*instr);
				break;
			case IC_DEF_FUN:
				def_fun(*instr);
				break;
			case IC_END_FUN:
				ret_fun(*instr);
				break;
			case IC_CALL_FUN:
				call_fun(*instr);
				break;
			case IC_RET_FUN:
				ret_fun(*instr);
				break;
			case IC_ADD_VAR:
				add_var(*instr);
				break;
			case IC_SUB_VAR:
				sub_var(*instr);
				break;
			case IC_MUL_VAR:
				mul_var(*instr);
				break;
			case IC_DIV_VAR:
				div_var(*instr);
				break;
			case IC_LT_VAR:
				lt_var(*instr);
				break;
			case IC_GT_VAR:
				gt_var(*instr);
				break;
			case IC_EQ_VAR:
				eq_var(*instr);
				break;
			case IC_AND_VAR:
				and_var(*instr);
				break;
			case IC_OR_VAR:
				or_var(*instr);
				break;
			case IC_NOT_VAR:
				not_var(*instr);
				break;
			case IC_INT2FLOAT_VAR:
				int2float(*instr);
				break;
			case IC_FLOAT2INT_VAR:
				float2int(*instr);
				break;
			case IC_INT2CHAR_VAR:
				int2char(*instr);
				break;
			case IC_STR2INT_VAR:
				str2int(*instr);
				break;
			case IC_READ_VAR:
				read_var(*instr);
				break;
			case IC_WRITE_VAR:
				write_var(*instr);
				break;
			case IC_CONCAT_STR:
				concat_str(*instr);
				break;
			case IC_STRLEN_STR:
				strlen_str(*instr);
				break;
			case IC_GETCHAR_STR:
				getchar_str(*instr);
				break;
			case IC_SETCHAR_STR:
				setchar_str(*instr);
				break;
			case IC_SUBSTR_STR:
				substr_str(*instr);
				break;
			case IC_IF_DEF:
				if_def(*instr);
				break;
			case IC_IF_START:
				if_start(*instr);
				break;
			case IC_IF_END:
				if_end(*instr);
				break;
			case IC_ELSEIF_DEF:
				elseif_def(*instr);
				break;
			case IC_ELSEIF_START:
				elseif_start(*instr);
				break;
			case IC_ELSEIF_END:
				elseif_end(*instr);
				break;
			case IC_ELSE_START:
				else_start(*instr);
				break;
			case IC_ELSE_END:
				else_end(*instr);
				break;
			case IC_FOR_DEF:
				for_def(*instr);
				break;
			case IC_FOR_COND:
				for_cond(*instr);
				break;
			case IC_FOR_STEP:
				for_step(*instr);
				break;
			case IC_FOR_BODY_START:
				for_body_start(*instr);
				break;
			case IC_FOR_BODY_END:
				for_body_end(*instr);
				break;
			default:
				error(99, "codegen.c", "codegen_generate_instr", "Invalid instruction");
				break;
		}
		last_instr = instr;
		instr = list_get_next(list);
	}
}

void codegen() {
	codegen_init();
	codegen_generate_instr();
}
