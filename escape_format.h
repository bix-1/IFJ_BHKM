/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: escape_format.c
 * Brief: Header file for escape_format.c
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */
#ifndef ESCAPE_FORMAT_H
#define ESCAPE_FORMAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

void merge_str(char *str, int start_i, int move);
char *escape_reformat(char *str);

#endif //ESCAPE_FORMAT_H
