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

#include "scanner.h"
#include "str.h"

FILE *f;
extern int line_num;

void print_lex(tToken *token) {
    if (token->token_type == T_INT) {
        printf("Lexema : %d, ma hodnotu: %d \n\n", token->token_type, token->attr.int_lit);
    } else if (token->token_type == T_FLOAT64) {
        printf("Lexema : %d, ma hodnotu: %f \n\n", token->token_type, token->attr.dec_lit);
    } else if (token->token_type == T_STRING || token->token_type == T_ELSE || token->token_type == T_IDENTIFIER ||
            token->token_type == T_IF || token->token_type == T_PACKAGE || token->token_type == T_RETURN ||
            token->token_type == T_FUNC || token->token_type == T_FOR) {
        printf("Lexema : %d, ma hodnotu: %s \n\n", token->token_type, token->attr.str_lit.str);
    } else {
        printf("Lexema : %d, ma hodnotu: %d \n\n", token->token_type, token->attr.int_lit);
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
    tToken tok;


    printf("[TEST01]\n");
    printf("Token pre integer literal : 118, value : 6542\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/int_lit_test");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST02]\n");
    printf("Token plus : 100\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/plus");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST03]\n");
    printf("Token minus : 101\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/minus");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST04]\n");
    printf("Token mul : 102\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/multiplication");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST05]\n");
    printf("Token division : 103\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/division");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST06]\n");
    printf("Token right_bracket : 113\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/right_bracket");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST07]\n");
    printf("Token left_bracket : 112\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/left_bracket");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST08]\n");
    printf("Token equator : 110\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/equator");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST09]\n");
    printf("Token assignment : 108\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/assignment");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST10]\n");
    printf("Token comma : 111\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/comma");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST11]\n");
    printf("Token not_eq : 109\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/not_eq");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST11]\n");
    printf("Token greater : 106\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/greater");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST12]\n");
    printf("Token greater_eq : 107\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/greater_eq");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST13]\n");
    printf("Token less : 104\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/less");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST14]\n");
    printf("Token less_eq : 105\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/less_eq");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST14]\n");
    printf("Token dec_num : 119, value : 42,42\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/dec_num");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);

    printf("[TEST14]\n");
    printf("Token string_abc : 120, value : abc\n");
    printf("~~~~~~~~~~~~~~~~~~~~\n");
    f_open("/home/jozef/3sm/ifj/IFJ_BHKM/tests/scanner_tests/string_abc");
    get_next_token(&tok);
    print_lex(&tok);
    fclose(f);
}