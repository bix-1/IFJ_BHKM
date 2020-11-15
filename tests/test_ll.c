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
	// instr_t * i_err = instr_create();

	symtable = symtable_init(10);
	list = list_create();

	list_t * list = list_create();
	if (list_is_empty(list))
		printf( "\nSize of empty list: %d\n", list_size(list) );
	else {
		fprintf(stderr, "---FAILED\n");
		exit(1);
	}
	printf( "Adding instruction...\n" );
	instr_t * i = instr_create();
	instr_set_type( i, 1 );
	list_add( list, i );
	printf( "Size of list: %d\n\n", list_size(list) );

	printf("Adding dest...\n");
	// sym_var_item_t d1 = {.name="d1", .type=VAR_STRING, .};
	// symbol_t sym_dest1 = {.sym_var_item = &d1};
	// elem_t * dest1 = elem_init(SYM_CONST, NULL);
	elem_t dest1;
	symtable_insert(symtable, "dest1", &dest1);
	instr_add_dest(i, &dest1);
	printf( "The dest now is: %s\n", *(instr_get_dest(i)->key) );

	printf( "Adding 1st element...\n" );
	elem_t e1;
	symtable_insert(symtable, "elem1", &e1);
	instr_add_elem1(i, &e1);


	printf( "The 1st elem now is: %s\n", *(instr_get_elem1(i)->key) );
	if ( instr_get_elem2(i) == NULL )
		printf( "2nd element is empty...\n" );
	printf( "Adding 2nd element...\n" );
	elem_t e2;
	symtable_insert(symtable, "elem2", &e2);
	instr_add_elem2(i, &e2);
	if ( instr_get_elem1(i) != NULL && instr_get_elem2(i) != NULL )
		printf(
			"Elements of instruction now are: %s & %s\n\n",
			*(instr_get_elem1(i)->key), *(instr_get_elem2(i)->key)
		);
	else {
		fprintf(stderr, "---FAILED\n");
		exit(1);
	}

	printf( "\nAdding 2nd instruction...\n" );
	instr_t * j = NULL;
	instr_init(&j, 2);
	list_add( list, j );
	elem_t dest2, e3, e4;
	symtable_insert(symtable, "dest2", &dest2);
	symtable_insert(symtable, "elem3", &e3);
	symtable_insert(symtable, "elem4", &e4);
	instr_add_dest(j, &dest2);
	instr_add_elem1(j, &e3);
	instr_add_elem2(j, &e4);

	printf( "Contents of list:\n" );
	int index = 0;
	for ( instr_t * tmp = list_get_active(list); tmp != NULL; tmp = list_get_next(list) )
	{
		printf( "Instr %d\n", ++index );
		printf( "\ttype: %d\n", instr_get_type(tmp) );
		printf( "\tdest:\t%s\n", *(instr_get_dest(tmp)->key) );
		printf( "\telem1:\t%s\n", *(instr_get_elem1(tmp)->key) );
		printf( "\telem2:\t%s\n", *(instr_get_elem2(tmp)->key) );
	}

	list_destroy( &list );
	if ( list == NULL )
		printf( "\nList destroyed\n\n" );
	else {
		fprintf(stderr, "---FAILED\n");
		exit(1);
	}

	// symtable_free(symtable);

	return 0;
}
