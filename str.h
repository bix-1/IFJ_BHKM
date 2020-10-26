/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: str.h
 * Brief: Dynamic string file
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#ifndef IFJ_BHKM_STR_H
#define IFJ_BHKM_STR_H

#define STR_LEN_INC 8 // initial allocated size for char

typedef struct {
    char *str;      // pointer to string ended with '\0'
    int str_lenght; // string lenght
    int str_alloc_size; // size of allocated memory
} string;

/*
 * Function creates new string.
 */
int str_init(string *s);

/*
 * Function frees allocated memory
 */
void str_free(string *s);

/*
 * Function clears string array.
 */
void str_clear(string *s);

/*
 * Function adds new char intostring array.
 */
int str_add_char(string *s, char c);

/*
 * Function copies second string into the first.
 */
int str_copy(string *s1, string *s2);

/*
 * Function compares string with constant.
 */
int str_cmp_cons(string *s, char *s_const);

/*
 * Function comapares two strings.
 */
int str_cmp(string *s1, string *s2);

#endif //IFJ_BHKM_STR_H
