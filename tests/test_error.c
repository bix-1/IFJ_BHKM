#include <stdio.h>
#include "../ll.h"
#include "../error.h"
#include "../symtable.h"

int main ()
{
	// testing ll and htab cleanup
	list = list_create();
	symtable = htab_init(1);

	error( 99, NULL, NULL, NULL );
	// check results using valgrind

	return 0;
}
