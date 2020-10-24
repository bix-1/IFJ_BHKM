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
    char *endptr; // auxiliary variable for strtod function
    Token token;

    if (str_init(token.attr.str_lit) == 1) {
        return I_ERROR;
    }

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
                } else if (c == '"') {
                    str_add_char(attr, c); //save first char
                    scanner_state = s_string_tmp;
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
                        ungetc(c, source);
                        token.token_type = T_ASSIGNMENT;
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
            case s_identifier: //f15
                // we are filling our string with indentifier
                if (isalnum(c) || c == '_') {
                    str_add_char(attr, c);
                } else {
                    if (str_init(token.attr.str_lit) == 1) {
                        return I_ERROR;
                    }

                    ungetc(c, source); // we read all and we need to go one char back

                    // we check for keyword, if there is one we do not treat him as identifier, and
                    // we store his token and his string attribute
                    if (str_cmp_cons(attr, "else") == 0) {
                        token.token_type = T_ELSE;
                        str_copy(token.attr.str_lit, attr);
                    } else if (str_cmp_cons(attr, "float64") == 0) {
                        token.token_type = T_FLOAT64;
                        str_copy(token.attr.str_lit, attr);
                    } else if (str_cmp_cons(attr, "for") == 0) {
                        token.token_type = T_FOR;
                        str_copy(token.attr.str_lit, attr);
                    } else if (str_cmp_cons(attr, "func") == 0) {
                        token.token_type = T_FUNC;
                        str_copy(token.attr.str_lit, attr);
                    } else if (str_cmp_cons(attr, "if") == 0) {
                        token.token_type = T_IF;
                        str_copy(token.attr.str_lit, attr);
                    } else if (str_cmp_cons(attr, "int") == 0) {
                        token.token_type = T_INT;
                        str_copy(token.attr.str_lit, attr);
                    } else if (str_cmp_cons(attr, "package") == 0) {
                        token.token_type = T_PACKAGE;
                        str_copy(token.attr.str_lit, attr);
                    } else if (str_cmp_cons(attr, "return") == 0) {
                        token.token_type = T_RETRUN;
                        str_copy(token.attr.str_lit, attr);
                    } else if (str_cmp_cons(attr, "string") == 0) {
                        token.token_type = T_STRING;
                        str_copy(token.attr.str_lit, attr);
                    } else {
                        token.token_type = T_IDENTIFIER;
                        str_copy(token.attr.str_lit, attr);
                    }

                    str_free(attr);
                    return L_SUCCESS;
                }
                break;
            case s_int_lit: //f16
                if (isdigit(c)) {
                    str_add_char(attr, c);
                } else if (c == 'e' || c == 'E') {
                    str_add_char(attr, c);
                    scanner_state = s_exp_tmp;
                } else if (c == '.') {
                    str_add_char(attr, c);
                    scanner_state = s_decimal_tmp;
                } else {
                    ungetc(c, source);
                    token.token_type = T_INT;
                    token.attr.int_lit = atoi(attr->str); // we dont need to check for conversion errors,
                    //because we know that just integer number literal
                }
                break;
            case s_exp_tmp: //q18
                if(isdigit(c)){
                    str_add_char(attr, c);
                    scanner_state = s_decimal_lit;
                } else if (c == '+' || c == '-'){
                    scanner_state = s_exp_sig_tmp;
                } else {
                    ungetc(c, source);
                    str_free(attr);
                    return L_ERROR;
                }
                break;
            case s_exp_sig_tmp: //r18
                if(isdigit(c)){
                    str_add_char(attr, c);
                    scanner_state = s_exp_lit;
                } else {
                    ungetc(c, source);
                    str_free(attr);
                    return L_ERROR;
                }
                break;
            case s_decimal_tmp: //q17
                if(isdigit(c)){
                    str_add_char(attr, c);
                    scanner_state = s_decimal_lit;
                } else {
                    ungetc(c, source);
                    str_free(attr);
                    return L_ERROR;
                }
                break;
            case s_decimal_lit: //f17
                if (isdigit(c)) {
                    str_add_char(attr, c);
                } else if (c == 'e' || c == 'E') {
                    str_add_char(attr, c);
                    scanner_state = s_exp_tmp;
                } else {
                    ungetc(c, source);
                    token.token_type = T_FLOAT64;
                    token.attr.dec_lit = strtod(attr->str, &endptr);
                }
                break;
            case s_exp_lit: //f18
                if(isdigit(c)){
                    str_add_char(attr, c);
                } else if (c == '+' || c == '-'){
                    str_add_char(attr, c);
                    scanner_state = s_decimal_lit;
                } else {
                    ungetc(c, source);
                    token.token_type = T_FLOAT64;
                    token.attr.dec_lit = strtod(attr->str, &endptr);
                }
                break;
            case s_string_tmp:
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