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

#include "expression.h"
#include "parser.h"
#include <stdio.h>

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
    OP_plus_minus,    // + -
    OP_mult_div,      // * /
    OP_parenth_open,  // (
    OP_parenth_close, // )
    OP_rel_comp,      // < <= > >= == !=  (RC)
    OP_value,         // i
    OP_dollar         // $

} PT_operator;

int precTable[7][7] = {
    /*  +- *,/  (   )   RC  i   $ */
    {R, S, S, R, R, S, R},     // +-
    {R, R, S, R, R, S, R},     // */
    {S, S, S, Eq, S, S, Err},  // (
    {R, R, Err, R, R, Err, R}, // )
    {S, S, S, R, Err, S, R},   // RC
    {R, R, Err, R, R, Err, R}, // i
    {S, S, S, Err, S, S, A},   // $

};

#define T_DOLLAR 500
#define T_EXPR 501

tToken stackToken;

int get_index(tToken token)
{

    switch (token.token_type)
    {
    case T_PLUS:
    case T_MINUS:
        return OP_plus_minus;
        break;
    case T_MUL:
    case T_DIV:
        return OP_mult_div;
        break;
    case T_L_BRACKET:
        return OP_parenth_open;
        break;
    case T_R_BRACKET:
        return OP_parenth_close;
        break;
    case T_LESS:
    case T_LESS_EQ:
    case T_GREATER:
    case T_GREATER_EQ:
    case T_EQ:
    case T_NEQ:
        return OP_rel_comp;
        break;
    case T_INT:
    case T_FLOAT64:
    case T_STRING:
    case T_EXPR:
    case T_IDENTIFIER:
        return OP_value;
        break;
    case T_DOLLAR:
        return OP_dollar;
        break;

    default:
        // INSERT ERROR CALL
        break;
    }

    return 0; // TODO ERROR
}

void shift()
{

    stack_push(parsData.stack, parsData.token);
    get_next_token(&parsData.token);
}

void reduce()
{

    tToken tmp = parsData.stack->topToken->token;
    if (tmp.token_type == T_INT || tmp.token_type == T_FLOAT64 || tmp.token_type == T_STRING || tmp.token_type == T_IDENTIFIER) /// E = id
    {
        tmp.token_type = T_EXPR;
        stack_pop(parsData.stack);
        stack_push(parsData.stack, tmp);
    }
    else if (tmp.token_type == T_EXPR && (top() == OP_plus_minus || top() == OP_mult_div)) /// E = E+E || E = E-E || E = E*E || E = E/E
    {
        for (int i = 0; i < 3; i++)
        {
            stack_pop(parsData.stack);
        }
        tmp.token_type = T_EXPR;
        stack_push(parsData.stack, tmp);
    }
    else if (tmp.token_type == T_R_BRACKET && top() == OP_parenth_open) /// (    E    )
    {
        do
        {
            stack_pop(parsData.stack);
        } while (parsData.stack->topToken->token.token_type != T_L_BRACKET);

        tmp.token_type = T_EXPR;
        stack_push(parsData.stack, tmp);
    }
}

int top()
{

    stackElemPtr tmp;
    tmp = parsData.stack->topToken;

    while (1)
    {
        if (get_index(tmp->token) == OP_value)
        {
            tmp = tmp->nextTok;
            continue;
        }
        else
        {
            return get_index(tmp->token);
        }
    }
}

void parse_expression()
{

    tToken token;
    token.token_type = T_DOLLAR;
    stack_push(parsData.stack, token);

    while (1)
    {
        int indexStack;
        int indexInput;

        indexStack = get_index(parsData.stack->topToken->token);
        indexInput = get_index(parsData.token);

        switch (precTable[indexStack][indexInput])
        {
        case R: /*>*/
            reduce();
            break;
        case S: /*<*/
            shift();
            break;
        case Eq: /*=*/
            equal();
            break;
        case Err: /* empty*/
            /*error*/
            break;
        case A:
            /*End while */
            break;
        default:
            break;
        }
    }
}

void equal () {
  return;
}
