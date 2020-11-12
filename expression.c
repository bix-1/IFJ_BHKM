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

// Precedence table
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

#define T_DOLLAR 500 // STACK BOTTOM
#define T_EXPR 501   // EXPRESSION

tToken exprToken;       // Expression token
tokenStack symbolStack; // Symbol stack
tokenStack tokStack;    // Token stack

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
        release_resources();
        error(2, "expression parser", "get_index", "Invalid input");
        return 0;
        break;
    }

    return 0;
}

void shift()
{

    //stackElemPtr tmp;

    // IF WE WANT TO PUSH NUMBERS
    if (get_index(*parsData.token) == OP_value)
    {
        stack_push(&tokStack, *parsData.token);

        /*                  // kontrola elementov nastacku
        tmp = tokStack.topToken;
        while (tmp->token.token_type != T_EMPTY)
        {
            printf("\nTOKEN SHIFT STACK VALUES TOP\t%d", tmp->token.token_type);
            tmp = tmp->nextTok;
        }  */
    }
    else // IF WE WANT TO PUSH SYMBOLS
    {
        stack_push(&symbolStack, *parsData.token);

        /*                // kontrola elementov nastacku
        tmp = symbolStack.topToken;
        while (tmp->token.token_type != T_EMPTY)
        {
            printf("\nSYMBOL SHIFT VALUES TOP\t%d", tmp->token.token_type);
            tmp = tmp->nextTok;
        } */
    }
    get_next_token(parsData.token);
}

void reduce()
{
    /*     // Vypis elementov na stacku
    stackElemPtr tmp2;
    tmp2 = tokStack.topToken;
    while (tmp2->token.token_type != T_EMPTY)
    {
        printf("\nREDUCE STACK VALUES TOP\t%d", tmp2->token.token_type);
        tmp2 = tmp2->nextTok;
    } */

    tToken *tokenTop = &tokStack.topToken->token;               // Top member on VALUE stack
    tToken *tokenAfterTop = &tokStack.topToken->nextTok->token; // Second from the top member on VALUE stack
    tToken symbolTop = symbolStack.topToken->token;             // Top member on SYMBOL stack

    // If value stack is !empty
    if (tokStack.topToken->token.token_type != T_DOLLAR)
    {
        switch (symbolTop.token_type)
        {
        case T_PLUS:
            //IF E + E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_PLUS\tE = E+E\n");
                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 + E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $+E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E+E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression at sum");
                }

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E + 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E+ IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E+E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression at sum");
                }

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;
        case T_MINUS:
            //IF E - E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_MINUS\tE = E - E\n");
                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 - E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $-E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E - E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression at substaction");
                }

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E - 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E- IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E+E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression at substaction");
                }

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;
        case T_MUL:
            //IF E * E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_MINUS\tE = E * E\n");
                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 * E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $*E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E * E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression at multiplication");
                }

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E * 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E* IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E+E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression at multiplication");
                }

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;
        case T_DIV:
            //IF E / E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_MINUS\tE = E / E\n");

                if (tokenTop->attr.int_lit == 0)
                {
                    release_resources();
                    error(9, "expression parser", "reduce", "Division by ZERO");
                }

                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 / E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $/E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E / E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression at division");
                }

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E / 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E/ IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E+E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression at division");
                }

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;
        case T_LESS:
            //IF E < E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_LESS\tE < E\n");
                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 < E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $<E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E < E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions <");
                }

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E < 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E< IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E < E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions <");
                }

                // TO DO INSERT INSTRUCTIONS
                printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;
        case T_LESS_EQ:
            //IF E <= E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_LESS_EQ\tE <= E\n");
                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 <= E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $/E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E <= E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions <=");
                }

                printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E <= 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E<= IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E <= E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions <=");
                }

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;
        case T_GREATER:
            //IF E > E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_GREATER\tE > E\n");
                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 > E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $>E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E > E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions >");
                }

                printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E > 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E > IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E > E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions >");
                }

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;
        case T_GREATER_EQ:
            //IF E >= E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_GREATER_EQ\tE >= E\n");
                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 >= E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $>=E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    printf("\n\t\t\tERROR E= E >= E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions >=");
                }

                printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E >= 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E >= IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E >= E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions >=");
                }

                // TO DO INSERT INSTRUCTIONS
                printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;
        case T_EQ:
            //IF E == E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_EQ\tE == E\n");
                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 == E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $ == E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    // printf("\n\t\t\tERROR E= E == E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions ==");
                }

                printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E == 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E == IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E == E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions ==");
                }

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;
        case T_NEQ:
            //IF E != E IS ON STACK
            if (tokenTop->token_type == T_EXPR && tokenAfterTop->token_type == T_EXPR)
            {
                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE T_NEQ\tE != E\n");
                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
                stack_pop(&tokStack);
                stack_push(&tokStack, exprToken);
            } //IF 5 != E IS ON STACK
            else if (tokenAfterTop->token_type != T_EXPR)
            {
                // IF $ != E IS ON STACK
                if (tokenAfterTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E != E expr1\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions !=");
                }

                printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->token_type = T_EXPR;
            } //IF E != 5 IS ON STACK
            else if (tokenTop->token_type != T_EXPR)
            {
                // IF E != IS ON STACK
                if (tokenTop->token_type == T_DOLLAR)
                {
                    //printf("\n\t\t\tERROR E= E != E expr2\n");
                    release_resources();
                    error(2, "expression parser", "reduce", "Missing expression when comparing expressions !=");
                }

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->token_type = T_EXPR;
            }
            break;

        default:
            release_resources();
            error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            //printf("\n\t\tPARS ERROR\n\n");
            break;
        }
    }

    /*     // KONTROLA STACKU
    tmp2 = tokStack.topToken;
    while (tmp2->token.token_type != T_EMPTY)
    {
        printf("\nREDUCE STACK VALUES BOTTOM\t%d", tmp2->token.token_type);
        tmp2 = tmp2->nextTok;
    }
    printf("\nEND REDUCE\n"); */
}

void equal()
{
    if (symbolStack.topToken->token.token_type != T_DOLLAR)
    {
        stack_pop(&symbolStack);
    }
    else
    {
        release_resources();
        error(2, "expression parser", "reduce", "Missing left parenthesis");
    }

    get_next_token(parsData.token);
    //printf("\n\t\tRULE\t E -> ( E ) \n");
    return;
}

void release_resources()
{
    stack_free(&symbolStack);
    stack_free(&tokStack);
    free(parsData.token);
}

void parse_expression()
{
    parsData.token = (tToken *)malloc(sizeof(tToken));
    stack_init(&tokStack);
    stack_init(&symbolStack);

    //printf("PARS\t%d\n", tokStack.topToken->token.token_type);
    //printf("PARS\t%d\n", symbolStack.topToken->token.token_type);

    tToken token;
    token.token_type = T_DOLLAR;
    stack_push(&tokStack, token);
    stack_push(&symbolStack, token);
    get_next_token(parsData.token); // TODO REMOVE IF IMPLEMENTED IN PARSER
    //printf("\nGET TOKEN\t %d", parsData.token->token_type);

    while (1)
    {
        int indexStack, indexInput = 0;

        indexInput = get_index(*parsData.token);
        indexStack = get_index(symbolStack.topToken->token);

        //printf("\nSTACK x INPUT:\t%d\t%d", indexStack, indexInput);

        switch (precTable[indexStack][indexInput])
        {
        case R: /*>*/
            //printf("\nREDUCE");
            reduce();
            break;
        case S: /*<*/
            //printf("\nSHIFT");
            shift();
            break;
        case Eq: /*=*/
            equal();
            break;
        case Err: /* empty*/
            release_resources();
            error(2, "expression parser", "parse_expression", "Faulty expression");
            break;
        case A:

            /*             // Kontrolne vypisy
            //printf("\nParse end tok stack\t%d", tokStack.topToken->token.token_type);
            //printf("\nParse end symbol stack\t%d\n", symbolStack.topToken->token.token_type);
            stackElemPtr tmp;
            tmp = tokStack.topToken;
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
            } */
            release_resources();
            return;
            break;
        default:

            release_resources();
            error(99, "expression parser", "parse_expression", " ???? ");
            break;
        }
    }
}
