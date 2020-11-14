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

	symtable = symtable_init(100);

	list_t * l = list_create();
	printf( "\nSize of empty list: %d\n", list_size(l) );
	printf( "Adding one instruction...\n" );
	instr_t * i = instr_create();
	instr_set_type( i, 1 );
	list_add( l, i );
	printf( "Size of list: %d\n\n", list_size(l) );

	printf( "Adding 1st element...\n" );
	instr_add_elem1( i, "first_elem" );
	elem_t e1;
	e1.key = i->elem1_key;
	symtable_lookup_add(symtable, i->elem1_key, e1);


	printf( "The 1st elem now is: %s\n", instr_get_elem1(i)->key );
	if ( instr_get_elem2(i) == NULL )
		printf( "2nd element is empty...\n" );
	printf( "Adding 2nd element...\n" );
	instr_add_elem2( i, "second_elem" );
	elem_t e2;
	e2.key = i->elem2_key;
	symtable_lookup_add(symtable, i->elem2_key, e2);
	if ( instr_get_elem1(i) != NULL && instr_get_elem2(i) != NULL )
		printf(
			"Elements of instruction now are: %s & %s\n\n",
			instr_get_elem1(i)->key, instr_get_elem2(i)->key
		);
	else {
		fprintf(stderr, "---FAILED\n");
		exit(1);
	}

	instr_t * j = instr_create();
	instr_set_type( j, 2 );
	instr_add_elem1( j, "third_elem" );
	instr_add_elem2( j, "fourth_elem" );
	elem_t e3, e4;
	e3.key = j->elem1_key;
	e4.key = j->elem2_key;
	symtable_lookup_add(symtable, j->elem1_key, e3);
	symtable_lookup_add(symtable, j->elem2_key, e4);
	list_add( l, j );

	printf( "Contents of list:\n" );
	int index = 0;
	for ( instr_t * tmp = list_get_active(l); tmp != NULL; tmp = list_get_next(l) )
	{
		printf( "Instr %d\n", ++index );
		printf( "\ttype: %d\n", instr_get_type(tmp) );
		printf( "\telem1: %s\n", instr_get_elem1(tmp)->key );
		printf( "\telem2: %s\n", instr_get_elem2(tmp)->key );
	}

	list_destroy( &l );
	if ( l == NULL )
		printf( "\nList destroyed\n\n" );
	else {
		fprintf(stderr, "---FAILED\n");
		exit(1);
	}
	symtable_free(symtable);

	return 0;
}
