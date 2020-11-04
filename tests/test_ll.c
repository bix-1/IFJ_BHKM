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

	list_t * l = list_create();
	printf( "\nSize of empty list: %d\n", list_size(l) );
	printf( "Adding one instruction...\n" );
	instr_t * i = instr_create();
	instr_set_type( i, 1 );
	list_add( l, i );
	printf( "Size of list: %d\n\n", list_size(l) );

	printf( "Adding 1st element...\n" );
	instr_add_elem( i, "first_elem" );
	printf( "The 1st elem now is: %s\n", instr_get_elem1(i) );
	if ( instr_get_elem2(i) == NULL )
		printf( "2nd element is empty...\n" );
	printf( "Adding 2nd element...\n" );
	instr_add_elem( i, "second_elem" );
	if ( instr_get_elem1(i) != NULL && instr_get_elem2(i) != NULL )
		printf(
			"Elements of instruction now are: %s & %s\n\n",
			instr_get_elem1(i), instr_get_elem2(i)
		);

	instr_t * j = instr_create();
	instr_set_type( j, 2 );
	instr_add_elem( j, "third_elem" );
	instr_add_elem( j, "fourth_elem" );
	list_add( l, j );
	printf( "Contents of list:\n" );
	int index = 0;
	for ( instr_t * tmp = list_get_active(l); tmp != NULL; tmp = list_next(l) )
	{
		printf( "Instr %d\n", ++index );
		printf( "\ttype: %d\n", instr_get_type(tmp) );
		printf( "\telem1: %s\n", instr_get_elem1(tmp) );
		printf( "\telem2: %s\n", instr_get_elem2(tmp) );
	}

	list_destroy( &l );
	if ( l == NULL )
		printf( "\nList destroyed\n\n" );

	return 0;
}
