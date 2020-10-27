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

#include "str.h"
#include "stdio.h"
#include "ctype.h"
#include <stdlib.h>

// global variable for input file
FILE *source;

/*
 * Token constants.
 */

#define L_SUCCESS 0
#define L_ERROR 1
#define I_ERROR 99

// math
#define T_PLUS 100          // +
#define T_MINUS 101         // -
#define T_MUL 102           // *
#define T_DIV 103           // :
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
#define T_EOF 116
#define T_EOL 117

// data types
#define T_INT 118
#define T_FLOAT64 119
#define T_STRING 120

// keywords
#define T_IF 121
#define T_ELSE 122
#define T_FUNC 123
#define T_RETRUN 124
#define T_FOR 125
#define T_PACKAGE 126

#define T_IDENTIFIER 127
/*
 * Automatons states.
 */
enum scanner_state {
    // s - initial state
    s_start = 10,                //s

    // math operators
    s_plus,                 //f2
    s_minus,                //f3
    s_mul,                  //f4
    s_div,                  //f5

    // comparison operators
    s_assignment,           //f8
    s_less,                 //f13
    s_leq,                  //f14
    s_greater,              //f11
    s_geq,                  //f12
    s_eq,                   //f9
    s_neq,                  //f10

    // number literals
    s_identifier,          //f15
    s_int_lit,              //f16 integer literal
    s_decimal_tmp,          //q17
    s_decimal_lit,          //f17 decimal literal
    s_exp_tmp,              //q18
    s_exp_sig_tmp,          //r18
    s_exp_lit,              //f18 exponential num lit

    // string literal
    s_string_tmp,           //q19
    s_esc_seq,              //r19
    s_hex_tmp,              //t19
    s_hex_num,              //u19
    s_string,               //f19

    // others
    s_comma,                //f1
    s_l_bracket,            //f6
    s_r_bracket,            //f7
    s_line_c,               //p2
    s_block_c,              //p1
};

/*
 * Data type, that allows to store different data types in the same memory location.
 * Only one member can contain a value at any given time.
 */
typedef union {
    int int_lit;
    double dec_lit;
    string *str_lit;
} Literal_attr;

/*
 * Structure, that contains token type and his attribute.
 */
typedef struct {
    int token_type;
    Literal_attr attr;
} Token;

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
 * Main function for lexical analyzer that keeps providing token types and his attribute
 * for syntax analyzer. Function parameter with pointer to string struct.
 */
int get_next_token(string *attr);
/*
 * Returns current line for error handling in error.c module
 */
int get_err_line();

#endif //IFJ_BHKM_SCANNER_H
