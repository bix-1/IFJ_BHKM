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

#define OUTPUT stdout

int fun_deep = 0;
char **current_fun = NULL;

void codegen_init() {
	fprintf(OUTPUT, ".IFJcode20\n\n");
}

void declr_var(instr_t instr) {
	elem_t *elem_dest = instr.elem_dest_ptr;

	char *frame_dest = NULL;

	if (elem_dest->sym_type != SYM_VAR_ITEM) {
		error(99, "codegen.c", "declr_var", "Invalid symbol type");
	}

	if (instr.elem_dest_ptr->symbol.sym_var_item->is_global) {
		frame_dest = "GF";
	}
	else {
		frame_dest = "LF";

		if (current_fun == NULL) {
			error(99, "codegen.c", "declr_var", "Undefined current function");
		}
	}

	fprintf(OUTPUT, "DEFVAR %s@%s", frame_dest, elem_dest->symbol.sym_var_item->name);
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

	while (sym_var_list_next(elem_dest->symbol.sym_var_list) != NULL
	       && sym_var_list_next(elem_1->symbol.sym_var_list) != NULL) {
		sym_var_item_t *sym_dest = sym_var_list_get_active(elem_dest->symbol.sym_var_list);
		sym_var_item_t *sym_elem1 = sym_var_list_get_active(elem_1->symbol.sym_var_list);

		char *frame_dest = NULL;
		char *frame_elem1 = NULL;

		if (sym_dest->is_global) {
			frame_dest = "GF";
		}
		else {
			frame_dest = "LF";
		}

		if (sym_elem1->is_global) {
			frame_elem1 = "GF";
		}
		else {
			frame_elem1 = "LF";
		}

		if (sym_dest->type != sym_elem1->type) {
			error(99, "codegen.c", "def_var", "Incompatible data types");
		}

		if (sym_dest->is_const) {
			switch (sym_dest->type) {
				case VAR_INT:
					fprintf(OUTPUT, "MOVE %s@%s int@%d", frame_dest, sym_dest->name, sym_elem1->data.int_t);
					break;
				case VAR_FLOAT64:
					fprintf(OUTPUT, "MOVE %s@%s float64@%a", frame_dest, sym_dest->name, sym_elem1->data.float64_t);
					break;
				case VAR_STRING:
					fprintf(OUTPUT, "MOVE %s@%s string@%s", frame_dest, sym_dest->name, sym_elem1->data.string_t);
					break;
				case VAR_BOOL:
					if (sym_elem1->data.bool_t) {
						fprintf(OUTPUT, "MOVE %s@%s bool@true", frame_dest, sym_dest->name);
					}
					else {
						fprintf(OUTPUT, "MOVE %s@%s bool@false", frame_dest, sym_dest->name);
					}
					break;
				case VAR_NIL:
					fprintf(OUTPUT, "MOVE %s@%s nil@nil", frame_dest, sym_dest->name);
					break;
			}
		}
		else {
			fprintf(OUTPUT, "MOVE %s@%s %s@%s", frame_dest, sym_dest->name, frame_elem1, sym_elem1->name);
		}
	}

}

void codegen_generate_instr() {
	instr_t *instr = list->first;

	if (instr == NULL) {
		error(99, "codegen.c", "codegen_generate_instr", "Empty instructions list");
	}

	while (instr != NULL) {
		// switch
		switch (instr->type) {
			case IC_DECL_VAR:
				declr_var(*instr);
				break;
			case IC_DEF_VAR:
				def_var(*instr);
				break;
			case IC_DEF_FUN:
				break;
			case IC_CALL_FUN:
				break;
			case IC_RET_FUN:
				break;
			case IC_ADD_VAR:
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
	}
}

void code_gen() {
	codegen_init();
	codegen_generate_instr();
}
