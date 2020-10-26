#include <stdio.h>
#include "../ll.h"

int main() {
	List * l = list_create();
	list_destroy( l );

	return 0;
}
