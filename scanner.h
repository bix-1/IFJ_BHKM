/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: scanner.h
 * Brief: Scanner header file
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#ifndef IFJ_BHKM_SCANNER_H
#define IFJ_BHKM_SCANNER_H

#include <stdio.h>
#include <stdint.h>
#include "str.h"

// global variable for input file
FILE *source;

/*
 * Token constants.
 */

#define L_SUCCESS 0

// math
#define T_PLUS 100          // +
#define T_MINUS 101         // -
#define T_MUL 102           // *
#define T_DIV 103           // /
#define T_LESS 104          // <
#define T_LESS_EQ 105       // <=
#define T_GREATER 106       // >
#define T_GREATER_EQ 107    // >=
#define T_EQ 108            // ==
#define T_NEQ 109           // !=
#define T_ASSIGNMENT 110    // =

// others
#define T_COMMA 111         // ,
#define T_L_BRACKET 112     // (
#define T_R_BRACKET 113     // )
#define T_LINE_C 114        // '//'
#define T_BLOCK_C 115       // '/* */'
#define T_EOF 116           // '\n'
#define T_EOL 117

// data types
#define T_INT 118
#define T_FLOAT64 119
#define T_STRING 120

// keywords
#define T_IF 121
#define T_ELSE 122
#define T_FUNC 123
#define T_RETURN 124
#define T_FOR 125
#define T_PACKAGE 126

// identifier token
#define T_IDENTIFIER 127

#define T_EMPTY 128

// value tokens
#define T_INT_VALUE 129
#define T_DEC_VALUE 130

#define T_LEFT_BRACE 131 //{
#define T_RIGHT_BRACE 132 //}

#define T_DEF_IDENT 133 // :=
#define T_SEMICOLON 134 // ;

#define T_STRING_VALUE 135

// boolthen
#define T_NEG 136 // !
#define T_AND 137 // &&
#define T_OR 138 // ||

// bool data type
#define T_BOOL 139 // bool

// bool data value
#define T_TRUE 140 // true
#define T_FALSE 141 // false

/*
 * Automatons states.
 */
enum scanner_state {
    // s - initial state
    s_start = 10,

    // number literals
    s_identifier,
    s_int_lit,
    s_decimal_tmp,
    s_decimal_lit,
    s_exp_tmp,
    s_exp_sig_tmp,
    s_exp_lit,

    // string literal
    s_string_tmp,
    s_esc_seq,
    s_hex_tmp,
    s_hex_num,
    s_string,

    // others
    s_line_c,
    s_block_c,
};

/*
 * Data type, that allows us to store different data types in the same memory location.
 * Only one member can contain a value at any given time.
 */
typedef union {
    int64_t int_lit;
    double dec_lit;
    string str_lit;
} Literal_attr;

/*
 * Structure, that contains token type and his attribute.
 */
typedef struct {
    int token_type;
    Literal_attr attr;
} tToken;

/*
 * Function that helps us set up our input file, with parameter for it.
 */
void source_file_setup(FILE *f);

/*
 * Function with parameter for string and char. Function clears loaded char and frees
 * allocated memory with our string.
 */
void char_clear(string *attr, int c);

/*
 * Main function for lexical analyzer that keeps providing token types and his attributes
 * for syntax analyzer. Function parameter is token struct.
 */
int get_next_token(tToken *token);

/*
 * Returns current line for error handling in error.c module
 */
int get_err_line();

#endif //IFJ_BHKM_SCANNER_H
