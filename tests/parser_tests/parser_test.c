/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: error.c
 * Brief: Implementation of functions handling errors
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "str.h"
#include "parser.h"
#include <stdlib.h>

FILE *f;
extern int line_num;

void print_lex(tToken *token) {
    if (token->token_type == T_INT) {
        printf("Lexema : %d, ma hodnotu: %d", token->token_type, token->attr.int_lit);
    } else if (token->token_type == T_FLOAT64) {
        printf("Lexema : %d, ma hodnotu: %f", token->token_type, token->attr.dec_lit);
    } else if (token->token_type == T_STRING || token->token_type == T_ELSE || token->token_type == T_IDENTIFIER ||
            token->token_type == T_IF || token->token_type == T_PACKAGE || token->token_type == T_RETURN ||
            token->token_type == T_FUNC || token->token_type == T_FOR) {
        printf("Lexema : %d, ma hodnotu: %s", token->token_type, token->attr.str_lit);
    } else {
        printf("Lexema : %d, ma hodnotu: %d", token->token_type, token->attr.int_lit);
    }
}

void f_open(char *file) {
    f = fopen(file, "r");
    if (f == NULL) {
        fprintf(stderr, "Nepodarilo sa otvorit subor");
        return;
    }

    source_file_setup(f);
}
int main() {
    printf("[TEST01]\n");
    printf("Token pre integer literal\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/kostyo/3sm/ifj/IFJ_BHKM/tests/parser_tests/int_lit_test");
    //tToken tok;
    //get_next_token(&tok);
    //print_lex(&tok);
    parse();
    fclose(f);
}

