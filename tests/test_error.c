#include <stdio.h>
#include "../ll.h"
#include "../error.h"
#include "../htab.h"

int main ()
{
	// testing ll and htab cleanup
	list = list_create();
	htab = htab_init(1);

	error( 99, NULL, NULL, NULL );
	// check results using valgrind

	return 0;
}
