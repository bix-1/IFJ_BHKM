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

int line_num = 0; // line count for accurate error message

void source_file_setup(FILE *f) {
    source = f;
}

void char_clear(string *attr, int c) {
    ungetc(c, source);
    str_free(attr);
}

int get_next_token(string *attr) {
    int scanner_state; // variable declaration for switch
    int c; // variable for input char
    char *endptr; // auxiliary variable for strtod and strtol function
    char hex[2]; // array that stores hex number for conversion
    Token token;

    if (str_init(attr) == 1) {
        return I_ERROR;
    }

    str_clear(attr); // clears everything in string, if it is identifier we start storing data

    if (source == NULL) {
        return I_ERROR;
    }

    scanner_state = s_start; // we set initial scanner state

    while (1) {
        //we get another char from our source file
        c = getc(source);
        // calculating number of
        if (c == '\n') {
            line_num++;
        }

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
                    // we dont save first char '"', because we dont want it in the string attribute
                    scanner_state = s_string_tmp;
                } else if (c == '/') {
                    // we load another char so we can decide, whether it is comment or div
                    c = getc(source);

                    if (c == '/') {
                        scanner_state = s_line_c;
                    } else if (c == '*') {
                        scanner_state = s_block_c;
                    } else {
                        token.token_type = T_DIV;
                        char_clear(attr, c);
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
                        char_clear(attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == '!') {
                    // we check for not eq operator otherwise it is error
                    c = getc(source);

                    if (c == '=') {
                        token.token_type = T_NEQ;
                        str_free(attr);
                        return L_SUCCESS;
                    }

                    char_clear(attr, c);
                    return L_ERROR;
                } else if (c == '>') {
                    c = getc(source);

                    if (c == '=') {
                        token.token_type = T_GREATER_EQ;
                        str_free(attr);
                        return L_SUCCESS;
                    } else {
                        token.token_type = T_GREATER;
                        char_clear(attr, c);
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
                        char_clear(attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == EOF) {
                    token.token_type = T_EOF;
                    str_free(attr);
                    return L_SUCCESS;
                } else {
                    char_clear(attr, c);
                    return L_ERROR;
                }
                break;
            case s_identifier: //f151
                // we are filling our string with indentifier
                if (isalnum(c) || c == '_') {
                    str_add_char(attr, c);
                } else {
                    // we initialize string attribute for identifier
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
                    token.attr.int_lit = atoi(attr->str);
                    // we dont need to check for conversion errors,
                    //because we know that just integer number literal
                }
                break;
            case s_exp_tmp: //q18
                if (isdigit(c)) {
                    str_add_char(attr, c);
                    scanner_state = s_decimal_lit;
                } else if (c == '+' || c == '-') {
                    scanner_state = s_exp_sig_tmp;
                } else {
                    char_clear(attr, c);
                    return L_ERROR;
                }
                break;
            case s_exp_sig_tmp: //r18
                if (isdigit(c)) {
                    str_add_char(attr, c);
                    scanner_state = s_exp_lit;
                } else {
                    ungetc(c, source);
                    str_free(attr);
                    return L_ERROR;
                }
                break;
            case s_decimal_tmp: //q17
                if (isdigit(c)) {
                    str_add_char(attr, c);
                    scanner_state = s_decimal_lit;
                } else {
                    char_clear(attr, c);
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
                    // conversion check
                    if (*endptr != 0){
                        char_clear(attr, c);
                        return L_ERROR;
                    }
                }
                break;
            case s_exp_lit: //f18
                if (isdigit(c)) {
                    str_add_char(attr, c);
                } else {
                    ungetc(c, source);
                    token.token_type = T_FLOAT64;
                    token.attr.dec_lit = strtod(attr->str, &endptr);
                    // conversion check
                    if (*endptr != 0){
                        char_clear(attr, c);
                        return L_ERROR;
                    }
                }
                break;
            case s_line_c: //p2
                if (c == EOF || c == '\n') {
                    ungetc(c, source);
                    scanner_state = s_start;
                }
                break;
            case s_block_c: //p1
                if (c == '*') {
                    c = getc(source);

                    if (c == '/') {
                        scanner_state = s_start;
                    } else {
                        ungetc(c, source);
                    }
                } else if (c == EOF) { // block comment without end
                    char_clear(attr, c);
                    return L_ERROR;
                }
                break;
            case s_string_tmp: //q19
                if (c > 31) {
                    if (c == '"') {
                        scanner_state = s_string;
                    } else if (c == '\\') {
                        scanner_state = s_esc_seq;
                    } else {
                        str_add_char(attr, c);
                    }
                } else {
                    char_clear(attr, c);
                    return L_ERROR;
                }
                break;
            case s_esc_seq: //r19
                if (c == '\\') {
                    str_add_char(attr, '\\');
                    scanner_state = s_string_tmp;
                } else if (c == 't') {
                    str_add_char(attr, '\t');
                    scanner_state = s_string_tmp;
                } else if (c == 'n') {
                    str_add_char(attr, '\n');
                    scanner_state = s_string_tmp;
                } else if (c == 'x') {
                    scanner_state = s_hex_tmp;
                } else if (c == '"') {
                    str_add_char(attr, '\"');
                    scanner_state = s_string_tmp;
                } else {
                    ungetc(c, source);
                    str_free(attr);
                    return L_ERROR;
                }
                break;
            case s_hex_tmp: //t19
                if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' ||
                    c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
                    isdigit(c)) {
                    hex[0] = c; // stroring first hex number
                    scanner_state = s_hex_num;
                } else {
                    char_clear(attr, c);
                    return L_ERROR;
                }
                break;
            case s_hex_num: //u19
                if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' ||
                    c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
                    isdigit(c)) {
                    long pars_tmp; // temp variable for strtol function
                    hex[1] = c; // storing second hex number
                    pars_tmp = strtol(hex, &endptr, 16); // converting hex number into decimal number
                    c = (int) pars_tmp;
                    // conversion check
                    if (*endptr != 0){
                        char_clear(attr, c);
                        return L_ERROR;
                    }

                    if (c < 32) {
                        char_clear(attr, c);
                        return L_ERROR;
                    }
                    // we are storing converted hex number as char into our string literal
                    str_add_char(attr, c);
                    scanner_state = s_string_tmp;
                } else {
                    char_clear(attr, c);
                    return L_ERROR;
                }
                break;
            case s_string: //f19
                ungetc(c, source);
                // we initialize string attribute for string
                if (str_init(token.attr.str_lit) == 1) {
                    return I_ERROR;
                }

                token.token_type = T_STRING;
                str_copy(token.attr.str_lit, attr);
                str_free(attr);
                break;
        }
    }
}