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

#include "error.h"
#include "scanner.h"
#include <ctype.h>
#include <stdlib.h>


int line_num = 0; // line count for accurate error message

int get_err_line () {
  return line_num;
}

void source_file_setup(FILE *f) {
    source = f;
}

void char_clear(string *attr, int c) {
    ungetc(c, source);
    str_free(attr);
}

int get_next_token(tToken *token) {
    int scanner_state; // variable declaration for switch
    int c; // variable for input char
    char *endptr; // auxiliary variable for strtod and strtol function
    char hex[2]; // array that stores hex number for conversion
    string attr; // string struckt that stores identifers and number literals

    str_init(&attr);

    str_clear(&attr); // clears everything in string, if it is identifier we start storing data

    if (source == NULL) {
        error(99,"scanner.c", "get_next_token(string *attr)", "Internal error in: %s", source);
    }

    scanner_state = s_start; // we set initial scanner state

    while (1) {
        // we keep on getting our chars from source file until we get full token expression
        c = getc(source);

        switch (scanner_state) {
            case s_start:
                //check for space, tab, new line ...
                if (isspace(c)) {
                    if (c == '\n') {
                        line_num++;
                        token->token_type = T_EOL;
                        return L_SUCCESS;
                    }

                    scanner_state = s_start;
                } else if (isalpha(c) || c == '_') {
                    str_add_char(&attr, (char) c); //saves first char
                    scanner_state = s_identifier;
                } else if (isdigit(c)) {
                    // we check for useless zeros, that may signal number in different number system
                    if (c == '0') {
                        str_add_char(&attr, (char) c);
                        c = getc(source);
                        if (c == '.') { // it might be number like 0.1654
                            str_add_char(&attr, (char) c);
                            scanner_state = s_decimal_tmp;
                            break;
                        } else if (c == 'e' || c == 'E') { // zero on something is just zero
                            str_add_char(&attr, (char) c);
                            scanner_state = s_exp_tmp;
                            break;
                        } else if (isdigit(c)) { // if there is more numbers after zero, that is error
                            char_clear(&attr, c);
                            error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                        } else { // single zero
                            token->token_type = T_INT_VALUE;
                            token->attr.int_lit = strtol(attr.str, NULL, 10);
                            char_clear(&attr, c);
                            return L_SUCCESS;
                        }
                    }
                    str_add_char(&attr, (char) c); //save first number and switch state
                    scanner_state = s_int_lit;
                } else if (c == '"') {
                    // we dont save first char '"', because we dont want it in the string attribute
                    scanner_state = s_string_tmp;
                } else if (c == '/') {
                    // we load another char so we can decide, whether it is comment or div
                    c = getc(source);

                    if (c == '/') { // line comment
                        scanner_state = s_line_c;
                    } else if (c == '*') { // block comment
                        scanner_state = s_block_c;
                    } else {
                        token->token_type = T_DIV;
                        char_clear(&attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == ',') {
                    token->token_type = T_COMMA;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '+') {
                    token->token_type = T_PLUS;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '-') {
                    token->token_type = T_MINUS;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '*') {
                    token->token_type = T_MUL;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '(') {
                    token->token_type = T_L_BRACKET;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == ')') {
                    token->token_type = T_R_BRACKET;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '=') {
                    c = getc(source);

                    if (c == '=') { //
                        token->token_type = T_EQ;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else {
                        token->token_type = T_ASSIGNMENT;
                        char_clear(&attr, c); // we have to go one char back, because we loaded one if front of the assignment
                        return L_SUCCESS;
                    }
                } else if (c == '!') {
                    // we check for not eq operator otherwise it is error
                    c = getc(source);

                    if (c == '=') {
                        token->token_type= T_NEQ;
                        str_free(&attr);
                        return L_SUCCESS;
                    }

                    char_clear(&attr, c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                } else if (c == '>') {
                    c = getc(source);

                    if (c == '=') { // >=
                        token->token_type = T_GREATER_EQ;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else { // one char back and it is >
                        token->token_type = T_GREATER;
                        char_clear(&attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == '<') {
                    c = getc(source);

                    if (c == '=') { // <=
                        token->token_type= T_LESS_EQ;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else { // < and one char back
                        token->token_type = T_LESS;
                        char_clear(&attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == '{') {
                    token->token_type = T_LEFT_BRACE;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '}') {
                    token->token_type = T_RIGHT_BRACE;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == EOF) {
                    token->token_type = T_EOF;
                    str_free(&attr);
                    return L_SUCCESS;
                } else {
                    // wrong token
                    char_clear(&attr, c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                }
                break;
            // if we are reading correct char from source, then we are adding him into our string
            // otherwise we unget the char and check for the string if it is identifier or keyword
            case s_identifier: //f151
                // we are filling our string with indentifier
                if (isalnum(c) || c == '_') {
                    str_add_char(&attr, (char) c);
                } else {
                    // we initialize string attribute for identifier
                    str_init(&token->attr.str_lit);

                    ungetc(c, source); // we read all and we need to go one char back

                    // we check for keyword, if there is one we do not treat him as identifier, and
                    // we store his token and his string attribute
                    if (str_cmp_cons(&attr, "else") == 0) {
                        token->token_type = T_ELSE;
                        str_copy(&token->attr.str_lit, &attr);
                    } else if (str_cmp_cons(&attr, "float64") == 0) {
                        token->token_type = T_FLOAT64;
                        str_copy(&token->attr.str_lit, &attr);
                    } else if (str_cmp_cons(&attr, "for") == 0) {
                        token->token_type= T_FOR;
                        str_copy(&token->attr.str_lit, &attr);
                    } else if (str_cmp_cons(&attr, "func") == 0) {
                        token->token_type= T_FUNC;
                        str_copy(&token->attr.str_lit, &attr);
                    } else if (str_cmp_cons(&attr, "if") == 0) {
                        token->token_type = T_IF;
                        str_copy(&token->attr.str_lit, &attr);
                    } else if (str_cmp_cons(&attr, "int") == 0) {
                        token->token_type = T_INT;
                        str_copy(&token->attr.str_lit, &attr);
                    } else if (str_cmp_cons(&attr, "package") == 0) {
                        token->token_type= T_PACKAGE;
                        str_copy(&token->attr.str_lit, &attr);
                    } else if (str_cmp_cons(&attr, "return") == 0) {
                        token->token_type = T_RETURN;
                        str_copy(&token->attr.str_lit, &attr);
                    } else if (str_cmp_cons(&attr, "string") == 0) {
                        token->token_type = T_STRING;
                        str_copy(&token->attr.str_lit, &attr);
                    } else {
                        token->token_type= T_IDENTIFIER;
                        str_copy(&token->attr.str_lit, &attr);
                    }

                    str_free(&attr);
                    return L_SUCCESS;
                }
                break;
            case s_int_lit: //f16
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                } else if (c == 'e' || c == 'E') { // exp number
                    str_add_char(&attr, (char) c);
                    scanner_state = s_exp_tmp;
                } else if (c == '.') { // decimal number
                    str_add_char(&attr, (char) c);
                    scanner_state = s_decimal_tmp;
                } else {
                    token->token_type = T_INT_VALUE;
                    token->attr.int_lit = strtol(attr.str, NULL, 10);
                    char_clear(&attr, c);
                    return L_SUCCESS;
                }
                break;
            case s_exp_tmp: //q18
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                    scanner_state = s_decimal_lit;
                } else if (c == '+' || c == '-') { // signed exp number
                    str_add_char(&attr, (char) c);
                    scanner_state = s_exp_sig_tmp;
                } else {
                    char_clear(&attr, c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                }
                break;
            case s_exp_sig_tmp: //r18
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                    scanner_state = s_exp_lit;
                } else {
                    char_clear(&attr, c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                }
                break;
            case s_decimal_tmp: //q17
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                    scanner_state = s_decimal_lit;
                } else {
                    char_clear(&attr, c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                }
                break;
            case s_decimal_lit: //f17
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                } else if (c == 'e' || c == 'E') { // decimal exponential number
                    str_add_char(&attr, (char) c);
                    scanner_state = s_exp_tmp;
                } else {
                    ungetc(c, source);
                    token->token_type = T_DEC_VALUE;
                    token->attr.dec_lit = strtod(attr.str, NULL);
                    return L_SUCCESS;
                }
                break;
            case s_exp_lit: //f18
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                } else {
                    ungetc(c, source);
                    token->token_type = T_DEC_VALUE;
                    token->attr.dec_lit = strtod(attr.str, NULL);
                    return L_SUCCESS;
                }
                break;
            case s_line_c: //p2
                if (c == EOF || c == '\n') {
                    ungetc(c, source);
                    scanner_state = s_start;
                }
                break;
            case s_block_c: //p1
                if (c == '*') { // we loaded asterix and we check if it is end iwth */
                    c = getc(source);

                    if (c == '/') { // end of block comment and we go to start state
                        scanner_state = s_start;
                    } else if (c == '\n'){ // we increment line number for error message
                        line_num++;
                    } else { // we just loaded asterix and we keep going
                        ungetc(c, source);
                        scanner_state = s_block_c;
                    }
                } else if (c == EOF) { // block comment without end
                    char_clear(&attr, c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                } else { // keeps going in block comment state
                    scanner_state = s_block_c;
                }
                break;
            case s_string_tmp: //q19
                if (c > 31) {
                    if (c == '"') { // end of string
                        scanner_state = s_string;
                    } else if (c == '\\') { // escape sequence
                        scanner_state = s_esc_seq;
                    } else {
                        str_add_char(&attr, (char) c);
                    }
                } else { // chars less then 32 should be in escape sequence
                    char_clear(&attr, c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                }
                break;
            case s_esc_seq: //r19
                if (c == '\\') {
                    str_add_char(&attr, '\\');
                    scanner_state = s_string_tmp;
                } else if (c == 't') {
                    str_add_char(&attr, '\t');
                    scanner_state = s_string_tmp;
                } else if (c == 'n') {
                    str_add_char(&attr, '\n');
                    scanner_state = s_string_tmp;
                } else if (c == 'x') {
                    scanner_state = s_hex_tmp;
                } else if (c == '"') {
                    str_add_char(&attr, '\"');
                    scanner_state = s_string_tmp;
                } else {
                    char_clear(&attr, (char) c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                }
                break;
            case s_hex_tmp: //t19
                if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' ||
                    c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
                    isdigit(c)) {
                    hex[0] = (char) c; // stroring first hex number
                    scanner_state = s_hex_num;
                } else { // wrong hex number
                    char_clear(&attr, c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                }
                break;
            case s_hex_num: //u19
                if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' ||
                    c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
                    isdigit(c)) {
                    long pars_tmp; // temp variable for strtol function
                    hex[1] = (char) c; // storing second hex number
                    pars_tmp = strtol(hex, &endptr, 16); // converting hex number into decimal number
                    c = (int) pars_tmp;
                    // we are storing converted hex number as char into our string literal
                    str_add_char(&attr, (char) c);
                    scanner_state = s_string_tmp; //goes back to string reading
                } else { // wrong hex number
                    char_clear(&attr, c);
                    error(1,"scanner.c", "get_next_token(string *attr)", "Syntax error in: %s", source);
                }
                break;
            case s_string: //f19
                ungetc(c, source);
                // we initialize string attribute for string
                str_init(&token->attr.str_lit);
                token->token_type = T_STRING;
                str_copy(&token->attr.str_lit, &attr);
                str_free(&attr);
                return L_SUCCESS;
                break;
            default:
                error(1,"scanner.c", "get_next_token(string *attr)","Default option in switch statement");
                break;
        }
    }
}
