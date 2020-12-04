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

instr_t *last_instr = NULL;
int skip_index = 0;
int end_index = 0;
int for_def_index = 0;

bool read_int_used = false;
bool read_float_used = false;
bool read_string_used = false;
bool read_bool_used = false;
bool getchar_used = false;
bool str2int_used = false;
bool substr_used = false;

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
		fprintf(OUTPUT, "JUMPIFEQ %s%d %s@%s bool@true\n", FOR_BODY, jmp_label_stack_top(for_def_top), frame_dest, sym_dest->name);
		fprintf(OUTPUT, "JUMPIFNEQ %s%d %s@%s bool@true\n", FOR_END, jmp_label_stack_top(for_def_top), frame_dest, sym_dest->name);
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

}

void def_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem_1 = instr.elem1_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "def_var", "NULL element");
	}

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

	while (sym_elem1 != NULL) {
		if (sym_dest == NULL) {
			sym_dest = sym_var_list_next(elem_dest->symbol.sym_var_list);
			sym_elem1 = sym_var_list_next(elem_1->symbol.sym_var_list);
			continue;
		}

		char *frame_dest = get_frame(sym_dest);
		char *frame_elem1 = get_frame(sym_elem1);

		if (sym_dest->type != sym_elem1->type) {
			error(99, "codegen.c", "def_var", "Incompatible data types");
		}

		if (sym_elem1->is_const) {
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

		sym_dest = sym_var_list_next(elem_dest->symbol.sym_var_list);
		sym_elem1 = sym_var_list_next(elem_1->symbol.sym_var_list);
	}

}

void def_fun(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;

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

	if (elem1 != NULL) {
		sym_var_list_t *sym_declarations = elem1->symbol.sym_var_list;
		// Declare all variables in function
		if (sym_declarations != NULL) {
			sym_var_item_t *declaration = sym_var_list_next(sym_declarations);

			while (declaration != NULL) {
				char *frame_declaration = get_frame(declaration);

				// Declare variable
				fprintf(OUTPUT, "DEFVAR %s@%s\n", frame_declaration, declaration->name);

				declaration = sym_var_list_next(sym_declarations);
			}
		}
	}
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
	if (returns != NULL && instr.type == IC_RET_FUN) {
		sym_var_item_t *return_active = sym_var_list_next(returns);

		while (return_active != NULL) {
			char *frame = get_frame(return_active);

			if (return_active->is_const) {
				switch (return_active->type) {
					case VAR_INT:
						fprintf(OUTPUT, "PUSHS int@%d\n", return_active->data.int_t);
						break;
					case VAR_FLOAT64:
						fprintf(OUTPUT, "PUSHS float@%a\n", return_active->data.float64_t);
						break;
					case VAR_STRING:
						fprintf(OUTPUT, "PUSHS string@%s\n", return_active->data.string_t);
						break;
					case VAR_BOOL:
						if (return_active->data.bool_t) {
							fprintf(OUTPUT, "PUSHS bool@true\n");
						}
						else {
							fprintf(OUTPUT, "PUSHS bool@false\n");
						}
						break;
					case VAR_NIL:
						error(99, "codegen.c", "ret_fun", "NIL variable");
						break;
					case VAR_UNDEFINED:
						error(99, "codegen.c", "ret_fun", "Undefined variable");
						break;
				}
			}
			else {
				fprintf(OUTPUT, "PUSHS %s@%s\n", frame, return_active->name);
			}
			return_active = sym_var_list_next(returns);
		}
	}

	fprintf(OUTPUT, "POPFRAME\n");

	// check if function is main, if yes then EXIT may be called
	if (strcmp(elem_dest->symbol.sym_func->name, "main") == 0) {
		// main function
			fprintf(OUTPUT, "EXIT int@0\n\n");
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

	if (sym_dest->type == VAR_STRING && sym_elem1->type == VAR_STRING && sym_elem2->type == VAR_STRING) {
		concat_str(instr);
		return;
	}

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
		error(99, "codegen.c", "lt_var", "Non bool destination type");
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
			case VAR_NIL:
				error(99, "codegen.c", "lt_var", "Undefined data type");
				break;
			case VAR_UNDEFINED:
				error(99, "codegen.c", "lt_var", "Wrong data type");
				break;
			default:
				error(99, "codegen.c", "lt_var", "Undefined data type");
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
			case VAR_NIL:
				error(99, "codegen.c", "lt_var", "Undefined data type");
				break;
			case VAR_UNDEFINED:
				error(99, "codegen.c", "lt_var", "Wrong data type");
				break;
			default:
				error(99, "codegen.c", "lt_var", "Undefined data type");
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
			case VAR_NIL:
				error(99, "codegen.c", "lt_var", "Undefined data type");
				break;
			case VAR_UNDEFINED:
				error(99, "codegen.c", "lt_var", "Wrong data type");
				break;
			default:
				error(99, "codegen.c", "lt_var", "Undefined data type");
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
			case VAR_NIL:
				error(99, "codegen.c", "gt_var", "Undefined data type");
				break;
			case VAR_UNDEFINED:
				error(99, "codegen.c", "gt_var", "Wrong data type");
				break;
			default:
				error(99, "codegen.c", "gt_var", "Undefined data type");
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
			case VAR_NIL:
				error(99, "codegen.c", "gt_var", "Undefined data type");
				break;
			case VAR_UNDEFINED:
				error(99, "codegen.c", "gt_var", "Wrong data type");
				break;
			default:
				error(99, "codegen.c", "gt_var", "Undefined data type");
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
			case VAR_NIL:
				error(99, "codegen.c", "gt_var", "Undefined data type");
				break;
			case VAR_UNDEFINED:
				error(99, "codegen.c", "gt_var", "Wrong data type");
				break;
			default:
				error(99, "codegen.c", "gt_var", "Undefined data type");
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
			case VAR_NIL:
				error(99, "codegen.c", "eq_var", "Undefined data type");
				break;
			case VAR_UNDEFINED:
				error(99, "codegen.c", "eq_var", "Wrong data type");
				break;
			default:
				error(99, "codegen.c", "eq_var", "Undefined data type");
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
			case VAR_NIL:
				error(99, "codegen.c", "eq_var", "Undefined data type");
				break;
			case VAR_UNDEFINED:
				error(99, "codegen.c", "eq_var", "Wrong data type");
				break;
			default:
				error(99, "codegen.c", "eq_var", "Undefined data type");
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
					fprintf(OUTPUT, "EQ %s@%s %s@%s bool@false\n", frame_dest, sym_dest->name,
					        frame_elem1, sym_elem1->name);
				}
				break;
			case VAR_NIL:
				error(99, "codegen.c", "eq_var", "Undefined data type");
				break;
			case VAR_UNDEFINED:
				error(99, "codegen.c", "eq_var", "Wrong data type");
				break;
			default:
				error(99, "codegen.c", "eq_var", "Undefined data type");
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

	char *frame_dest = get_frame(sym_dest);
	char *frame_err = get_frame(sym_err);
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

	if (sym_err->type != VAR_INT) {
		error(99, "codegen.c", "str2int", "Incompatible data type");
	}

	//push params
	if (sym_elem1->is_const) {
		fprintf(OUTPUT, "PUSHS string@%s\n", sym_elem1->data.string_t);
	}
	else {
		fprintf(OUTPUT, "PUSHS %s@%s\n", frame_elem1, sym_elem1->name);
	}

	if (sym_elem2->is_const) {
		fprintf(OUTPUT, "PUSHS int@%d\n", sym_elem2->data.int_t);
	}
	else {
		fprintf(OUTPUT, "PUSHS %s@%s\n", frame_elem2, sym_elem2->name);
	}

	fprintf(OUTPUT, "CALL str2int--def\n");

	// pop returns
	fprintf(OUTPUT, "POPS %s@%s\n", frame_err, sym_err->name);
	fprintf(OUTPUT, "POPS %s@%s\n", frame_dest, sym_dest->name);

	str2int_used = true;
}

void read_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "read_var", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_err = elem_dest->symbol.sym_func->returns->first->next->item;

	if (sym_dest == NULL || sym_err == NULL) {
		error(99, "codegen.c", "read_var", "NULL symbol");
	}

	if (sym_err->type != VAR_INT) {
		error(99, "codegen.c", "read_var", "Invalid error data type");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_err = get_frame(sym_err);

	if (sym_dest->type == VAR_INT) {
		fprintf(OUTPUT, "CALL read-int--def\n");
		read_int_used = true;
	}
	else if (sym_dest->type == VAR_FLOAT64) {
		fprintf(OUTPUT, "CALL read-float--def\n");
		read_float_used = true;
	}
	else if (sym_dest->type == VAR_STRING) {
		fprintf(OUTPUT, "CALL read-string--def\n");
		read_string_used = true;
	}
	else if (sym_dest->type == VAR_BOOL) {
		fprintf(OUTPUT, "CALL read-bool--def\n");
		read_bool_used = true;
	}
	else {
		error(99, "codegen.c", "read_var", "Invalid data type");
	}

	fprintf(OUTPUT, "POPS %s@%s\n", frame_err, sym_err->name);
	fprintf(OUTPUT, "POPS %s@%s\n", frame_dest, sym_dest->name);
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
		return;
		//error(99, "codegen.c", "write_var", "Invalid symbol");
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
	elem_t *elem_dest = instr_get_dest(&instr);
	elem_t *elem1 = instr_get_elem1(&instr);
	elem_t *elem2 = instr_get_elem2(&instr);

	if (elem_dest == NULL || elem1 == NULL || elem2 == NULL) {
		error(99, "codegen.c", "concat_str", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_var_item;
	sym_var_item_t *sym_elem1 = elem1->symbol.sym_var_item;
	sym_var_item_t *sym_elem2 = elem2->symbol.sym_var_item;

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
		error(99, "codegen.c", "int2char", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_err = elem_dest->symbol.sym_func->returns->first->next->item;
	sym_var_item_t *sym_elem1 = elem_dest->symbol.sym_func->params->first->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "int2char", "NULL symbol");
	}

	if (sym_err == NULL) {
		error(99, "codegen.c", "int2char", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "int2char", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_err = get_frame(sym_err);
	char *frame_elem1 = get_frame(sym_elem1);

	if (sym_dest->type != VAR_STRING) {
		error(99, "codegen.c", "int2char", "Incompatible data type");
	}

	if (sym_err->type != VAR_INT) {
		error(99, "codegen.c", "int2char", "Incompatible data type");
	}

	if (sym_elem1->type != VAR_INT) {
		error(99, "codegen.c", "int2char", "Incompatible data type");
	}

	// push params
	if (sym_elem1->is_const) {
		fprintf(OUTPUT, "PUSHS int@%d\n", sym_elem1->data.int_t);
	}
	else {
		fprintf(OUTPUT, "PUSHS %s@%s\n", frame_elem1, sym_elem1->name);
	}

	fprintf(OUTPUT, "CALL getchar--def\n");

	// pop returns
	fprintf(OUTPUT, "POPS %s@%s\n", frame_err, sym_err->name);
	fprintf(OUTPUT, "POPS %s@%s\n", frame_dest, sym_dest->name);

	getchar_used = true;
}

void substr_str(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "substr_str", "NULL element");
	}

	sym_var_item_t *sym_dest = elem_dest->symbol.sym_func->returns->first->item;
	sym_var_item_t *sym_err = elem_dest->symbol.sym_func->returns->first->next->item;
	sym_var_item_t *sym_elem1 = elem_dest->symbol.sym_func->params->first->item;
	sym_var_item_t *sym_elem2 = elem_dest->symbol.sym_func->params->first->next->item;
	sym_var_item_t *sym_elem3 = elem_dest->symbol.sym_func->params->first->next->next->item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "substr_str", "NULL symbol");
	}

	if (sym_err == NULL) {
		error(99, "codegen.c", "substr_str", "NULL symbol");
	}

	if (sym_elem1 == NULL) {
		error(99, "codegen.c", "substr_str", "NULL symbol");
	}

	if (sym_elem2 == NULL) {
		error(99, "codegen.c", "substr_str", "NULL symbol");
	}

	if (sym_elem3 == NULL) {
		error(99, "codegen.c", "substr_str", "NULL symbol");
	}

	char *frame_dest = get_frame(sym_dest);
	char *frame_err = get_frame(sym_err);
	char *frame_elem1 = get_frame(sym_elem1);
	char *frame_elem2 = get_frame(sym_elem2);
	char *frame_elem3 = get_frame(sym_elem3);

	if (sym_dest->type != VAR_STRING) {
		error(99, "codegen.c", "substr_str", "Incompatible data type");
	}

	if (sym_err->type != VAR_INT) {
		error(99, "codegen.c", "substr_str", "Incompatible data type");
	}

	if (sym_elem1->type != VAR_STRING) {
		error(99, "codegen.c", "substr_str", "Incompatible data type");
	}

	if (sym_elem2->type != VAR_INT) {
		error(99, "codegen.c", "substr_str", "Incompatible data type");
	}

	if (sym_elem3->type != VAR_INT) {
		error(99, "codegen.c", "substr_str", "Incompatible data type");
	}

	// push params
	if (sym_elem1->is_const) {
		fprintf(OUTPUT, "PUSHS string@%s\n", sym_elem1->data.string_t);
	}
	else {
		fprintf(OUTPUT, "PUSHS %s@%s\n", frame_elem1, sym_elem1->name);
	}

	if (sym_elem2->is_const) {
		fprintf(OUTPUT, "PUSHS int@%d\n", sym_elem2->data.int_t);
	}
	else {
		fprintf(OUTPUT, "PUSHS %s@%s\n", frame_elem2, sym_elem2->name);
	}

	if (sym_elem3->is_const) {
		fprintf(OUTPUT, "PUSHS int@%d\n", sym_elem3->data.int_t);
	}
	else {
		fprintf(OUTPUT, "PUSHS %s@%s\n", frame_elem3, sym_elem3->name);
	}

	fprintf(OUTPUT, "CALL substr--def\n");

	// pop returns
	fprintf(OUTPUT, "POPS %s@%s\n", frame_err, sym_err->name);
	fprintf(OUTPUT, "POPS %s@%s\n", frame_dest, sym_dest->name);

	substr_used = true;
}

// Build-in function definitions
void substr_str_def() {
	fprintf(OUTPUT, "LABEL substr--def\n");
	fprintf(OUTPUT, "CREATEFRAME\n");

	fprintf(OUTPUT, "DEFVAR TF@dest\n");
	fprintf(OUTPUT, "DEFVAR TF@error\n");
	fprintf(OUTPUT, "DEFVAR TF@src\n");
	fprintf(OUTPUT, "DEFVAR TF@index\n");
	fprintf(OUTPUT, "DEFVAR TF@length\n");

	fprintf(OUTPUT, "POPS TF@length\n");
	fprintf(OUTPUT, "POPS TF@index\n");
	fprintf(OUTPUT, "POPS TF@src\n");

	fprintf(OUTPUT, "PUSHFRAME\n");

	// init default
	fprintf(OUTPUT, "MOVE LF@dest string@\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "DEFVAR LF@substr-res\n");
	fprintf(OUTPUT, "DEFVAR LF@substr-src-len\n");
	fprintf(OUTPUT, "MOVE LF@substr-res bool@false\n");
	fprintf(OUTPUT, "MOVE LF@substr-src-len int@0\n");

	fprintf(OUTPUT, "STRLEN LF@substr-src-len LF@src\n");

	// conditions
	fprintf(OUTPUT, "LT LF@substr-res LF@index int@0\n");
	fprintf(OUTPUT, "JUMPIFEQ substr--error LF@substr-res bool@true\n");
	fprintf(OUTPUT, "GT LF@substr-res LF@index LF@substr-src-len\n");
	fprintf(OUTPUT, "JUMPIFEQ substr--error LF@substr-res bool@true\n");
	fprintf(OUTPUT, "LT LF@substr-res LF@length int@0\n");
	fprintf(OUTPUT, "JUMPIFEQ substr--error LF@substr-res bool@true\n");

	// valid
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	// clean (prepare) destination
	fprintf(OUTPUT, "MOVE LF@dest string@\n");

	// check if length > strlen(src) > index
	fprintf(OUTPUT, "DEFVAR LF@substr-overflow\n");
	fprintf(OUTPUT, "DEFVAR LF@substr-len-min-i\n");
	fprintf(OUTPUT, "DEFVAR LF@substr-final-size\n");

	fprintf(OUTPUT, "MOVE LF@substr-overflow bool@false\n");
	fprintf(OUTPUT, "MOVE LF@substr-len-min-i int@0\n");
	fprintf(OUTPUT, "SUB LF@substr-len-min-i LF@substr-src-len LF@index\n");
	fprintf(OUTPUT, "GT LF@substr-overflow LF@length LF@substr-len-min-i\n");
	fprintf(OUTPUT, "JUMPIFEQ substr--method-alt LF@substr-overflow bool@true\n");

	// use n
	fprintf(OUTPUT, "MOVE LF@substr-final-size LF@length\n");
	fprintf(OUTPUT, "JUMP substr--cycle-init\n");

	// use strlen(src) - index
	fprintf(OUTPUT, "LABEL substr--method-alt\n");
	fprintf(OUTPUT, "MOVE LF@substr-final-size LF@substr-len-min-i\n");

	// cycle from index
	fprintf(OUTPUT, "LABEL substr--cycle-init\n");
	fprintf(OUTPUT, "ADD LF@substr-final-size LF@substr-final-size LF@index\n");
	fprintf(OUTPUT, "DEFVAR LF@substr-cycle-i\n");
	fprintf(OUTPUT, "DEFVAR LF@substr-cycle-b\n");
	fprintf(OUTPUT, "DEFVAR LF@substr-cycle-dest\n");
	fprintf(OUTPUT, "MOVE LF@substr-cycle-i LF@index\n");
	fprintf(OUTPUT, "MOVE LF@substr-cycle-b bool@false\n");
	fprintf(OUTPUT, "MOVE LF@substr-cycle-dest string@\n");

	// cycle iteration
	fprintf(OUTPUT, "LABEL substr--cycle-start\n");
	fprintf(OUTPUT, "LT LF@substr-cycle-b LF@substr-cycle-i LF@substr-final-size\n");
	fprintf(OUTPUT, "JUMPIFEQ substr--cycle-stop LF@substr-cycle-b bool@false\n");
	fprintf(OUTPUT, "GETCHAR LF@substr-cycle-dest LF@src LF@substr-cycle-i\n");
	fprintf(OUTPUT, "CONCAT LF@dest LF@dest LF@substr-cycle-dest\n");

	// cycle post iteration
	fprintf(OUTPUT, "ADD LF@substr-cycle-i LF@substr-cycle-i int@1\n");
	fprintf(OUTPUT, "JUMP substr--cycle-start\n");

	// cycle end
	fprintf(OUTPUT, "LABEL substr--cycle-stop\n");
	fprintf(OUTPUT, "JUMP substr--exit\n");

	// error
	fprintf(OUTPUT, "LABEL substr--error\n");
	fprintf(OUTPUT, "MOVE LF@error int@1\n");

	// exit
	fprintf(OUTPUT, "LABEL substr--exit\n");

	fprintf(OUTPUT, "PUSHS LF@dest\n");
	fprintf(OUTPUT, "PUSHS LF@error\n");

	fprintf(OUTPUT, "POPFRAME\n");

	fprintf(OUTPUT, "RETURN\n\n");
}

void getchar_str_def() {
	fprintf(OUTPUT, "LABEL getchar--def\n");
	fprintf(OUTPUT, "CREATEFRAME\n");

	fprintf(OUTPUT, "DEFVAR TF@dest\n");
	fprintf(OUTPUT, "DEFVAR TF@error\n");
	fprintf(OUTPUT, "DEFVAR TF@code\n");

	fprintf(OUTPUT, "POPS TF@code\n");

	fprintf(OUTPUT, "PUSHFRAME\n");

	// init default
	fprintf(OUTPUT, "MOVE LF@dest string@\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "DEFVAR LF@getchar-res\n");
	fprintf(OUTPUT, "MOVE LF@getchar-res bool@false\n");

	// condition
	fprintf(OUTPUT, "LT LF@getchar-res LF@code int@0\n");
	fprintf(OUTPUT, "JUMPIFEQ getchar--error LF@getchar-res bool@true\n");
	fprintf(OUTPUT, "GT LF@getchar-res LF@code int@255\n");
	fprintf(OUTPUT, "JUMPIFEQ getchar--error LF@getchar-res bool@true\n");

	// valid
	fprintf(OUTPUT, "MOVE LF@error int@0\n");
	fprintf(OUTPUT, "INT2CHAR LF@dest LF@code\n");
	fprintf(OUTPUT, "JUMP getchar--exit\n");

	// error
	fprintf(OUTPUT, "LABEL getchar--error\n");
	fprintf(OUTPUT, "MOVE LF@error int@1\n");

	// exit
	fprintf(OUTPUT, "LABEL getchar--exit\n");

	fprintf(OUTPUT, "PUSHS LF@dest\n");
	fprintf(OUTPUT, "PUSHS LF@error\n");

	fprintf(OUTPUT, "POPFRAME\n");

	fprintf(OUTPUT, "RETURN\n\n");
}

void str2int_def() {
	fprintf(OUTPUT, "LABEL str2int--def\n");
	fprintf(OUTPUT, "CREATEFRAME\n");

	fprintf(OUTPUT, "DEFVAR TF@dest\n");
	fprintf(OUTPUT, "DEFVAR TF@error\n");
	fprintf(OUTPUT, "DEFVAR TF@src\n");
	fprintf(OUTPUT, "DEFVAR TF@index\n");

	fprintf(OUTPUT, "POPS TF@index\n");
	fprintf(OUTPUT, "POPS TF@src\n");

	fprintf(OUTPUT, "PUSHFRAME\n");

	// init default
	fprintf(OUTPUT, "MOVE LF@dest int@0\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "DEFVAR LF@str2int-src-len\n");
	fprintf(OUTPUT, "DEFVAR LF@str2int-src-res\n");
	fprintf(OUTPUT, "MOVE LF@str2int-src-len int@0\n");
	fprintf(OUTPUT, "MOVE LF@str2int-src-res bool@false\n");

	fprintf(OUTPUT, "STRLEN LF@str2int-src-len LF@src\n");
	fprintf(OUTPUT, "SUB LF@str2int-src-len LF@str2int-src-len int@1\n");

	// Check conditions
	fprintf(OUTPUT, "LT LF@str2int-src-res LF@index int@0\n");
	fprintf(OUTPUT, "JUMPIFEQ str2int--error LF@str2int-src-res bool@true\n");
	fprintf(OUTPUT, "GT LF@str2int-src-res LF@index LF@str2int-src-len\n");
	fprintf(OUTPUT, "JUMPIFEQ str2int--error LF@str2int-src-res bool@true\n");

	// valid
	fprintf(OUTPUT, "MOVE LF@error int@0\n");
	fprintf(OUTPUT, "STRI2INT LF@dest LF@src LF@index\n");
	fprintf(OUTPUT, "JUMP str2int--exit\n");

	// error
	fprintf(OUTPUT, "LABEL str2int--error\n");
	fprintf(OUTPUT, "MOVE LF@error int@1\n");

	// exit
	fprintf(OUTPUT, "LABEL str2int--exit\n");

	fprintf(OUTPUT, "PUSHS LF@dest\n");
	fprintf(OUTPUT, "PUSHS LF@error\n");

	fprintf(OUTPUT, "POPFRAME\n");

	fprintf(OUTPUT, "RETURN\n\n");
}

void read_int_def() {
	fprintf(OUTPUT, "LABEL read-int--def\n");
	fprintf(OUTPUT, "CREATEFRAME\n");

	fprintf(OUTPUT, "DEFVAR TF@dest\n");
	fprintf(OUTPUT, "DEFVAR TF@error\n");

	fprintf(OUTPUT, "PUSHFRAME\n");

	// init default
	fprintf(OUTPUT, "MOVE LF@dest int@0\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "DEFVAR LF@read-int-dest\n");
	fprintf(OUTPUT, "DEFVAR LF@read-int-res\n");
	fprintf(OUTPUT, "MOVE LF@read-int-res bool@false\n");

	// read string
	fprintf(OUTPUT, "READ LF@read-int-dest int\n");

	fprintf(OUTPUT, "EQ LF@read-int-res LF@read-int-dest nil@nil\n");
	fprintf(OUTPUT, "JUMPIFEQ read-int--valid LF@read-int-res bool@false\n");

	// error
	fprintf(OUTPUT, "MOVE LF@error int@1\n");
	fprintf(OUTPUT, "JUMP read-int--exit\n");

	// valid
	fprintf(OUTPUT, "LABEL read-int--valid\n");
	fprintf(OUTPUT, "MOVE LF@dest LF@read-int-dest\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "LABEL read-int--exit\n");

	fprintf(OUTPUT, "PUSHS LF@dest\n");
	fprintf(OUTPUT, "PUSHS LF@error\n");

	fprintf(OUTPUT, "POPFRAME\n");

	fprintf(OUTPUT, "RETURN\n\n");
}

void read_float_def() {
	fprintf(OUTPUT, "LABEL read-float--def\n");
	fprintf(OUTPUT, "CREATEFRAME\n");

	fprintf(OUTPUT, "DEFVAR TF@dest\n");
	fprintf(OUTPUT, "DEFVAR TF@error\n");

	fprintf(OUTPUT, "PUSHFRAME\n");

	// init default
	fprintf(OUTPUT, "MOVE LF@dest float@0x0p+0\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "DEFVAR LF@read-float-dest\n");
	fprintf(OUTPUT, "DEFVAR LF@read-float-res\n");
	fprintf(OUTPUT, "MOVE LF@read-float-res bool@false\n");

	// read string
	fprintf(OUTPUT, "READ LF@read-float-dest float\n");

	fprintf(OUTPUT, "EQ LF@read-float-res LF@read-float-dest nil@nil\n");
	fprintf(OUTPUT, "JUMPIFEQ read-float--valid LF@read-float-res bool@false\n");

	// error
	fprintf(OUTPUT, "MOVE LF@error int@1\n");
	fprintf(OUTPUT, "JUMP read-float--exit\n");

	// valid
	fprintf(OUTPUT, "LABEL read-float--valid\n");
	fprintf(OUTPUT, "MOVE LF@dest LF@read-float-dest\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "LABEL read-float--exit\n");

	fprintf(OUTPUT, "PUSHS LF@dest\n");
	fprintf(OUTPUT, "PUSHS LF@error\n");

	fprintf(OUTPUT, "POPFRAME\n");

	fprintf(OUTPUT, "RETURN\n\n");
}

void read_string_def() {
	fprintf(OUTPUT, "LABEL read-string--def\n");
	fprintf(OUTPUT, "CREATEFRAME\n");

	fprintf(OUTPUT, "DEFVAR TF@dest\n");
	fprintf(OUTPUT, "DEFVAR TF@error\n");

	fprintf(OUTPUT, "PUSHFRAME\n");

	// init default
	fprintf(OUTPUT, "MOVE LF@dest string@\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "DEFVAR LF@read-string-dest\n");
	fprintf(OUTPUT, "DEFVAR LF@read-string-res\n");
	fprintf(OUTPUT, "MOVE LF@read-string-res bool@false\n");

	// read string
	fprintf(OUTPUT, "READ LF@read-string-dest string\n");

	fprintf(OUTPUT, "EQ LF@read-string-res LF@read-string-dest nil@nil\n");
	fprintf(OUTPUT, "JUMPIFEQ read-string--valid LF@read-string-res bool@false\n");

	// error
	fprintf(OUTPUT, "MOVE LF@error int@1\n");
	fprintf(OUTPUT, "JUMP read-string--exit\n");

	// valid
	fprintf(OUTPUT, "LABEL read-string--valid\n");
	fprintf(OUTPUT, "MOVE LF@dest LF@read-string-dest\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "LABEL read-string--exit\n");

	fprintf(OUTPUT, "PUSHS LF@dest\n");
	fprintf(OUTPUT, "PUSHS LF@error\n");

	fprintf(OUTPUT, "POPFRAME\n");

	fprintf(OUTPUT, "RETURN\n\n");
}

void read_bool_def() {
	fprintf(OUTPUT, "LABEL read-bool--def\n");
	fprintf(OUTPUT, "CREATEFRAME\n");

	fprintf(OUTPUT, "DEFVAR TF@dest\n");
	fprintf(OUTPUT, "DEFVAR TF@error\n");

	fprintf(OUTPUT, "PUSHFRAME\n");

	// init default
	fprintf(OUTPUT, "MOVE LF@dest bool@false\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "DEFVAR LF@read-bool-dest\n");
	fprintf(OUTPUT, "DEFVAR LF@read-bool-res\n");
	fprintf(OUTPUT, "MOVE LF@read-bool-res bool@false\n");

	// read string
	fprintf(OUTPUT, "READ LF@read-bool-dest bool\n");

	fprintf(OUTPUT, "EQ LF@read-bool-res LF@read-bool-dest nil@nil\n");
	fprintf(OUTPUT, "JUMPIFEQ read-bool--valid LF@read-bool-res bool@false\n");

	// error
	fprintf(OUTPUT, "MOVE LF@error int@1\n");
	fprintf(OUTPUT, "JUMP read-bool--exit\n");

	// valid
	fprintf(OUTPUT, "LABEL read-bool--valid\n");
	fprintf(OUTPUT, "MOVE LF@dest LF@read-bool-dest\n");
	fprintf(OUTPUT, "MOVE LF@error int@0\n");

	fprintf(OUTPUT, "LABEL read-bool--exit\n");

	fprintf(OUTPUT, "PUSHS LF@dest\n");
	fprintf(OUTPUT, "PUSHS LF@error\n");

	fprintf(OUTPUT, "POPFRAME\n");

	fprintf(OUTPUT, "RETURN\n\n");
}

void if_def() {
	jmp_label_stack_push(end_labels_top, end_index);
	end_index++;
}

void if_start() {
	//fprintf(OUTPUT, "CREATEFRAME\n");
	//fprintf(OUTPUT, "PUSHFRAME\n");
}

void if_end(instr_t instr) {
	//fprintf(OUTPUT, "POPFRAME\n");
	fprintf(OUTPUT, "JUMP %s%d\n", IF_END, jmp_label_stack_top(end_labels_top));
	fprintf(OUTPUT, "LABEL %s%d\n", IF_SKIP, jmp_label_stack_pop(skip_labels_bottom, skip_labels_top));

	if (instr.next->type != IC_ELSEIF_DEF && instr.next->type != IC_ELSE_START) {
		// if block end
		fprintf(OUTPUT, "LABEL %s%d\n", IF_END, jmp_label_stack_pop(end_labels_bottom, end_labels_top));
	}
}

void elseif_def() {
	//
}

void elseif_start() {
	//fprintf(OUTPUT, "CREATEFRAME\n");
	//fprintf(OUTPUT, "PUSHFRAME\n");
}

void elseif_end(instr_t instr) {
	//fprintf(OUTPUT, "POPFRAME\n");
	fprintf(OUTPUT, "JUMP %s%d\n", IF_END, jmp_label_stack_top(end_labels_top));
	fprintf(OUTPUT, "LABEL %s%d\n", IF_SKIP, jmp_label_stack_pop(skip_labels_bottom, skip_labels_top));

	if (instr.next->type != IC_ELSEIF_DEF && instr.next->type != IC_ELSE_START) {
		// if block end
		fprintf(OUTPUT, "LABEL %s%d\n", IF_END, jmp_label_stack_pop(end_labels_bottom, end_labels_top));
	}
}

void else_start() {
	//fprintf(OUTPUT, "CREATEFRAME\n");
	//fprintf(OUTPUT, "PUSHFRAME\n");
}

void else_end() {
	fprintf(OUTPUT, "JUMP %s%d\n", IF_END, jmp_label_stack_top(end_labels_top));

	// if block end
	fprintf(OUTPUT, "LABEL %s%d\n", IF_END, jmp_label_stack_pop(end_labels_bottom, end_labels_top));
	//fprintf(OUTPUT, "POPFRAME\n");
}

void for_def_codegen() {
	jmp_label_stack_push(for_def_top, for_def_index);
	for_def_index++;
}

void for_cond() {
	fprintf(OUTPUT, "LABEL %s%d\n", FOR_COND, jmp_label_stack_top(for_def_top));
}

void for_step() {
	fprintf(OUTPUT, "LABEL %s%d\n", FOR_STEP, jmp_label_stack_top(for_def_top));
}

void for_body_start() {
	fprintf(OUTPUT, "JUMP %s%d\n", FOR_COND, jmp_label_stack_top(for_def_top));
	fprintf(OUTPUT, "LABEL %s%d\n", FOR_BODY, jmp_label_stack_top(for_def_top));
}

void for_body_end() {
	fprintf(OUTPUT, "JUMP %s%d\n", FOR_STEP, jmp_label_stack_top(for_def_top));
	fprintf(OUTPUT, "LABEL %s%d\n", FOR_END, jmp_label_stack_top(for_def_top));

	jmp_label_stack_pop(for_def_bottom, for_def_top);
}

void codegen_generate_instr() {
	instr_t *instr = list_get_active(list);

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
			case IC_SUBSTR_STR:
				substr_str(*instr);
				break;
			case IC_IF_DEF:
				if_def();
				break;
			case IC_IF_START:
				if_start();
				break;
			case IC_IF_END:
				if_end(*instr);
				break;
			case IC_ELSEIF_DEF:
				elseif_def();
				break;
			case IC_ELSEIF_START:
				elseif_start();
				break;
			case IC_ELSEIF_END:
				elseif_end(*instr);
				break;
			case IC_ELSE_START:
				else_start();
				break;
			case IC_ELSE_END:
				else_end();
				break;
			case IC_FOR_DEF:
				for_def_codegen();
				break;
			case IC_FOR_COND:
				for_cond();
				break;
			case IC_FOR_STEP:
				for_step();
				break;
			case IC_FOR_BODY_START:
				for_body_start();
				break;
			case IC_FOR_BODY_END:
				for_body_end();
				break;
			default:
				error(99, "codegen.c", "codegen_generate_instr", "Invalid instruction");
				break;
		}
		last_instr = instr;
		instr = list_get_next(list);
	}
}

// Generate build-in instruction if they were used
void codegen_post_generate() {
	if (read_int_used) {
		read_int_def();
	}

	if (read_float_used) {
		read_float_def();
	}

	if (read_string_used) {
		read_string_def();
	}

	if (read_bool_used) {
		read_bool_def();
	}

	if (getchar_used) {
		getchar_str_def();
	}

	if (str2int_used) {
		str2int_def();
	}

	if (substr_used) {
		substr_str_def();
	}
}

void codegen() {
	codegen_init();
	codegen_generate_instr();
	codegen_post_generate();
}
