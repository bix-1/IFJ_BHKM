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

// TODO
//  a:= "test1"+"test2" unitialized
/*   a:= "test1"
  b:= "test2"
  d:= "test3"
  c:= a+(b+c)       malloc */

// TODO INT2FLOAT neposlat do codegen ak to je uz float
// INT2FLOAT v relacnych
// INT2CHAR alias vstavana funkcia
// BOOL

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
    {R, S, S, R, R, S, S, R},           // +-
    {R, R, S, R, R, S, Err, R},         // */
    {S, S, S, Eq, S, S, Err, Err},      // (
    {R, R, Err, R, R, Err, Err, R},     // )
    {S, S, S, R, Err, S, Err, R},       // RC
    {R, R, Err, R, R, Err, Err, R},     // i
    {R, Err, Err, Err, R, Err, Err, R}, // STRING
    {S, S, S, Err, S, S, S, A},         // $

};

#define T_DOLLAR 500 // STACK BOTTOM
#define T_EXPR 501   // EXPRESSION

tToken exprToken;       // Expression token
tokenStack * symbolStack = NULL; // Symbol stack
tokenStack * tokStack = NULL;    // Token stack
int exprNumber = 0;
symtable_value_t retExpr;
tPData * parsData = NULL;
bool eps;
bool ret;
bool was_func = false;

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
    case T_TRUE:
    case T_FALSE:
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

void check_func() {
  // stash ID
  char *id = to_string(parsData->token);
  // check for func
  symtable_value_t func = try_func(parsData->token);

  if (func == NULL) { // recovery
    // check if defined
    eps = false;
    id_find(scope_get_head(), id);
    // stash following token
    int ttype = parsData->token->token_type;
    // add stashed ID
    parsData->token->token_type = T_IDENTIFIER;
    parsData->token->attr.str_lit.str = id;
    stack_push(tokStack, *parsData->token);
    check_symtable(tokStack->topToken);
    // restore following token
    parsData->token->token_type = ttype;
  }
  else { // adding func call as expression
    // stash following token
    int ttype = parsData->token->token_type;

    // create key, name & token's name for dest variable
    char *idExpr = create_id();
    char *id_scope = id_add_scope(scope_get_head(), idExpr);

    // create dest variable for func's return
    sym_var_item_t *var_item = sym_var_item_init(id_scope);
    sym_var_item_set_type(var_item, VAR_UNDEFINED);
    symbol_t var_sym = {.sym_var_item = var_item};
    elem_t *var = elem_init(SYM_VAR_ITEM, var_sym);
    symtable_insert(symtable, id_scope, var);
    instr_add_var_decl(var);
    var_item->is_defined = true;

    // add func call instruction
    instr_type_t type = get_func_instr_type(func->symbol.sym_func->name);
    instr_add_func_call(func, type);
    // link func call to the rest of the expression
    func_add_ret(func, var_item);

    // add dest variable to stack
    parsData->token->token_type = T_IDENTIFIER;
    parsData->token->attr.str_lit.str = idExpr;
    stack_push(tokStack, *parsData->token);
    check_symtable(tokStack->topToken);

    // restore following token
    parsData->token->token_type = ttype;
  }
}

void check_func_start()
{
    if (parsData->token->token_type != T_IDENTIFIER)
    {
        was_func = false;
        return;
    }

    char *id = to_string(parsData->token);
    symtable_value_t func = try_func(parsData->token);
    if (func != NULL)
    {
        int index = get_index(*parsData->token);
        if (index == OP_dollar || index == OP_parenth_close)
        {
            retExpr = func; // a = foo()
            was_func = true;
            return;
        }
        else
        {
            int ttype = parsData->token->token_type;

            char *idExpr = create_id();
            char *id_scope = id_add_scope(scope_get_head(), idExpr);

            // create variable and add to symtable
            sym_var_item_t *var_item = sym_var_item_init(id_scope);
            sym_var_item_set_type(var_item, VAR_UNDEFINED);
            symbol_t var_sym = {.sym_var_item = var_item};
            elem_t *var = elem_init(SYM_VAR_ITEM, var_sym);
            symtable_insert(symtable, id_scope, var);
            instr_add_var_decl(var);
            var_item->is_defined = true;

            instr_type_t type = get_func_instr_type(func->symbol.sym_func->name);
            instr_add_func_call(func, type);

            func_add_ret(func, var_item);

            parsData->token->token_type = T_IDENTIFIER;
            parsData->token->attr.str_lit.str = idExpr;
            stack_push(tokStack, *parsData->token);
            check_symtable(tokStack->topToken);


            parsData->token->token_type = ttype;
        }
    }
    else
    {
        eps = false;
        id_find(scope_get_head(), id);

        int ttype = parsData->token->token_type;

        parsData->token->token_type = T_IDENTIFIER;
        parsData->token->attr.str_lit.str = id;
        stack_push(tokStack, *parsData->token);
        check_symtable(tokStack->topToken);

        parsData->token->token_type = ttype;
    }
}

void check_symtable(stackElemPtr elem)
{
    //printf("\nCHECK_SYMTABLE ORIGINAL TYPE\t%d\n", elem->originalType);
    if (elem->originalType == T_IDENTIFIER)
    {
        // Variable already existing
        eps = false;
        elem->data = id_find(scope_get_head(), to_string(&(elem->token)));
        char ** str = &(parsData->token->attr.str_lit.str);
        if (*str != NULL) {
          free(*str);
          *str = NULL;
        }
    }
    else
    {
        // Variable non-existent -> create it
        elem->data = create_variable(elem);
        //printf("\nSymtable\t%s\n", elem->data->symbol.sym_var_item->data.string_t);
    }

    // If we just define... f.e. a:=5
    retExpr = elem->data;
}

symtable_value_t create_dest(stackElemPtr elem)
{
    char *id = create_id();
    char *id_scope = id_add_scope(scope_get_head(), id);
    free(id);

    // create variable and add to symtable
    sym_var_item_t *var_item = sym_var_item_init(id_scope);
    sym_var_item_set_type(var_item, elem->data->symbol.sym_var_item->type);
    symbol_t var_sym = {.sym_var_item = var_item};
    elem_t *var = elem_init(SYM_VAR_ITEM, var_sym);
    symtable_insert(symtable, id_scope, var);
    instr_add_var_decl(var);
    var_item->is_defined = true;

    return var;
}

symtable_value_t create_variable(stackElemPtr elem)
{
    char *id = create_id();
    char *id_scope = id_add_scope(scope_get_head(), id);
    free(id);
    // create variable and add to symtable
    sym_var_item_t *var_item = sym_var_item_init(id_scope);
    sym_var_item_set_type(var_item, var_type_check(elem));
    sym_var_item_set_const(var_item, true);

    variable_t variable;
    switch (elem->token.token_type)
    {
    case T_INT_VALUE:
        variable.int_t = to_int(&(elem->token));
        break;
    case T_DEC_VALUE:
        variable.float64_t = to_double(&(elem->token));
        break;
    case T_STRING_VALUE:
        variable.string_t = to_string(&(elem->token));
        break;
    case T_TRUE:
        variable.bool_t = true;
        break;
    case T_FALSE:
        variable.bool_t = false;
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
    // id = 0Express = exprNumber + "expr" + \0
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
    strcpy(id, number);
    free(number);
    strcat(id, "expr");
    exprNumber++;
    return id;
}

int var_type_check(stackElemPtr elem)
{

    switch (elem->token.token_type)
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
    case T_FALSE:
        return VAR_BOOL;
        break;
    default:
        //printf("\ntype\n");
        release_resources();
        error(99, "expression parser", "var_type_check", "Symtable variable creation failed");
        break;
    }

    release_resources();
    error(99, "expression parser", "var_type_check", "Symtable variable creation failed");
    return 1;
}

void check_string(stackElemPtr top, stackElemPtr afterTop, tToken *symbol)
{
    // ERROR if both expressions are not strings
    if ((top->data->symbol.sym_var_item->type == VAR_STRING && afterTop->data->symbol.sym_var_item->type != VAR_STRING) || (top->data->symbol.sym_var_item->type != VAR_STRING && afterTop->data->symbol.sym_var_item->type == VAR_STRING))
    {
        release_resources();
        error(5, "expression parser", "check_string", "Variable types do not match");
    }

    // ERROR if strings are operated with -,*,/ (string concatenation)
    if (top->data->symbol.sym_var_item->type == VAR_STRING && afterTop->data->symbol.sym_var_item->type == VAR_STRING && (symbol->token_type == T_MINUS || symbol->token_type == T_MUL || symbol->token_type == T_DIV))
    {
        release_resources();
        error(2, "expression parser", "check_string", "FAULTY INPUT EXPRESSION");
    }
}

void check_num(stackElemPtr top, stackElemPtr afterTop)
{

    if ((top->data->symbol.sym_var_item->type == VAR_INT && afterTop->data->symbol.sym_var_item->type == VAR_FLOAT64) || (top->data->symbol.sym_var_item->type == VAR_FLOAT64 && afterTop->data->symbol.sym_var_item->type == VAR_INT))
    {
        release_resources();
        error(5, "expression parser", "check_string", "Variable types do not match");
    }
}

void check_expr(tToken *token)
{
    // If token stack is empty
    if (token->token_type == T_DOLLAR)
    {
        release_resources();
        error(2, "expression parser", "reduce", "Missing expression at operation");
    }
}

void shift()
{
    // IF WE WANT TO PUSH NUMBERS
    if (get_index(*parsData->token) == OP_value || get_index(*parsData->token) == OP_STRING)
    {
        // two elements without separating operation
        if (
          get_index(symbolStack->topToken->token) == OP_dollar &&
          get_index(tokStack->topToken->token) != OP_dollar
        ) {
          ret = true;
          retExpr = tokStack->topToken->data;
          release_resources();
          return;
        }

        if (parsData->token->token_type == T_IDENTIFIER) {
          check_func();
          return;
        } else {
          stack_push(tokStack, *parsData->token);
          check_symtable(tokStack->topToken);
        }

        //print();

    } // IF WE WANT TO PUSH SYMBOLS
    else if (get_index(*parsData->token) == OP_plus_minus || get_index(*parsData->token) == OP_mult_div || get_index(*parsData->token) == OP_parenth_close || get_index(*parsData->token) == OP_parenth_open || get_index(*parsData->token) == OP_rel_comp || get_index(*parsData->token) == OP_dollar)
    {
        stack_push(symbolStack, *parsData->token);
        get_next_token(parsData->token);

        if (
          get_index(symbolStack->topToken->token) != OP_dollar &&
          parsData->token->token_type == T_EOL
        ) {
          next.token_type = parsData->token->token_type;
          next.attr = parsData->token->attr;
          skip_empty();
          parsData->token->token_type = next.token_type;
          parsData->token->attr = next.attr;
        }
        ret = true;
    }
    else
    {
        release_resources();
        error(2, "expression parser", "reduce", "Missing an expression");
    }

    if (!ret) get_next_token(parsData->token);
    else ret = false;

    // ERROR if '(' is followed by ')' without expression ... f.e.  a= 5+5*()5
    if (
      symbolStack->topToken->token.token_type == T_L_BRACKET &&
      parsData->token->token_type == T_R_BRACKET &&
      get_index(tokStack->topToken->token) == OP_dollar
    )
    {
        release_resources();
        error(2, "expression parser", "reduce", "Missing an expression");
    }
}

void int_to_float(stackElemPtr elem)
{

    instr_t *instrI2F = instr_create();
    instr_set_type(instrI2F, IC_INT2FLOAT_VAR);
    instr_add_dest(instrI2F, elem->data);
    instr_add_elem1(instrI2F, elem->data);
    list_add(list, instrI2F);
}

void reduce()
{
    print();

    stackElemPtr tokenTop = tokStack->topToken;               // Top member on VALUE stack
    stackElemPtr tokenAfterTop = tokStack->topToken->nextTok; // Second from the top member on VALUE stack
    tToken symbolTop = symbolStack->topToken->token;          // Top member on SYMBOL stack
                                                             // printf("\nERR\t%p\t%p\n", tokenTop->data, tokenAfterTop->data);
                                                             // printf("\nHHERE\n");

    if (tokenTop->data == NULL || tokenAfterTop->data == NULL)
    {
        release_resources();
        error(2, "expression parser", "reduce", "Missing an expression");
    }

    // If value stack isn't empty
    if (tokStack->topToken->token.token_type != T_DOLLAR)
    {
        check_string(tokenTop, tokenAfterTop, &symbolTop);

        check_num(tokenTop, tokenAfterTop);

        switch (symbolTop.token_type)
        {
        case T_PLUS:
            //IF E + E IS ON STACK
            if (tokenTop->expr == true && tokenAfterTop->expr == true)
            {

                check_types(
                  tokenTop->data->symbol.sym_var_item,
                  tokenAfterTop->data->symbol.sym_var_item
                );
                symtable_value_t dest = create_dest(tokenTop);
                check_types(
                  dest->symbol.sym_var_item,
                  tokenTop->data->symbol.sym_var_item
                );

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

                stack_pop(symbolStack);
                stack_pop(tokStack);
            } //IF 5 + E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                //// printf("\n\t\tRULE +\tE -> id\n");
                // IF $+E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                tokenAfterTop->expr = true;

            } //IF E + 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E+ IS ON STACK
                //// printf("\n\t\tRULE +\tE -> id\n");
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
                //// printf("\n\t\tRULE T_MINUS\tE = E - E\n");

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

                stack_pop(symbolStack);
                stack_pop(tokStack);
            } //IF 5 - E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $-E IS ON STACK
                check_expr(&(tokenAfterTop->token));

                //// printf("\n\t\tRULE -\tE -> id\n");
                tokenAfterTop->expr = true;

            } //IF E - 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E- IS ON STACK
                check_expr(&(tokenTop->token));

                // TO DO INSERT INSTRUCTIONS
                //// printf("\n\t\tRULE -\tE -> id\n");
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
                //// printf("\n\t\tRULE T_MUL\tE = E * E\n");

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

                stack_pop(symbolStack);
                stack_pop(tokStack);
            } //IF 5 * E IS ON STACK
            else if (tokenAfterTop->expr == false)
            {
                // IF $*E IS ON STACK
                //// printf("\n\t\tRULE *\tE -> id\n");
                check_expr(&(tokenAfterTop->token));

                tokenAfterTop->expr = true;
            } //IF E * 5 IS ON STACK
            else if (tokenTop->expr == false)
            {
                // IF E* IS ON STACK
                //// printf("\n\t\tRULE *\tE -> id\n");
                check_expr(&(tokenTop->token));

                tokenTop->expr = true;
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

                stack_pop(symbolStack);
                stack_pop(tokStack);
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

                symtable_value_t dest = create_dest(tokenTop);
                //printf("\n\ndest\t%s", *tokenAfterTop->data->key);
                //printf("\n DEST EXPRESSION,elem1,elem2\t %s\t%s\t%s\n", dest->symbol.sym_var_item->name, tokenTop->data->symbol.sym_var_item->name, tokenAfterTop->data->symbol.sym_var_item->name);

                // INSTRUCTIONS
                instr_t *instrLT = instr_create();
                instr_set_type(instrLT, IC_LT_VAR);
                instr_add_dest(instrLT, dest);
                instr_add_elem1(instrLT, tokenAfterTop->data);
                instr_add_elem2(instrLT, tokenTop->data);
                list_add(list, instrLT);

                tokenAfterTop->data = dest;
                retExpr = dest;

                stack_pop(symbolStack);
                stack_pop(tokStack);
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

                stack_pop(symbolStack);
                stack_pop(tokStack);
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

                stack_pop(symbolStack);
                stack_pop(tokStack);
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

                stack_pop(symbolStack);
                stack_pop(tokStack);
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
                stack_pop(symbolStack);
                stack_pop(tokStack);
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
                symtable_value_t tmpDestEQ = create_dest(tokenTop);
                symtable_value_t tmpDestRet = create_dest(tokenTop);

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
                stack_pop(symbolStack);
                stack_pop(tokStack);
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
            break;
        }
    }
    else
    {
        release_resources();
        error(2, "expression parser", "reduce", "debilko");
    }

    /*     // KONTROLA STACKU
    tmp2 = tokStack->topToken;
    while (tmp2->token.token_type != T_EMPTY)
    {
        //printf("\nREDUCE STACK VALUES BOTTOM\t%d", tmp2->token.token_type);
        tmp2 = tmp2->nextTok;
    }
    //printf("\nEND REDUCE\n"); */
}

void equal()
{
    if (symbolStack->topToken->token.token_type != T_DOLLAR)
    {
        stack_pop(symbolStack);
    }
    else
    {
        release_resources();
        error(2, "expression parser", "reduce", "Missing left parenthesis");
    }
    ////// printf("\n\t\tRULE\t E -> ( E ) \n");

    get_next_token(parsData->token);
    return;
}

void release_resources()
{
    if (parsData != NULL) {
      if (parsData->token != NULL) {
        next.token_type = parsData->token->token_type;
        next.attr = parsData->token->attr;
        free(parsData->token);
        parsData->token = NULL;
      }
      free(parsData);
      parsData = NULL;
    }
    stack_free(&symbolStack);
    stack_free(&tokStack);

    if (frame_stack.first_pd != NULL)
      frame_stack_pop();
    else
      frame_stack_destroy();
}

void print()
{

    stackElemPtr tmpT;
    // kontrola elementov na stacku
    tmpT = tokStack->topToken;
    while (tmpT->token.token_type != T_EMPTY)
    {
        // printf("\nTOKEN SHIFT VALUES\t%d", tmpT->token.token_type);
        tmpT = tmpT->nextTok;
    }

    stackElemPtr tmp;
    // kontrola elementov nastacku
    tmp = symbolStack->topToken;
    while (tmp->token.token_type != T_EMPTY)
    {
        // printf("\nSYMBOL SHIFT VALUES\t%d", tmp->token.token_type);
        tmp = tmp->nextTok;
    }
}




void frame_stack_init() {
  frame_stack.first_pd = NULL;
  frame_stack.first_tok = NULL;
  frame_stack.first_sym = NULL;
}

void frame_stack_destroy() {
  pd_item_t * next1;
  for (
    pd_item_t * it = frame_stack.first_pd;
    it != NULL;
    it = next1
  ) {
    next1 = it->next;
    free(it);
  }

  stack_item_t * next2;
  for (
    stack_item_t * it = frame_stack.first_tok;
    it != NULL;
    it = next2
  ) {
    next2 = it->next;
    free(it);
  }

  stack_item_t * next3;
  for (
    stack_item_t * it = frame_stack.first_sym;
    it != NULL;
    it = next3
  ) {
    next3 = it->next;
    free(it);
  }

  frame_stack_init();
}

void frame_stack_push() {
  pd_item_t * new1 = malloc(sizeof(pd_item_t));
  stack_item_t * new2 = malloc(sizeof(stack_item_t));
  stack_item_t * new3 = malloc(sizeof(stack_item_t));

  new1->pd = parsData;
  new2->ts = tokStack;
  new3->ts = symbolStack;

  new1->next = frame_stack.first_pd;
  new2->next = frame_stack.first_tok;
  new3->next = frame_stack.first_sym;

  frame_stack.first_pd = new1;
  frame_stack.first_tok = new2;
  frame_stack.first_sym = new3;
}

void frame_stack_pop() {
  if (frame_stack.first_pd == NULL) return;

  if (parsData != NULL) {
    if (parsData->token != NULL) {
      next.token_type = parsData->token->token_type;
      next.attr = parsData->token->attr;
      free(parsData->token);
      parsData->token = NULL;
    }
    free(parsData);
    parsData = NULL;
  }
  stack_free(&symbolStack);
  stack_free(&tokStack);


  parsData = frame_stack.first_pd->pd;
  tokStack = frame_stack.first_tok->ts;
  symbolStack = frame_stack.first_sym->ts;

  pd_item_t * tmp1 = frame_stack.first_pd;
  stack_item_t * tmp2 = frame_stack.first_tok;
  stack_item_t * tmp3 = frame_stack.first_sym;

  frame_stack.first_pd = frame_stack.first_pd->next;
  frame_stack.first_tok = frame_stack.first_tok->next;
  frame_stack.first_sym = frame_stack.first_sym->next;



  free(tmp1);
  free(tmp2);
  free(tmp3);
}


symtable_value_t parse_expression()
{
    if (get_index(next) == OP_dollar)
    {
        error(2, "expression parser", NULL, "Missing an expression");
    }

    if (parsData != NULL) {
      frame_stack_push();
    }

    parsData = malloc(sizeof(tPData));

    parsData->token = (tToken *)malloc(sizeof(tToken));
    parsData->token->token_type = next.token_type;
    parsData->token->attr = next.attr;

    stack_init(&tokStack);
    stack_init(&symbolStack);

    tToken token;
    token.token_type = T_DOLLAR;
    stack_push(tokStack, token);
    stack_push(symbolStack, token);

    check_func_start();

    if (was_func == true)
    {
      release_resources();
      was_func = false;
      return retExpr;
    }

    while (1)
    {
        int indexStack, indexInput;

        indexInput = get_index(*parsData->token);
        indexStack = get_index(symbolStack->topToken->token);

        switch (precTable[indexStack][indexInput])
        {
        case R: /*>*/
            // printf("\nREDUCE");
            reduce();
            break;
        case S: /*<*/
            // printf("\nSHIFT\n");
            shift();
            if (ret) {
              ret = false;
              return retExpr;
            }
            break;
        case Eq: /*=*/
            //printf("\nEQUAL");
            equal();
            //print();
            break;
        case Err: /* empty*/
            // print();

            if (symbolStack->topToken->token.token_type == T_DOLLAR)
            {
                next.token_type = parsData->token->token_type;
                next.attr = parsData->token->attr;
            }
            /* else if (tokStack->topToken->originalType == T_STRING_VALUE)
            {
                // ERROR a = string * string || a = string / string or anything starting with string followed by operation
                free(parsData->token->attr.str_lit.str);
                parsData->token->attr.str_lit.str = NULL;
                release_resources();
                error(5, "expression parser", "parse_expression", "Invalid operation with strings");
                // alebo Faulty input or sth like that
            } */
            else
            {
                // f.e a := foo(3)... ')' is left on symbolStack so I pass it to parser
                next.token_type = symbolStack->topToken->token.token_type;
                next.attr = symbolStack->topToken->token.attr;
            }

            stack_free(&symbolStack);
            stack_free(&tokStack);
            free(parsData->token);
            parsData->token = NULL;
            frame_stack_pop();
            return retExpr;
            break;
        case A:
            //printf("\nACCEPT\n");
            //print();

            /*             // Kontrolne vypisy
            ////printf("\nParse end tok stack\t%d", tokStack->topToken->token.token_type);
            ////printf("\nParse end symbol stack\t%d\n", symbolStack->topToken->token.token_type);
            stackElemPtr tmp;
            tmp = tokStack->topToken;
            while (tmp->token.token_type != T_EMPTY)
            {
                //printf("TOKEN STACK VALUES\t%d\n", tmp->token.token_type);
                tmp = tmp->nextTok;
            }
            tmp = symbolStack->topToken;
            while (tmp->token.token_type != T_EMPTY)
            {
                //printf("SYMBOL STACK VALUES\t%d\n", tmp->token.token_type);
                tmp = tmp->nextTok;
            } */

            // if (get_index(tokStack->topToken->token) != OP_dollar) {
            //   error(
            //     2, "expression parser", NULL, "Invalid expression"
            //   );
            // }

            //printf("\n-----END-----\n\n");
            //printf("\nTOKEN DATA\t%s\n", tokStack->topToken->data->symbol.sym_var_item->name);
            release_resources();
            if (retExpr == NULL)
            {
                // TODO ERROR
                error(2, "expression parser", "parse_expression", " Missing expression ");
            }
            //printf("\nAccept\t%s\n", retExpr->symbol.sym_var_item->data.string_t);


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
