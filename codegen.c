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
#include "error.h"

#include "escape_format.h"

#define OUTPUT stdout
#define GF "GF"
#define LF "LF"
#define TF "TF"

int fun_deep = 0;

void codegen_init() {
	fprintf(OUTPUT, ".IFJcode20\n\n");
	fprintf(OUTPUT, "JUMP main\n\n");
}

void declr_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	char *frame_dest = NULL;

	if (elem_dest->sym_type != SYM_VAR_ITEM) {
		error(99, "codegen.c", "declr_var", "Invalid symbol type");
	}

	sym_var_item_t *sym_dest = instr.elem_dest_ptr->symbol.sym_var_item;

	if (sym_dest == NULL) {
		error(99, "codegen.c", "declr_var", "NULL symbol");
	}

	if (sym_dest->is_global) {
		frame_dest = GF;
	}
	else {
		frame_dest = LF;
	}

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
			char *new_seq = escape_reformat(sym_dest->data.string_t);
			variable_t new_data = {.string_t = new_seq};
			sym_var_item_change_data(sym_dest, new_data);

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

		char *frame_dest = NULL;
		char *frame_elem1 = NULL;

		if (sym_dest->is_global) {
			frame_dest = GF;
		}
		else {
			frame_dest = LF;
		}

		if (sym_elem1->is_global) {
			frame_elem1 = GF;
		}
		else {
			frame_elem1 = LF;
		}

		if (sym_dest->type != sym_elem1->type) {
			error(99, "codegen.c", "def_var", "Incompatible data types");
		}

		if (sym_dest->is_const) {
			switch (sym_dest->type) {
				case VAR_INT:
					fprintf(OUTPUT, "MOVE %s@%s int@%d\n", frame_dest, sym_dest->name, sym_elem1->data.int_t);
					break;
				case VAR_FLOAT64:
					fprintf(OUTPUT, "MOVE %s@%s float64@%a\n", frame_dest, sym_dest->name, sym_elem1->data.float64_t);
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

	// TODO ?
	sym_func->params->active = NULL;
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
	elem_t *elem_dest = instr.elem_dest_ptr;
	elem_t *elem1 = instr.elem1_ptr;

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

	// TODO ?
	sym_func->params->active = NULL;
	sym_var_list_t *params = sym_func->params;

	if (params != NULL) {
		sym_var_item_t *param_active = sym_var_list_next(params);

		while (param_active != NULL) {
			char *frame = LF;
			fprintf(OUTPUT, "PUSHS %s@%s\n", frame, param_active->name);
			param_active = sym_var_list_next(params);
		}
	}

	fprintf(OUTPUT, "CALL %s\n", sym_func->name);

	// TODO ?
	sym_func->returns->active = NULL;
	sym_var_list_t *returns = sym_func->returns;

	if (sym_func->returns != NULL) {
		sym_func->returns->active = NULL;
	}

	if (elem_dest != NULL && elem_dest->symbol.sym_var_list != NULL) {
		elem_dest->symbol.sym_var_list->active = NULL;
	}

	if (elem_dest != NULL && returns != NULL) {
		if (elem_dest->sym_type != SYM_VAR_LIST) {
			error(99, "codegen.c", "call_fun", "Invalid symbol, not var list");
		}

		size_t dest_size = sym_var_list_size(elem_dest->symbol.sym_var_list);
		size_t elem1_size = sym_var_list_size(elem1->symbol.sym_func->returns);

		if (dest_size != elem1_size) {
			error(99, "codegen.c", "def_var", "Different var list sizes");
		}

		sym_var_item_t *return_active = sym_var_list_next(returns);
		sym_var_item_t *dest = sym_var_list_next(elem_dest->symbol.sym_var_list);

		while (dest != NULL && return_active != NULL) {
			if (dest->type != return_active->type) {
				error(99, "codegen.c", "def_var", "Different data type");
			}
			char *frame_dest = LF;
			char *frame_input = TF;
			fprintf(OUTPUT, "MOVE %s@%s %s@%s\n", frame_dest, dest->name, frame_input, return_active->name);
			return_active = sym_var_list_next(returns);
		}
	}
}

void ret_fun(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "ret_fun", "NULL element");
	}

	if (elem_dest->sym_type != SYM_FUNC) {
		error(99, "codegen.c", "ret_fun", "Invalid symbol");
	}

	fprintf(OUTPUT, "POPFRAME\n");

	if (strcmp(elem_dest->symbol.sym_func->name, "main") == 0) {
		// main function
		fprintf(OUTPUT, "EXIT int@0\n");
	}
	else {
		fprintf(OUTPUT, "RETURN\n\n");
	}
}

/*void end_fun(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "end_fun", "NULL element");
	}

	if (elem_dest->sym_type != SYM_FUNC) {
		error(99, "codegen.c", "end_fun", "Invalid symbol");
	}

	sym_func_t *sym_func = elem_dest->symbol.sym_func;

	if (sym_func == NULL) {
		error(99, "codegen.c", "end_fun", "NULL element");
	}

	fprintf(OUTPUT, "POPFRAME");
	fprintf(OUTPUT, "RETURN\n");
}*/

void write_fun(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	if (elem_dest == NULL) {
		error(99, "codegen.c", "write_fun", "NULL element");
	}

	if (elem_dest->sym_type != SYM_VAR_LIST) {
		error(99, "codegen.c", "write_fun", "Invalid symbol");
	}

	sym_var_list_t *sym_var_list = instr.elem_dest_ptr->symbol.sym_var_list;

	if (sym_var_list == NULL) {
		error(99, "codegen.c", "write_fun", "Invalid symbol");
	}

	// TODO ?
	sym_var_list->active = NULL;
	sym_var_item_t *active = sym_var_list_next(sym_var_list);
	char *frame = NULL;

	while (active != NULL) {
		if (active->is_global) {
			frame = GF;
		}
		else {
			frame = LF;
		}

		if (active->type == VAR_UNDEFINED) {
			error(99, "codegen.c", "write_fun", "Invalid data type");
		}

		fprintf(OUTPUT, "WRITE %s@%s\n", frame, active->name);

		active = sym_var_list_next(sym_var_list);
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

	char *frame_dest = NULL;
	char *frame_elem1 = NULL;
	char *frame_elem2 = NULL;

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

	if (sym_dest->is_global) {
		frame_dest = GF;
	}
	else {
		frame_dest = LF;
	}

	if (sym_elem1->is_global) {
		frame_elem1 = GF;
	}
	else {
		frame_elem1 = LF;
	}

	if (sym_elem2->is_global) {
		frame_elem2 = GF;
	}
	else {
		frame_elem2 = LF;
	}

	fprintf(OUTPUT, "ADD %s@%s %s@%s %s@%s\n", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name,
	        frame_elem2, sym_elem2->name);
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
				break;
			case IC_MUL_VAR:
				break;
			case IC_DIV_VAR:
				break;
			case IC_LT_VAR:
				break;
			case IC_GT_VAR:
				break;
			case IC_EQ_VAR:
				break;
			case IC_AND_VAR:
				break;
			case IC_OR_VAR:
				break;
			case IC_NOT_VAR:
				break;
			case IC_INT2FLOAT_VAR:
				break;
			case IC_FLOAT2INT_VAR:
				break;
			case IC_INT2CHAR_VAR:
				break;
			case IC_STR2INT_VAR:
				break;
			case IC_READ_VAR:
				break;
			case IC_WRITE_VAR:
				write_fun(*instr);
				break;
			case IC_CONCAT_STR:
				break;
			case IC_STRLEN_STR:
				break;
			case IC_GETCHAR_STR:
				break;
			case IC_SETCHAR_STR:
				break;
			case IC_IF_DEF:
				break;
			case IC_IF_START:
				break;
			case IC_IF_END:
				break;
			case IC_ELSE_START:
				break;
			case IC_ELSE_END:
				break;
			case IC_FOR_DEF:
				break;
			case IC_FOR_COND:
				break;
			case IC_FOR_STEP:
				break;
			case IC_FOR_BODY_START:
				break;
			case IC_FOR_BODY_END:
				break;
			default:
				error(99, "codegen.c", "codegen_generate_instr", "Invalid instruction");
				break;
		}
		instr = list_get_next(list);
	}
}

void codegen() {
	codegen_init();
	codegen_generate_instr();
}
