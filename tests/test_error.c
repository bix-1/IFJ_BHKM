#include <stdio.h>
#include "../ll.h"
#include "../error.h"
#include "../symtable.h"

int main ()
{
	// testing ll and symtable cleanup
	list = list_create();
	symtable = symtable_init(1);

	// error(99, NULL, NULL, NULL);
	const char s[] = "msg";
	error(99, "test_error.c", "main", "Test error %s", s);

	// check results using valgrind
	return 0;
}
