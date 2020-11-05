/*
 * Project: Compiler for imperative programing language IFJ20
 *
 * File: codegen.h
 * Brief: Code generator header file
 *
 * Authors: Hladký Tomáš    xhladk15@stud.fit.vutbr.cz
 *          Kostolányi Adam xkosto04@stud.fit.vutbr.cz
 *          Makiš Jozef     xmakis00@stud.fit.vutbr.cz
 *          Bartko Jakub    xbartk07@stud.fit.vutbr.cz
 */
#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>

enum intermediate_code_instr {
	/*
	 *  Variables are stored in symtable in format main:<num_of_call><function>:<variable>
	 *  num_of_call and function can repeat with another function
	 *
	 *  num_of_call is need for multiple different function calls (different inputs)
	 *
	 *  Example for variable:
	 *  main:0init:var
	 *  main:0fun:0hello:var
	 *
	 *  Example for function:
	 *  main:0some_fun:0another_fun
	 *
	 *  Example for recursive function:
	 *  main:0hello:1hello:2hello
	 *
	 */

	/*
	 * Symbol constants:
	 *
	 * symbol var:   stores variable of int || float64 || string
	 * symbol const: stores const variables of int || float64 || string or its reference
	 *               to const in program, eg. 42
	 * symbol func:  stores function with its parameters and returns (variables or const)
	 *
	 */

	/*
	 * Variable/variables declaration
	 *
	 * elem1: symbol var
	 * elem2: NULL
	 *
	 * Example:
	 * var x int
	 * var y float64
	 * var z string
	 * var b bool
	 *
	 * ==========================
	 * Note:
	 * multiple variables in line are taken one by one
	 *
	 * var x, y, z int
	 *
	 * is rewritten to this:
	 *
	 * var x int
	 * var y int
	 * var z int
	 */
	IC_DECL_VAR,

	/*
	 * Variable/variables definition/assign with another var/vars or/and const/consts
	 *
	 * elem1: symbol var_list (dest)
	 * elem2: symbol var_list (src)
	 *
	 * Example:
	 * var x = a                // variable is also declared here
	 * var x, y = a, b          // variables are also declared here
	 *
	 * x := a                   // variable cannot be already declared
	 * x, y := a, b             // variables may or may not be declared
	 *
	 * x = a                    // variable must be already declared
	 * x, y = a, b              // variables must be already declared
	 *
	 * x = 42
	 *
	 * ==========================
	 * Note 1:
	 *
	 * If variable/variables were not declared, then IC_DECL_VAR is called first for
	 * each variable one by one.
	 *
	 * ==========================
	 * Note 2:
	 *
	 * symbol var_list is used because in multiple variables it's not clear
	 * if variable is var or const
	 *
	 */
	IC_DEF_VAR,

	/*
	 * Function call with single/multiple values return
	 *
	 * elem1: symbol var_list
	 * elem2: symbol func
	 *
	 * Example:
	 * var x = some_fun()           // variable is also declared here
	 * var x, y, z = some_fun()     // variables are also declared here
	 *
	 * x := some_fun()              // variable cannot be already declared
	 * x, y, z := some_fun()        // variables may or may not be declared
	 *
	 * x = some_fun()               // variable must be already declared
	 * x, y, z = some_fun()         // variables must be already declared
	 *
	 * ==========================
	 * Note 1:
	 *
	 * If variable/variables were not declared, then IC_DECL_VAR is called first for
	 * each variable one by one.
	 *
	 *
	 * ==========================
	 * Note 2:
	 * in case that some variables are defined and some not
	 *
	 * x = defined, y = undefined
	 *
	 * var x, y = some_fun()
	 * Result: x is redeclared = error
	 *
	 * x, y := some_fun()
	 * Result: valid but data type of x must be same as declared, value of x is reassigned.
	 *
	 */
	IC_DEF_FUN,

	/*
	 * Function call without assigning return value/values
	 *
	 * elem1: symbol func
	 * elem2: NULL
	 *
	 * Example:
	 * some_fun()
	 */
	IC_CALL_FUN,

	/*
	 * Variable add operation with var
	 *
	 * elem1: symbol var (dest)
	 * elem2: symbol var (src)
	 *
	 * Example:
	 * x += y
	 *
	 */
	IC_ADD_VAR,

	/*
	 * Variable add operation with const
	 *
	 * elem1: symbol var (dest)
	 * elem2: symbol const (src)
	 *
	 * Example:
	 * x += 12
	 *
	 */
	IC_ADD_CONST,

	/*
	 * Variable subtract operation with var
	 *
	 * elem1: symbol var (dest)
	 * elem2: symbol var (src)
	 *
	 * Example:
	 * x -= y
	 *
	 */
	IC_SUB_VAR,

	/*
	 * Variable subtract operation with const
	 *
	 * elem1: symbol var (dest)
	 * elem2: symbol var (src)
	 *
	 * Example:
	 * x -= 12
	 *
	 */
	IC_SUB_CONST,

	/*
	 * Variable multiply operation with var
	 *
	 * elem1: symbol var (dest)
	 * elem2: symbol var (src)
	 *
	 * Example:
	 * x *= y
	 *
	 */
	IC_MUL_VAR,

	/*
	 * Variable multiply operation with const
	 *
	 * elem1: symbol var (dest)
	 * elem2: symbol var (src)
	 *
	 * Example:
	 * x *= 12
	 *
	 */
	IC_MUL_CONST,

	/*
	 * Variable divide operation with variable. Must be both int or float
	 *
	 * elem1: symbol var
	 * elem2: symbol var
	 *
	 * Example:
	 * x /= y
	 */
	IC_DIV_VAR,

	/*
	 * Variable divide operation with const. Must be both int or float
	 *
	 * elem1: symbol var
	 * elem2: symbol var
	 *
	 * Example:
	 * x /= 12
	 */
	IC_DIV_CONST
};

#endif //CODEGEN_H
