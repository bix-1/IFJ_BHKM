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
#include <stdlib.h>
#include <stdbool.h>

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

tToken exprToken;
tToken stopToken;
tokenStack symbolStack;
bool err = false;

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
    case T_INT_VALUE:
        return OP_value;
        break;
    case T_DOLLAR:
    case T_EOL:
    case T_EOF:
        return OP_dollar;
        break;

    default:
        // INSERT ERROR CALL
        printf("\nGET INDEX ERROR\n");
        err = true;
        return;
        break;
    }

    return 0; // TODO ERROR
}

void shift()
{

    stackElemPtr tmp;
    if (get_index(*parsData.token) == OP_value)
    {
        stack_push(parsData.stack, *parsData.token);
        tmp = parsData.stack->topToken;
        while (tmp->token.token_type != T_EMPTY)
        {
            printf("\nTOKEN SHIFT STACK VALUES TOP\t%d", tmp->token.token_type);
            tmp = tmp->nextTok;
        }
    }
    else
    {
        stack_push(&symbolStack, *parsData.token);
        tmp = symbolStack.topToken;
        while (tmp->token.token_type != T_EMPTY)
        {
            printf("\nSYMBOL SHIFT VALUES TOP\t%d", tmp->token.token_type);
            tmp = tmp->nextTok;
        }
    }
    get_next_token(parsData.token);
}

void reduce()
{
    stackElemPtr tmp2;
    tmp2 = parsData.stack->topToken;
    while (tmp2->token.token_type != T_EMPTY)
    {
        printf("\nREDUCE STACK VALUES TOP\t%d", tmp2->token.token_type);
        tmp2 = tmp2->nextTok;
    }

    tToken *tokenTop = &(parsData.stack->topToken->token);
    tToken *tokenAfterTop = &(parsData.stack->topToken->nextTok->token);
    tToken symbolTop = symbolStack.topToken->token;
    if (symbolTop.token_type == T_PLUS && parsData.stack->topToken->token.token_type != T_DOLLAR)
    {
        //IF E+E
        if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
        {
            // TO DO INSERT INSTRUCTIONS
            printf("\n\t\tRULE T_PLUS\tE = E+E\n");
            exprToken.token_type = T_EXPR;
            stack_pop(&symbolStack);
            stack_pop(parsData.stack);
            stack_pop(parsData.stack);
            stack_push(parsData.stack, exprToken);
        }
        else if (tokenAfterTop->token_type != T_EXPR)
        {
            // IF $+E IS ON STACK
            if (tokenAfterTop->token_type == T_DOLLAR)
            {
                printf("\n\t\t\tERROR E= E+E expr1\n");
                return;
                // TODO ERROR
            }

            printf("\n\t\tRULE\tE -> id\n");
            tokenAfterTop->token_type = T_EXPR;
        }
        else if (tokenTop->token_type != T_EXPR)
        {
            // IF E+ IS ON STACK
            if (tokenTop->token_type == T_DOLLAR)
            {
                printf("\n\t\t\tERROR E= E+E expr2\n");
                return;
                // TODO ERROR
            }

            // TO DO INSERT INSTRUCTIONS
            printf("\n\t\tRULE\tE -> id\n");
            tokenTop->token_type = T_EXPR;
        }
    }
    if (symbolTop.token_type == T_MINUS && parsData.stack->topToken->token.token_type != T_DOLLAR)
    {
        //IF
        if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
        {
            // TO DO INSERT INSTRUCTIONS
            printf("\n\t\tRULE T_MINUS\tE = E - E\n");
            exprToken.token_type = T_EXPR;
            stack_pop(&symbolStack);
            stack_pop(parsData.stack);
            stack_pop(parsData.stack);
            stack_push(parsData.stack, exprToken);
        }
        else if (tokenAfterTop->token_type != T_EXPR)
        {
            // IF $-E IS ON STACK
            if (tokenAfterTop->token_type == T_DOLLAR)
            {
                printf("\n\t\t\tERROR E= E - E expr1\n");
                return;
                // TODO ERROR
            }

            printf("\n\t\tRULE\tE -> id\n");
            tokenAfterTop->token_type = T_EXPR;
        }
        else if (tokenTop->token_type != T_EXPR)
        {
            // IF E- IS ON STACK
            if (tokenTop->token_type == T_DOLLAR)
            {
                printf("\n\t\t\tERROR E= E+E expr2\n");
                return;
                // TODO ERROR
            }

            // TO DO INSERT INSTRUCTIONS
            printf("\n\t\tRULE\tE -> id\n");
            tokenTop->token_type = T_EXPR;
        }
    }
    else
    {
        printf("\n\t\tPARS ERROR\n\n");
        err = true;
    }
    // KONTROLA STACKU
    tmp2 = parsData.stack->topToken;
    while (tmp2->token.token_type != T_EMPTY)
    {
        printf("\nREDUCE STACK VALUES BOTTOM\t%d", tmp2->token.token_type);
        tmp2 = tmp2->nextTok;
    }

    printf("\nEND REDUCE\n");
}

void parse_expression()
{

    int stop = 0;

    parsData.stack = (tokenStack *)malloc(sizeof(struct tokenStack));
    parsData.token = (tToken *)malloc(sizeof(tToken));
    stack_init(parsData.stack);
    stack_init(&symbolStack);

    printf("PARS\t%d\n", parsData.stack->topToken->token.token_type);
    printf("PARS\t%d\n", symbolStack.topToken->token.token_type);

    tToken token;
    token.token_type = T_DOLLAR;
    stack_push(parsData.stack, token);
    stack_push(&symbolStack, token);
    get_next_token(parsData.token);
    printf("\nGET TOKEN\t %d", parsData.token->token_type);

    while (1)
    {
        int indexStack, indexInput = 0;

        indexInput = get_index(*parsData.token);
        indexStack = get_index(symbolStack.topToken->token);

        printf("\nSTACK x INPUT:\t%d\t%d", indexStack, indexInput);

        switch (precTable[indexStack][indexInput])
        {
        case R: /*>*/
            printf("\nREDUCE");
            reduce();
            break;
        case S: /*<*/
            printf("\nSHIFT");
            shift();
            break;
        case Eq: /*=*/
            equal();
            break;
        case Err: /* empty*/
            /*error*/
            printf("\nExpression parse Error line 221\n");
            break;
        case A:
            printf("\nParse end tok stack\t%d", parsData.stack->topToken->token.token_type);
            printf("\nParse end symbol stack\t%d\n", symbolStack.topToken->token.token_type);
            stackElemPtr tmp;
            tmp = parsData.stack->topToken;
            while (tmp->token.token_type != T_EMPTY)
            {
                printf("TOKEN STACK VALUES\t%d\n", tmp->token.token_type);
                tmp = tmp->nextTok;
            }
            tmp = symbolStack.topToken;
            while (tmp->token.token_type != T_EMPTY)
            {
                printf("SYMBOL STACK VALUES\t%d\n", tmp->token.token_type);
                tmp = tmp->nextTok;
            }

            stack_free(&symbolStack);
            stack_free(parsData.stack);
            free(parsData.stack);
            free(parsData.token);
            return;
            break;
        default:
            break;
        }

/*         if (err || stop == 25)
        {
            printf("\nERROR\n");

            stack_free(&symbolStack);
            stack_free(parsData.stack);
            free(parsData.stack);
            free(parsData.token);
            return;
        }
        stop++; */
    }
}

void equal()
{
    return;
}
