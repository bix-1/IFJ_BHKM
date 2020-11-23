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
#include "error.h"
#include "ll.h"
#include "codegen.h"
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
    OP_STRING,        // string
    OP_dollar         // $

} PT_operator;

// Precedence table
int precTable[8][8] = {
    /*  +- *,/  (   )   RC  i  STRING  $ */
    {R, S, S, R, R, S, S, R},             // +-
    {R, R, S, R, R, S, Err, R},           // */
    {S, S, S, Eq, S, S, Err, Err},        // (
    {R, R, Err, R, R, Err, Err, R},       // )
    {S, S, S, R, Err, S, Err, R},         // RC
    {R, R, Err, R, R, Err, Err, R},       // i
    {R, Err, Err, Err, Err, Err, Err, R}, // STRING
    {S, S, S, Err, S, S, S, A},           // $

};

#define T_DOLLAR 500 // STACK BOTTOM
#define T_EXPR 501   // EXPRESSION

tToken exprToken;       // Expression token
tokenStack symbolStack; // Symbol stack
tokenStack tokStack;    // Token stack
int exprNumber = 0;
symtable_value_t retExpr = NULL;
bool eps;

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
    case T_DEC_VALUE:
        return OP_value;
        break;
    case T_STRING_VALUE:
        return OP_STRING;
        break;
    case T_DOLLAR:
    case T_EOL:
    case T_COMMA:
    case T_LEFT_BRACE:
    case T_SEMICOLON:
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

void check_symtable(stackElemPtr elem)
{
    //printf("\nCHECK_SYMTABLE ORIGINAL TYPE\t%d\n", elem->originalType);
    if (elem->originalType == T_IDENTIFIER)
    {
        printf("\nSEARCH IDENTIFIER VARIABLE\n");
        eps = false;
        elem->data = id_find(scope_get_head(), to_string(&(elem->token)));
        //printf("\nID FOUND Variable \t%s\t%d\n", *(elem->data->key), elem->data->symbol.sym_var_item->data.int_t);
    }
    else
    {
        printf("\nADD NUMBER VARIABLE \n");
        elem->data = create_variable(elem);
        //printf("\nPRINT FOUND ID  \t%s\n", elem->data->symbol.sym_var_item->name);
    }

    retExpr = elem->data;
}

symtable_value_t create_dest(stackElemPtr elem)
{
    char *id = create_id();
    char *id_scope = id_add_scope(scope_get_head(), id);

    // create variable and add to symtable
    sym_var_item_t *var_item = sym_var_item_init(id);
    sym_var_item_set_type(var_item, var_type_check(&(elem->token)));
    symbol_t var_sym = {.sym_var_item = var_item};
    elem_t *var = elem_init(SYM_VAR_ITEM, var_sym);
    symtable_iterator_t it = symtable_insert(symtable, id_scope, var);

    return symtable_iterator_get_value(it);
}

symtable_value_t create_variable(stackElemPtr elem)
{
    char *id = create_id();
    char *id_scope = id_add_scope(scope_get_head(), id);
    // create variable and add to symtable
    sym_var_item_t *var_item = sym_var_item_init(id);
    sym_var_item_set_type(var_item, var_type_check(&(elem->token)));
    variable_t variable;
    switch (elem->token.token_type)
    {
    case T_INT_VALUE:
        variable.int_t = to_int(&(elem->token));
        //printf("\nVARIABLE INT\t%d\n", variable);
        break;
    case T_DEC_VALUE:
        variable.float64_t = to_double(&(elem->token));
        ////printf("\nVARIABLE FLOAT\t%f\n", variable);
        break;
    case T_STRING_VALUE:
        variable.string_t = to_string(&(elem->token));
        ////printf("\nVARIABLE String\t%s\n", variable);
        break;
    default:
        release_resources();
        error(99, "expression parser", "create_variable", "Memory allocation failed");
        break;
    }

    sym_var_item_set_data(var_item, variable);

    symbol_t var_sym = {.sym_var_item = var_item};
    elem_t *var = elem_init(SYM_VAR_ITEM, var_sym);
    symtable_insert(symtable, id_scope, var);

    return var;
}

char *create_id()
{
    // id = scope:N.oExpress = strlen scope + sizeof exprNumber + "express" + \0
    int length = snprintf(NULL, 0, "%d", exprNumber);
    char *number = malloc(length + 1);
    char *id = malloc(sizeof(char) * (length + 5));
    if (id == NULL || number == NULL)
    {
        release_resources();
        free(id);
        free(number);
        error(99, "expression parser", "create_variable", "Memory allocation failed");
    }
    sprintf(number, "%d", exprNumber); // INT TO STING
    id[0] = '\0';
    strcat(id, number);
    free(number);
    strcat(id, "expr");
    exprNumber++;
    return id;
}

int var_type_check(tToken *token)
{

    switch (token->token_type)
    {
    case T_INT_VALUE:
        return VAR_INT;
        break;
    case T_DEC_VALUE:
        return VAR_FLOAT64;
        break;
    case T_STRING_VALUE:
        return VAR_STRING;
        break;
    case T_TRUE:
        return VAR_BOOL;
        break;
    case T_FALSE:
        return VAR_BOOL;
        break;

    default:
        break;
    }
    // TODO ADD ERROR
    return -1; // PLACEHOLDER TODO remove
}

void check_types(stackElemPtr top, stackElemPtr afterTop)
{
    if ((top->originalType == T_STRING_VALUE && afterTop->originalType != T_STRING_VALUE) || (top->originalType != T_STRING_VALUE && afterTop->originalType == T_STRING_VALUE))
    {
        release_resources();
        error(5, "expression parser", "check_types", "Variable types do not match.");
    }

    //printf("\nCHECK TYPES\n");
}

void check_expr(tToken *token)
{

    if (token->token_type == T_DOLLAR)
    {
        release_resources();
        error(2, "expression parser", "reduce", "Missing expression at operation");
    }

    //printf("\nCHECK EXPRS\n");
}

void shift()
{

    //stackElemPtr tmp;

    // IF WE WANT TO PUSH NUMBERS

    if (get_index(*parsData.token) == OP_value || get_index(*parsData.token) == OP_STRING)
    {
        stack_push(&tokStack, *parsData.token);

        check_symtable(tokStack.topToken);
        print();
    }
    else // IF WE WANT TO PUSH SYMBOLS
    {
        stack_push(&symbolStack, *parsData.token);
    }

    /*     if (parsData.token->token_type == T_IDENTIFIER)
    {
        // printf("\nTOKEN %s\n", parsData.token->attr.str_lit.str);
        //free(parsData.token->attr.str_lit.str);
    } */

    //printf("\nSHIFT DATA\t%s\n", *(last_elem->key));
    get_next_token(parsData.token);
}

void reduce()
{
    // Vypis elementov na stacku
    // stackElemPtr tmp2;
    // tmp2 = tokStack.topToken;
    // while (tmp2->token.token_type != T_EMPTY)
    // {
    //     printf("\nREDUCE STACK VALUES TOP\t%d\n", tmp2->token.token_type);
    //     tmp2 = tmp2->nextTok;
    // }
    stackElemPtr tokenTop = tokStack.topToken;               // Top member on VALUE stack
    stackElemPtr tokenAfterTop = tokStack.topToken->nextTok; // Second from the top member on VALUE stack
    tToken symbolTop = symbolStack.topToken->token;          // Top member on SYMBOL stack
    // If value stack is !empty
    //printf("\nSYMBOL\t%d\n", symbolTop.token_type);
    if (tokStack.topToken->token.token_type != T_DOLLAR)
    {
        //printf("\nERR\t%d\t%d\t%d\n", tokenTop->originalType, tokenAfterTop->originalType, symbolTop.token_type);
        if (tokenTop->originalType == T_STRING_VALUE && tokenAfterTop->originalType == T_STRING_VALUE && symbolTop.token_type != T_PLUS)
        {
            release_resources();
            error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
        }

        check_types(tokenTop, tokenAfterTop);

        switch (symbolTop.token_type)
        {
        case T_PLUS:
            //IF E + E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {

                //printf("\n\t\tRULE T_PLUS\tE = E+E\n");

                symtable_value_t dest = create_dest(tokenTop);

                if (tokenTop->originalType == T_STRING_VALUE && tokenAfterTop->originalType == T_STRING_VALUE)
                {
                    // Because valgring was fucking me up about conditional jump on uninitialised value
                    dest->symbol.sym_var_item->data.string_t = NULL;

                    // STRING CONCATENATION
                    instr_t *instrCONCAT = instr_create();
                    instr_set_type(instrCONCAT, IC_CONCAT_STR);
                    instr_add_dest(instrCONCAT, dest);
                    instr_add_elem1(instrCONCAT, tokenAfterTop->data);
                    instr_add_elem2(instrCONCAT, tokenTop->data);
                    list_add(list, instrCONCAT);
                }
                else
                {
                    // Number addition
                    // INSTRUCTIONS
                    instr_t *instrADD = instr_create();
                    instr_set_type(instrADD, IC_ADD_VAR);
                    instr_add_dest(instrADD, dest);
                    instr_add_elem1(instrADD, tokenAfterTop->data);
                    instr_add_elem2(instrADD, tokenTop->data);
                    list_add(list, instrADD);
                }

                tokenAfterTop->data = dest;

                retExpr = dest;

                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 + E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                //printf("\n\t\tRULE +\tE -> id\n");
                // IF $+E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                tokenAfterTop->expr = true;

            } //IF E + 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E+ IS ON STACK
                //printf("\n\t\tRULE +\tE -> id\n");
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                tokenTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        case T_MINUS:
            //IF E - E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {
                //printf("\n\t\tRULE T_MINUS\tE = E - E\n");

                symtable_value_t dest = create_dest(tokenTop);

                //INSTRUCTIONS
                instr_t *instrSUB = instr_create();
                instr_set_type(instrSUB, IC_SUB_VAR);
                instr_add_dest(instrSUB, dest);
                instr_add_elem1(instrSUB, tokenAfterTop->data);
                instr_add_elem2(instrSUB, tokenTop->data);
                list_add(list, instrSUB);

                tokenAfterTop->data = dest;
                retExpr = dest;

                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 - E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $-E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //printf("\n\t\tRULE -\tE -> id\n");
                tokenAfterTop->expr = true;

            } //IF E - 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E- IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE -\tE -> id\n");
                tokenTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        case T_MUL:
            //IF E * E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {
                //printf("\n\t\tRULE T_MUL\tE = E * E\n");

                symtable_value_t dest = create_dest(tokenTop);

                // INSTRUCTIONS
                instr_t *instrMUL = instr_create();
                instr_set_type(instrMUL, IC_MUL_VAR);
                instr_add_dest(instrMUL, dest);
                instr_add_elem1(instrMUL, tokenAfterTop->data);
                instr_add_elem2(instrMUL, tokenTop->data);
                list_add(list, instrMUL);

                tokenAfterTop->data = dest;
                retExpr = dest;

                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 * E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $*E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //printf("\n\t\tRULE *\tE -> id\n");
                tokenAfterTop->expr = true;
            } //IF E * 5 IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF E* IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE *\tE -> id\n");
                tokenAfterTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        case T_DIV:
            //IF E / E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {
                //printf("\n\t\tRULE T_DIV\tE = E / E\n");
                // DIVIDING BY ZERO
                if (tokenTop->token.attr.int_lit == 0)
                {
                    release_resources();
                    error(9, "expression parser", "reduce", "Division by ZERO");
                }

                symtable_value_t dest = create_dest(tokenTop);

                // INSTRUCTIONS
                instr_t *instrDIV = instr_create();
                instr_set_type(instrDIV, IC_DIV_VAR);
                instr_add_dest(instrDIV, dest);
                instr_add_elem1(instrDIV, tokenAfterTop->data);
                instr_add_elem2(instrDIV, tokenTop->data);
                list_add(list, instrDIV);

                tokenAfterTop->data = dest;
                retExpr = dest;

                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 / E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $/E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->expr = true;
            } //IF E / 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E/ IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        case T_LESS:
            //IF E < E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {
                //printf("\n\t\tRULE T_LESS\tE < E\n");

                check_types(tokenTop, tokenAfterTop);

                symtable_value_t dest = create_dest(tokenTop);

                ////printf("\n DEST EXPRESSION\t %s\n", dest->symbol.sym_var_item->name);

                // INSTRUCTIONS
                instr_t *instrLT = instr_create();
                instr_set_type(instrLT, IC_LT_VAR);
                instr_add_dest(instrLT, dest);
                instr_add_elem1(instrLT, tokenAfterTop->data);
                instr_add_elem2(instrLT, tokenTop->data);
                list_add(list, instrLT);

                tokenAfterTop->data = dest;
                retExpr = dest;

                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 < E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $<E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //printf("\n\t\tRULE <\tE -> id\n");
                tokenAfterTop->expr = true;
            } //IF E < 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E< IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE <\tE -> id\n");
                tokenTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        case T_LESS_EQ:
            //IF E <= E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {
                //printf("\n\t\tRULE T_LESS_EQ\tE <= E\n");

                check_types(tokenTop, tokenAfterTop);

                symtable_value_t tmpDestLT = create_dest(tokenTop);
                symtable_value_t tmpDestEQ = create_dest(tokenTop);
                symtable_value_t tmpDestRet = create_dest(tokenTop);

                //printf("\n DEST1 EXPRESSION\t %s\n", tmpDestLT->symbol.sym_var_item->name);
                //printf("\n DEST2 EXPRESSION\t %s\n", tmpDestEQ->symbol.sym_var_item->name);
                //printf("\n DEST3 EXPRESSION\t %s\n", tmpDestRet->symbol.sym_var_item->name);

                // INSTRUCTIONS
                instr_t *instrLT = instr_create();
                instr_set_type(instrLT, IC_LT_VAR);
                instr_add_dest(instrLT, tmpDestLT);
                instr_add_elem1(instrLT, tokenAfterTop->data);
                instr_add_elem2(instrLT, tokenTop->data);
                list_add(list, instrLT);

                instr_t *instrEQ = instr_create();
                instr_set_type(instrEQ, IC_EQ_VAR);
                instr_add_dest(instrEQ, tmpDestEQ);
                instr_add_elem1(instrEQ, tokenAfterTop->data);
                instr_add_elem2(instrEQ, tokenTop->data);
                list_add(list, instrEQ);

                instr_t *instrOR = instr_create();
                instr_set_type(instrOR, IC_OR_VAR);
                instr_add_dest(instrOR, tmpDestRet);
                instr_add_elem1(instrOR, tmpDestEQ);
                instr_add_elem2(instrOR, tmpDestLT);
                list_add(list, instrOR);

                retExpr = tmpDestRet;

                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 <= E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF <=E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->expr = true;
            } //IF E <= 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E<= IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        case T_GREATER:
            //IF E > E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {
                //printf("\n\t\tRULE T_GREATER\tE > E\n");

                check_types(tokenTop, tokenAfterTop);

                symtable_value_t dest = create_dest(tokenTop);

                //printf("\n DEST EXPRESSION\t %s\n", dest->symbol.sym_var_item->name);

                // INSTRUCTIONS
                instr_t *instrGT = instr_create();
                instr_set_type(instrGT, IC_GT_VAR);
                instr_add_dest(instrGT, dest);
                instr_add_elem1(instrGT, tokenAfterTop->data);
                instr_add_elem2(instrGT, tokenTop->data);
                list_add(list, instrGT);

                retExpr = dest;

                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 > E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $>E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->expr = true;
            } //IF E > 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E > IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        case T_GREATER_EQ:
            //IF E >= E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {
                //printf("\n\t\tRULE T_GREATER_EQ\tE >= E\n");

                check_types(tokenTop, tokenAfterTop);

                symtable_value_t tmpDestGT = create_dest(tokenTop);
                symtable_value_t tmpDestEQ = create_dest(tokenTop);
                symtable_value_t tmpDestRet = create_dest(tokenTop);

                //printf("\n DEST1 EXPRESSION\t %s\n", tmpDestGT->symbol.sym_var_item->name);
                //printf("\n DEST2 EXPRESSION\t %s\n", tmpDestEQ->symbol.sym_var_item->name);
                //printf("\n DEST3 EXPRESSION\t %s\n", tmpDestRet->symbol.sym_var_item->name);

                // INSTRUCTIONS
                instr_t *instrGT = instr_create();
                instr_set_type(instrGT, IC_GT_VAR);
                instr_add_dest(instrGT, tmpDestGT);
                instr_add_elem1(instrGT, tokenAfterTop->data);
                instr_add_elem2(instrGT, tokenTop->data);
                list_add(list, instrGT);

                instr_t *instrEQ = instr_create();
                instr_set_type(instrEQ, IC_EQ_VAR);
                instr_add_dest(instrEQ, tmpDestEQ);
                instr_add_elem1(instrEQ, tokenAfterTop->data);
                instr_add_elem2(instrEQ, tokenTop->data);
                list_add(list, instrEQ);

                instr_t *instrOR = instr_create();
                instr_set_type(instrOR, IC_OR_VAR);
                instr_add_dest(instrOR, tmpDestRet);
                instr_add_elem1(instrOR, tmpDestEQ);
                instr_add_elem2(instrOR, tmpDestGT);
                list_add(list, instrOR);

                retExpr = tmpDestRet;

                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 >= E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $>=E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->expr = true;
            } //IF E >= 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E >= IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        case T_EQ:
            //IF E == E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {
                //printf("\n\t\tRULE T_EQ\tE == E\n");

                check_types(tokenTop, tokenAfterTop);

                symtable_value_t dest = create_dest(tokenTop);

                ////printf("\n DEST EXPRESSION\t %s\n", dest->symbol.sym_var_item->name);

                // INSTRUCTIONS
                instr_t *instrEQ = instr_create();
                instr_set_type(instrEQ, IC_EQ_VAR);
                instr_add_dest(instrEQ, dest);
                instr_add_elem1(instrEQ, tokenAfterTop->data);
                instr_add_elem2(instrEQ, tokenTop->data);
                list_add(list, instrEQ);

                retExpr = dest;

                exprToken.token_type = T_EXPR;
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 == E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $ == E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->expr = true;
            } //IF E == 5 IS ON STACK
            else if (tokenTop->token.token_type != T_EXPR)
            {
                // IF E == IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        case T_NEQ:
            //IF E != E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {

                check_types(tokenTop, tokenAfterTop);

                symtable_value_t tmpDestEQ = create_dest(tokenTop);
                symtable_value_t tmpDestRet = create_dest(tokenTop);
                ;

                //printf("\n DEST1 EXPRESSION\t %s\n", tmpDestEQ->symbol.sym_var_item->name);
                //printf("\n DEST2 EXPRESSION\t %s\n", tmpDestRet->symbol.sym_var_item->name);

                // INSTRUCTIONS
                instr_t *instrEQ = instr_create();
                instr_set_type(instrEQ, IC_EQ_VAR);
                instr_add_dest(instrEQ, tmpDestEQ);
                instr_add_elem1(instrEQ, tokenAfterTop->data);
                instr_add_elem2(instrEQ, tokenTop->data);
                list_add(list, instrEQ);

                instr_t *instrNOT = instr_create();
                instr_set_type(instrNOT, IC_NOT_VAR);
                instr_add_dest(instrNOT, tmpDestRet);
                instr_add_elem1(instrNOT, tmpDestEQ);
                list_add(list, instrNOT);

                retExpr = tmpDestRet;

                //printf("\n\t\tRULE T_NEQ\tE != E\n");
                stack_pop(&symbolStack);
                stack_pop(&tokStack);
            } //IF 5 != E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $ != E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //printf("\n\t\tRULE\tE -> id\n");
                tokenAfterTop->expr = true;
            } //IF E != 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E != IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //printf("\n\t\tRULE\tE -> id\n");
                tokenTop->expr = true;
            }
            else
            {
                release_resources();
                error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            }

            break;
        default:
            release_resources();
            error(2, "expression parser", "reduce", "FAULTY INPUT EXPRESSION");
            ////printf("\n\t\tPARS ERROR\n\n");
            break;
        }
    }
    else
    {
        release_resources();
        error(2, "expression parser", "reduce", "debilko");
    }

    /*     // KONTROLA STACKU
    tmp2 = tokStack.topToken;
    while (tmp2->token.token_type != T_EMPTY)
    {
        //printf("\nREDUCE STACK VALUES BOTTOM\t%d", tmp2->token.token_type);
        tmp2 = tmp2->nextTok;
    }
    //printf("\nEND REDUCE\n"); */
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
    printf("\n\t\tRULE\t E -> ( E ) \n");

    get_next_token(parsData.token);
    return;
}

void release_resources()
{
    next.token_type = parsData.token->token_type;
    next.attr = parsData.token->attr;
    stack_free(&symbolStack);
    stack_free(&tokStack);
    free(parsData.token);
}

void print()
{

    stackElemPtr tmpT;
    // kontrola elementov nastacku
    tmpT = tokStack.topToken;
    while (tmpT->token.token_type != T_EMPTY)
    {
        printf("\nTOKEN SHIFT VALUES\t%d", tmpT->token.token_type);
        tmpT = tmpT->nextTok;
    }

    stackElemPtr tmp;
    // kontrola elementov nastacku
    tmp = symbolStack.topToken;
    while (tmp->token.token_type != T_EMPTY)
    {
        printf("\nSYMBOL SHIFT VALUES\t%d", tmp->token.token_type);
        tmp = tmp->nextTok;
    }
}

symtable_value_t parse_expression()
{
    //printf("\n\n-----START-----\n");
    parsData.token = (tToken *)malloc(sizeof(tToken));
    parsData.token->token_type = next.token_type;
    parsData.token->attr = next.attr;
    stack_init(&tokStack);
    stack_init(&symbolStack);
    //int i = 0;

    ////printf("PARS\t%d\n", tokStack.topToken->token.token_type);
    ////printf("PARS\t%d\n", symbolStack.topToken->token.token_type);

    tToken token;
    token.token_type = T_DOLLAR;
    stack_push(&tokStack, token);
    stack_push(&symbolStack, token);
    while (1)
    {

        int indexStack, indexInput = 0;

        indexInput = get_index(*parsData.token);
        indexStack = get_index(symbolStack.topToken->token);
        printf("\n\nToken\t%d\n", parsData.token->token_type);
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
            //release_resources();
            printf("\nERROR\n");
            print();

            if (symbolStack.topToken->token.token_type == T_DOLLAR)
            {
                next.token_type = parsData.token->token_type;
                next.attr = parsData.token->attr;
            }
            else
            {

                next.token_type = symbolStack.topToken->token.token_type;
                next.attr = symbolStack.topToken->token.attr;
            }

            //error(2, "expression parser", "parse_expression", "Faulty expression");
            stack_free(&symbolStack);
            stack_free(&tokStack);
            free(parsData.token);
            printf("\nReturn token\t%d\n", next.token_type);
            return retExpr;
            break;
        case A:
            printf("\nACCEPT\n");
            //print();

            /*             // Kontrolne vypisy
            ////printf("\nParse end tok stack\t%d", tokStack.topToken->token.token_type);
            ////printf("\nParse end symbol stack\t%d\n", symbolStack.topToken->token.token_type);
            stackElemPtr tmp;
            tmp = tokStack.topToken;
            while (tmp->token.token_type != T_EMPTY)
            {
                //printf("TOKEN STACK VALUES\t%d\n", tmp->token.token_type);
                tmp = tmp->nextTok;
            }
            tmp = symbolStack.topToken;
            while (tmp->token.token_type != T_EMPTY)
            {
                //printf("SYMBOL STACK VALUES\t%d\n", tmp->token.token_type);
                tmp = tmp->nextTok;
            } */

            //printf("\n-----END-----\n\n");
            //printf("\nTOKEN DATA\t%s\n", tokStack.topToken->data->symbol.sym_var_item->name);
            release_resources();
            if (retExpr == NULL)
            {
                // TODO ERROR
                error(99, "expression parser", "parse_expression", " Missing expression ");
            }
            return retExpr;
            break;
        default:

            release_resources();
            error(99, "expression parser", "parse_expression", " ???? ");
            break;
        }
        /*         print();
        i++;
        if (i == 25)
        {
            exit(1);
        } */
    }
}
