#include <stdio.h>
#include "../ll.h"
#include "../error.h"

int main ()
{
	// testing ll and htab cleanup
	list = list_create();
	htab = htab_init();

	error( 99, NULL, NULL, NULL );
	// check results using valgrind

	return 0;
}
