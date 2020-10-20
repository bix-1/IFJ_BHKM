/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: scanner.c
 * Brief: Implementation of scanner.
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include "scanner.h"

void source_file_setup(FILE *f) {
    source = f;
}

int get_next_token(string *attr) {
    int scanner_state = s_start; // we set initial scanner state
    int c;
    Token token;

    str_clear(attr); // deleting everything in string, if it is identifier we start storing data

    while (1) {
        //we get another char from our source file
        c = getc(source);

        switch (scanner_state) {
            case s_start:
                if (isspace(c)) {
                    scanner_state = s_start;
                } else if (isalpha(c) || c == '_') {
                    str_add_char(attr, c); //save first char
                    scanner_state = s_identifier;
                } else if (isdigit(c)) {
                    str_add_char(attr, c); //save first char
                    scanner_state = s_int_lit;
                } else if (c == '/') {
                    c = getc(source);
                    if (c == '/') {
                        scanner_state = s_line_c;
                    } else if (c == '*') {
                        scanner_state = s_block_c;
                    } else {
                        token.token_type = T_DIV;
                        ungetc(c, source);
                        str_free(attr);
                        return L_SUCCESS;
                    }
                } else if (c == ',') {
                    token.token_type = T_COMMA;
                    str_free(attr);
                    return L_SUCCESS;
                } else if (c == '+') {
                    token.token_type = T_PLUS;
                    str_free(attr);
                    return L_SUCCESS;
                } else if (c == '-') {
                    token.token_type = T_MINUS;
                    str_free(attr);
                    return L_SUCCESS;
                } else if (c == '*') {
                    token.token_type = T_MUL;
                    str_free(attr);
                    return L_SUCCESS;
                } else if (c == '(') {
                    token.token_type = T_L_BRACKET;
                    str_free(attr);
                    return L_SUCCESS;
                } else if (c == ')') {
                    token.token_type = T_R_BRACKET;
                    str_free(attr);
                    return L_SUCCESS;
                } else if (c == '=') {
                    c = getc(source);

                    if (c == '=') {
                        token.token_type = T_EQ;
                        str_free(attr);
                        return L_SUCCESS;
                    } else {
                        token.token_type = T_ASSIGNMENT;
                        ungetc(c, source);
                        str_free(attr);
                        return L_SUCCESS;
                    }
                } else if (c == '!') {
                    c = getc(source);

                    if (c == '=') {
                        token.token_type = T_NEQ;
                        str_free(attr);
                        return L_SUCCESS;
                    }

                    ungetc(c, source);
                    str_free(attr);
                    return L_ERROR;
                } else if (c == '>') {
                    c = getc(source);

                    if (c == '=') {
                        token.token_type = T_GREATER_EQ;
                        str_free(attr);
                        return L_SUCCESS;
                    } else {
                        token.token_type = T_GREATER;
                        ungetc(c, source);
                        str_free(attr);
                        return L_SUCCESS;
                    }
                } else if (c == '<') {
                    c = getc(source);

                    if (c == '=') {
                        token.token_type = T_LESS_EQ;
                        str_free(attr);
                        return L_SUCCESS;
                    } else {
                        token.token_type = T_LESS;
                        ungetc(c, source);
                        str_free(attr);
                        return L_SUCCESS;
                    }
                } else if (c == EOF) {
                    token.token_type = T_EOF;
                    str_free(attr);
                    return L_SUCCESS;
                } else {
                    ungetc(c, source);
                    str_free(attr);
                    return L_ERROR;
                }
                break;
            case s_identifier:
                if (isalnum(c) || c == '_') {
                    str_add_char(attr, c);
                } else {
                    if (str_cmp_cons(attr, "else") == 0) {
                        token.token_type = T_ELSE;
                    } else if (str_cmp_cons(attr, "float64") == 0) {
                        token.token_type = T_FLOAT64;
                    } else if (str_cmp_cons(attr, "for") == 0) {
                        token.token_type = T_FOR;
                    } else if (str_cmp_cons(attr, "func") == 0) {
                        token.token_type = T_FUNC;
                    } else if (str_cmp_cons(attr, "if") == 0) {
                        token.token_type = T_IF;
                    } else if (str_cmp_cons(attr, "int") == 0) {
                        token.token_type = T_INT;
                    } else if (str_cmp_cons(attr, "package") == 0) {
                        token.token_type = T_PACKAGE;
                    } else if (str_cmp_cons(attr, "return") == 0) {
                        token.token_type = T_RETRUN;
                    } else if (str_cmp_cons(attr, "string") == 0) {
                        token.token_type = T_STRING;
                    } else {
                        token.token_type = T_IDENTIFIER;
                    }
                    str_free(attr);
                    return L_SUCCESS;
                }
                break;
            case s_int_lit:
                //TODO
                break;
            case s_line_c:
                //TODO
                break;
            case s_block_c:
                //TODO
                break;
        }
    }
}