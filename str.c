/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: str.c
 * Brief: Implementation of dynamic string.
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "str.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

void str_init(string *s) {
    //allocates string to 1 char size
    s->str = (char *) malloc(STR_LEN_INC);

    if (s->str == NULL) {
        error(99,"str.c", "str_init(string *s)", "Internal error");
    }

    s->str[0] = '\0';
    s->str_lenght = 0;
    s->str_alloc_size = 0;
}

void str_free(string *s) {
    free(s->str);
}

void str_clear(string *s) {
    s->str[0] = '\0';
    s->str_lenght = 0;
}

void str_add_char(string *s, char c) {
    if (s->str_lenght + 1 >= s->str_alloc_size) {
        //if there is not enoguh memory, we reallocate
        s->str = (char *) realloc(s->str, s->str_lenght + STR_LEN_INC);

        if (s->str == NULL) {
            error(99,"str.c", "str_init(string *s)", "Internal error");
        }

        s->str_alloc_size = s->str_lenght + STR_LEN_INC;
    }
    //adds char and increments lenght for '\0'
    s->str[s->str_lenght] = c;
    s->str_lenght++;
    s->str[s->str_lenght] = '\0';
}

/*
 * Function copies second string into the first.
 */
void str_copy(string *s1, string *s2) {
    int new_str_lenght = s2->str_lenght;
    //if there is not enoguh memory, we reallocate
    if (new_str_lenght >= s1->str_alloc_size) {
        s1->str = (char *) realloc(s1->str, new_str_lenght + 1);

        if (s1->str == NULL) {
            error(99,"str.c", "str_init(string *s)", "Internal error");
        }

        s1->str_alloc_size = new_str_lenght + 1;
    }
    //copies string and and changes new lenght
    strcpy(s1->str, s2->str);
    s1->str_lenght = new_str_lenght;
}

int str_cmp_cons(string *s, char *s_const) {
    return strcmp(s->str, s_const);
}

int str_cmp(string *s1, string *s2) {
    return strcmp(s1->str, s2->str);
}
