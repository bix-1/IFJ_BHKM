/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: escape_format.c
 * Brief: Reformat escape sequence to format \xyz
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "escape_format.h"

/*
 * Merge end of input string from start_i by move positions
 */
void merge_str(char *str, int start_i, int move) {
	if (str == NULL) {
		error(99, "codegen.c", "reformat", "NULL pointer");
	}

	size_t size = strlen(str);

	for (int i = size; i >= start_i ; i--) {
		str[i + move] = str[i];
	}
}

/*
 * Reformat string to match \xyz escape sequence
 * eg. "Hello\tWorld\n" => "Hello\009World\010"
 */
char *escape_reformat(char *string) {
	size_t size = strlen(string);
	char *copy = malloc(size + 1);

	if (copy == NULL) {
		error(99, "codegen.c", "reformat", "NULL pointer");
	}

	strcpy(copy, string);

	if (copy == NULL) {
		error(99, "codegen.c", "reformat", "NULL pointer");
	}

	for (int i = 0; i < size; ++i) {
		if ((copy[i] >= 0 && copy[i] <= 32) || copy[i] == 35 || copy[i] == 92) {
			// +3 extra chars \n => \010
			// +1 null char on end
			char *temp = malloc(sizeof(char) * size + 1);

			if (temp == NULL) {
				error(99, "codegen.c", "reformat", "NULL pointer");
			}

			strcpy(temp, copy);
			copy = realloc(copy, strlen(copy) + 3 + 1);

			if (copy == NULL) {
				error(99, "codegen.c", "reformat", "NULL pointer");
			}

			memset(copy, 0, size + 3);
			strncpy(copy, temp, size);
			size += 3;
			free(temp);

			merge_str(copy, i + 1, 3);

			char sequence[5];
			sprintf(sequence, "\\%03d", copy[i]);
			strncpy(&copy[i], sequence, 4);
		}
	}
	return copy;
}
