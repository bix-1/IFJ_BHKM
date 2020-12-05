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
#include "escape_format.h"
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
    string attr; // string struckt that stores identifers and number literals
    //int eol = 0; // special variable that decides if we return EOL token before comment
    char hex [2];
    char *endptr;
    int eolerr = 0; // variable that helps us to decide, wether we unget char after new line o not

    str_init(&attr); // string for litteral attribute init
    str_clear(&attr); // clears everything in string, if it is identifier we start storing data

    // check for valid file stream
    if (source == NULL) {
        error(99,"scanner.c", "get_next_token", "Invalid input stream");
    }

    scanner_state = s_start; // we set initial scanner state

    while (1) {
        // we keep on getting our chars from source file until we get full token expression
        c = getc(source);

        switch (scanner_state) {
            case s_start:
                // ignoring white spaces
                if (isspace(c)) {
                    if (c == '\n') {
                        eolerr = 0;
                        line_num++;
                        // f22
                        token->token_type = T_EOL;
                        while (isspace(c = getc(source))) {
                          if (c == '\n') line_num++;
                        }
                        if (c == '/') {
                          c = getc(source);
                          if (c == '/') {
                            scanner_state = s_line_c;
                            continue;
                          }
                          else if (c == '*') {
                            scanner_state = s_block_c;
                            continue;
                          } else {
                              ungetc(c,source);
                              ungetc('/', source);
                              eolerr=1;
                          }
                        }
                        // if we reached / after new line char, we dont unget him, because we already did
                        // if there is not / after new line, we unget char
                        if (eolerr == 0) {
                            ungetc(c,source);
                        }
                        str_free(&attr);
                        return L_SUCCESS;
                    }

                    scanner_state = s_start;
                } else if (isalpha(c) || c == '_') {
                    // saving first loaded char
                    str_add_char(&attr, (char) c);
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
                            error(1,"scanner", "get_next_token", "Lexical error");
                        } else { // single zero
                            token->token_type = T_INT_VALUE;
                            token->attr.int_lit = strtol(attr.str, NULL, 10);
                            char_clear(&attr, c);
                            return L_SUCCESS;
                        }
                    }
                    // save first number and goes into state for integer literal
                    str_add_char(&attr, (char) c);
                    scanner_state = s_int_lit;
                } else if (c == '"') {
                    // we dont save first char '"', because we dont want it in the string attribute
                    scanner_state = s_string_tmp;
                } else if (c == '/') {
                    // we load another char so we can decide, whether it is comment or div
                    c = getc(source);

                    if (c == '/') { // line comment
                        scanner_state = s_line_c;
                        continue;
                    } else if (c == '*') { // block comment
                        scanner_state = s_block_c;
                        continue;
                    } else {
                        token->token_type = T_DIV;
                        char_clear(&attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == ',') {
                    // f1
                    token->token_type = T_COMMA;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '+') {
                    // f2
                    token->token_type = T_PLUS;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '-') {
                    // f3
                    token->token_type = T_MINUS;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '*') {
                    // f4
                    token->token_type = T_MUL;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '(') {
                    // f6
                    token->token_type = T_L_BRACKET;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == ')') {
                    // f5
                    token->token_type = T_R_BRACKET;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == ';') {
                    // f7
                    token->token_type = T_SEMICOLON;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == ':') {
                    // we check for definition of id operator otherwise it is error
                    c = getc(source);
                    // f8
                    if (c == '=') {
                        token->token_type = T_DEF_IDENT;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else {
                        char_clear(&attr, c);
                        error(1,"scanner", "get_next_token", "Lexical error");
                    }
                } else if (c == '=') {
                    // we check for eq operator otherwise it is assignment operator
                    c = getc(source);
                    // f10
                    if (c == '=') { //
                        token->token_type = T_EQ;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else {
                        // f9
                        token->token_type = T_ASSIGNMENT;
                        char_clear(&attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == '!') {
                    // we check for not eq operator otherwise it is error
                    c = getc(source);
                    // f12
                    if (c == '=') {
                        token->token_type= T_NEQ;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else {
                        // f11
                        token->token_type = T_NEG;
                        char_clear(&attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == '>') {
                    // check for greater or equal operator or just greater operator
                    c = getc(source);
                    // f14
                    if (c == '=') {
                        token->token_type = T_GREATER_EQ;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else {
                        // f13
                        token->token_type = T_GREATER;
                        char_clear(&attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == '<') {
                    // check for less or equal operator or just less operator
                    c = getc(source);
                    // f16
                    if (c == '=') {
                        token->token_type= T_LESS_EQ;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else {
                        // f15
                        token->token_type = T_LESS;
                        char_clear(&attr, c);
                        return L_SUCCESS;
                    }
                } else if (c == '{') {
                    // f17
                    token->token_type = T_LEFT_BRACE;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '}') {
                    // f18
                    token->token_type = T_RIGHT_BRACE;
                    str_free(&attr);
                    return L_SUCCESS;
                } else if (c == '&') {
                    // check for and operator or error
                    c = getc(source);
                    // f19
                    if (c == '&') {
                        token->token_type = T_AND;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else {
                        char_clear(&attr, c);
                        error(1,"scanner", "get_next_token", "Lexical error");
                    }
                } else if (c == '|') {
                    // check for or operator, or error
                    c = getc(source);
                    // f20
                    if (c == '|') {
                        token->token_type = T_OR;
                        str_free(&attr);
                        return L_SUCCESS;
                    } else {
                        char_clear(&attr, c);
                        error(1,"scanner", "get_next_token", "Lexical error");
                    }
                }else if (c == EOF) {
                    // f21
                    token->token_type = T_EOF;
                    str_free(&attr);
                    return L_SUCCESS;
                } else {
                    // wrong token
                    char_clear(&attr, c);
                    error(1,"scanner", "get_next_token", "Lexical error");
                }
                break;
            // if we are reading correct char from source, then we are adding him into our string
            // otherwise we unget the char and check for the string if it is identifier or keyword
            case s_identifier: // f23
                // we are filling our string attribute with indentifier
                if (isalnum(c) || c == '_') {
                    str_add_char(&attr, (char) c);
                } else {
                    // we check for keyword, if there is one we do not treat him as identifier, and
                    // return him as a own token
                    if (str_cmp_cons(&attr, "else") == 0) {
                        token->token_type = T_ELSE;

                    } else if (str_cmp_cons(&attr, "float64") == 0) {
                        token->token_type = T_FLOAT64;

                    } else if (str_cmp_cons(&attr, "for") == 0) {
                        token->token_type= T_FOR;

                    } else if (str_cmp_cons(&attr, "func") == 0) {
                        token->token_type= T_FUNC;

                    } else if (str_cmp_cons(&attr, "if") == 0) {
                        token->token_type = T_IF;

                    } else if (str_cmp_cons(&attr, "int") == 0) {
                        token->token_type = T_INT;

                    } else if (str_cmp_cons(&attr, "package") == 0) {
                        token->token_type= T_PACKAGE;

                    } else if (str_cmp_cons(&attr, "return") == 0) {
                        token->token_type = T_RETURN;

                    } else if (str_cmp_cons(&attr, "string") == 0) {
                        token->token_type = T_STRING;

                    } else if (str_cmp_cons(&attr, "bool") == 0) {
                        token->token_type = T_BOOL;

                    } else if (str_cmp_cons(&attr, "true") == 0) {
                        token->token_type = T_TRUE;

                    } else if (str_cmp_cons(&attr, "false") == 0) {
                        token->token_type = T_FALSE;

                    } else {
                        // string init for value of identifier
                        str_init(&token->attr.str_lit);
                        token->token_type= T_IDENTIFIER;
                        str_copy(&token->attr.str_lit, &attr);
                    }

                    char_clear(&attr, c);
                    return L_SUCCESS;
                }
                break;
            case s_int_lit:
                // f24
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                } else if (c == 'e' || c == 'E') { // exp number state
                    str_add_char(&attr, (char) c);
                    scanner_state = s_exp_tmp;
                } else if (c == '.') { // decimal number state
                    str_add_char(&attr, (char) c);
                    scanner_state = s_decimal_tmp;
                } else if (isalpha(c)) {
                    char_clear(&attr, c);
                    error(2, "scanner", "get_next_token", "Invalid identifier");
                } else {
                    token->token_type = T_INT_VALUE;
                    token->attr.int_lit = strtol(attr.str, NULL, 10);
                    char_clear(&attr, c);
                    return L_SUCCESS;
                }
                break;
            case s_exp_tmp:
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                    scanner_state = s_decimal_lit;
                } else if (c == '+' || c == '-') { // signed exp number state
                    str_add_char(&attr, (char) c);
                    scanner_state = s_exp_sig_tmp;
                } else {
                    char_clear(&attr, c);
                    error(1,"scanner", "get_next_token", "Lexical error");
                }
                break;
            case s_exp_sig_tmp:
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                    scanner_state = s_exp_lit;
                } else {
                    char_clear(&attr, c);
                    error(1,"scanner", "get_next_token", "Lexical error");
                }
                break;
            case s_decimal_tmp:
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                    scanner_state = s_decimal_lit;
                } else {
                    char_clear(&attr, c);
                    error(1,"scanner", "get_next_token", "Lexical error");
                }
                break;
            case s_decimal_lit:
                // f25
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                } else if (c == 'e' || c == 'E') { // decimal exponential number state
                    str_add_char(&attr, (char) c);
                    scanner_state = s_exp_tmp;
                } else {
                    token->token_type = T_DEC_VALUE;
                    token->attr.dec_lit = strtod(attr.str, NULL);
                    char_clear(&attr, c);
                    return L_SUCCESS;
                }
                break;
            case s_exp_lit:
                // f26
                if (isdigit(c)) {
                    str_add_char(&attr, (char) c);
                } else {
                    token->token_type = T_DEC_VALUE;
                    token->attr.dec_lit = strtod(attr.str, NULL);
                    char_clear(&attr, c);
                    return L_SUCCESS;
                }
                break;
            case s_line_c:
                while (c != EOF && c != '\n') {
                  c = fgetc(source);
                }
                ungetc(c, source);
                scanner_state = s_start;
                break;
            case s_block_c:
                // if we loaded asterix, we load another and check for end of block comment
                if (c == '*') {
                    c = getc(source);

                    if (c == '/') { // end of block comment and we go to start state
                        /*if (eol == 1) {
                            return L_SUCCESS;
                        }*/
                        scanner_state = s_start;
                    } else {
                        ungetc(c, source);
                    }
                } else if (c == '\n'){ // we increment line number for error message
                    line_num++;
                } else if (c == EOF) { // block comment without end
                    char_clear(&attr, c);
                    error(1,"scanner", "get_next_token", "Lexical error");
                }
                break;
            case s_string_tmp:
                if (c > 31) {
                    if (c == '"') { // end of string
                        scanner_state = s_string;
                    } else if (c == '\\') { // escape sequence
                        //str_add_char(&attr, (char) c);
                        scanner_state = s_esc_seq;
                    } else {
                        str_add_char(&attr, (char) c);
                    }
                } else { // chars less then 32 should be in escape sequence
                    char_clear(&attr, c);
                    error(1,"scanner", "get_next_token", "Lexical error");
                }
                break;
            case s_esc_seq:
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
                    error(1,"scanner", "get_next_token", "Lexical error");
                }
                break;
            case s_hex_tmp:
                if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' ||
                    c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
                    isdigit(c)) {
                    hex[0] = (char) c;
                    scanner_state = s_hex_num;
                } else { // wrong hex number
                    char_clear(&attr, c);
                    error(1,"scanner", "get_next_token", "Lexical error");
                }
                break;
            case s_hex_num:
                if (c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f' ||
                    c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' ||
                    isdigit(c)) {
                    long pars_tmp; // temp variable for strtol function
                    hex[1] = (char) c; // storing second hex number
                    pars_tmp = strtol(hex, &endptr, 16); // converting hex number into decimal number
                    c = (int) pars_tmp;
                    //sprintf(attr.str, "%s", hex);
                    str_add_char(&attr, (char) c);
                    scanner_state = s_string_tmp;
                } else { // wrong hex number
                    char_clear(&attr, c);
                    error(1,"scanner", "get_next_token", "Lexical error");
                }
                break;
            case s_string: // f27
                // we initialize string attribute for string
                str_init(&token->attr.str_lit);
                str_clear(&token->attr.str_lit);
                token->token_type = T_STRING_VALUE;
                char *ss = attr.str;
                char *temp = escape_reformat(ss);
                attr.str = temp;
                attr.str_lenght = (int) strlen(attr.str) + 1;
                attr.str_alloc_size = (int) strlen(attr.str) + 1;
                free(ss);
                str_copy(&token->attr.str_lit, &attr);
                char_clear(&attr, c);
                return L_SUCCESS;
                break;
            default:
                char_clear(&attr, c);
                error(1,"scanner.c", "get_next_token(string *attr)","Default option in switch statement");
                break;
        }
    }
}
