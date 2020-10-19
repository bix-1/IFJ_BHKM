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

int str_init(string *s);
void str_free(string *s);
void str_clear(string *s);
int str_add_char(string *s, char c);
int str_copy(string *s1, string *s2);
int str_cmp_cons(string *s, char *s_const);
int str_cmp(string *s1, string *s2);
#endif //IFJ_BHKM_STR_H
