/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: test_ll.c
 * Brief: Tests of module ll.c
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */


#include <stdio.h>
#include "../ll.h"

int main() {
	// Error tests (removed from final implementation)
	// List * l_err = list_create();
	Instruction * i_err = instr_create();

	// List * l = list_create();
	// list_destroy( l );

	return 0;
}
