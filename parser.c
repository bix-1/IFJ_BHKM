/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: parser.c
 * Brief: Implementation of parser
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "parser.h"
#include <stdio.h>


void parse()
{
    parsData.stack = (tokenStack *)malloc(sizeof(struct tokenStack));
    parsData.token = (tToken*)malloc(sizeof( tToken));
    stack_init(parsData.stack);

    printf("PARS\t%d\t%d\n", parsData.stack->topToken->token.token_type);
    
    
    parse_expression();

    stack_free(parsData.stack);
    free(parsData.stack);
    free(parsData.token);
}
