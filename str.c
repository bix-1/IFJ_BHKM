/*
 * Purpose: Compiler implementation of imperative programing language IFJ20
 *
 * Authors: Hladký Tomáš	xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam	xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef	    xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub	xbartk07@stud.fit.vutbr.cz
 */

#include "str.h"
#include "stdlib.h"
#include "string.h"

/*
 * Function creates new string
 */
int str_init(string *s){
    s->str = (char *) malloc(STR_LEN_INC);

    if(s->str == NULL){
        return 1;
    }

    s->str[0] = '\0';
    s->str_lenght = 0;
    s->alloc_size = 0;
}

/*
 * Function frees allocated memory
 */
void str_free(string *s){
    free(s->str);
}

/*
 * Function clears string array
 */
void str_clear(string *s){
    s->str[0] = '\0';
    s->str_lenght = 0;
}

/*
 * Function adds new char intostring array
 */
int str_add_char(string *s1, char c){
    if(s1->str_lenght + 1 >= s1->alloc_size){
        //if there is not enoguh memory, we reallocate
        s1->str = (char *) realloc(s1->str, s1->str_lenght + STR_LEN_INC);

        if(s1->str == NULL){
            return 1;
        }

        s1->alloc_size = s1->str_lenght + STR_LEN_INC;
    }
    //adds char and increments lenght for '\0'
    s1->str[s1->str_lenght] = c;
    s1->str_lenght++;
    s1->str[s1->str_lenght] = '\0';
    return 0;
}

/*
 * Function copies second string into the first
 */
int str_copy(string *s1, string *s2){
    int new_str_lenght = s2->str_lenght;
    //if there is not enoguh memory, we reallocate
    if(new_str_lenght >= s1->alloc_size){
        s1->str = (char *) realloc(s1->str, new_str_lenght + 11);

        if(s1->str == NULL){
            return 1;
        }

        s1->alloc_size = new_str_lenght + 1;
    }
    //copies string and and changes new lenght
    strcpy(s1->str, s2->str);
    s1->str_lenght = new_str_lenght;
    return 0;
}

/*
 * Function compares string with constant
 */
int str_cmp_cons(string *s1, char *s2){
    return strcmp(s1->str, s2);
}

/*
 * Function comapares two strings
 */
int str_cmp(string *s1, string *s2){
    return strcmp(s1->str, s2->str);
}