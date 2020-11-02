/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: expression.c
 * Brief: Implementation of expression processing.
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */

#include <stack.h>
#include <scanner.h>

// Precedence table actions
typedef enum
{

    S,  // Shift <
    R,  // Reduce >
    A,  // Accept
    Eq, // Equal =
    Err // Error

} PT_action;

// Precedence table tokens
typedef enum
{
    OP_plus_minus,      // + -
    OP_mult_div,        // * /
    OP_parenth_open,    // (
    OP_parenth_close,   // )
    OP_rel_comp,        // < <= > >= == !=
    OP_value,           // i
    OP_dollar           // $

} PT_operator;

int precTable[7][7] = {
//  
    {R,S,S,R,R,S,R},
    {R,R,S,R,R,S,R},
    {S,S,S,Eq,S,S,Err},
    {R,R,Err,R,R,Err,R},
    {S,S,S,R,Err,S,R},
    {R,R,Err,R,R,Err,R},
    {S,S,S,Err,S,S,A},

};