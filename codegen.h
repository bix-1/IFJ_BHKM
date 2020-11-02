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
	 * Variable declaration
	 *
	 * tab_elem1: symbol var
	 * tab_elem2: NULL
	 *
	 * Example:
	 * var x int
	 * var y float64
	 * var z string
	 * var b bool
	 */
	IC_DECL_VAR,

	/*
	 * Variable definition with init with var
	 *
	 * tab_elem1: symbol var
	 * tab_elem2: NULL
	 *
	 * Example:
	 * var x = y
	 * x := z
	 *
	 */
	IC_DEF_VAR,

	/*
	 * Variable definition with init with const
	 *
	 * tab_elem1: symbol const
	 * tab_elem2: NULL
	 *
	 * Example:
	 * const var x = 12
	 *
	 */
	IC_DEF_CONST,

	/*
	 * Function call with single/multiple values return
	 *
	 * tab_elem1: symbol var_list
	 * tab_elem2: symbol func
	 *
	 * Example:
	 * var x = some_fun()
	 * x := some_fun()
	 *
	 * var x, y, z = some_fun()
	 * x, y, z := some_fun()
	 *
	 * ==========================
	 * Note:
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
	 * Defines function start, parameter variables are inside symbol func
	 *
	 * tab_elem1: symbol func
	 * tab_elem2: NULL
	 *
	 * Example:
	 * some_fun()
	 */
	IC_CALL_FUN,

	/*
	 * Variable value set with var
	 *
	 * tab_elem1: symbol var
	 * tab_elem2: symbol var
	 *
	 * Example:
	 * x = y
	 *
	 */
	IC_MOV_VAR,

	/*
	 * Variable value set with const
	 *
	 * tab_elem1: symbol var
	 * tab_elem2: symbol const
	 *
	 * Example:
	 * x = 12
	 *
	 */
	IC_MOV_CONST,

	/*
	 * Variable add operation with var
	 *
	 * tab_elem1: symbol var (dest)
	 * tab_elem2: symbol var (src)
	 *
	 * Example:
	 * x += y
	 *
	 */
	IC_ADD_VAR,

	/*
	 * Variable add operation with const
	 *
	 * tab_elem1: symbol var (dest)
	 * tab_elem2: symbol const (src)
	 *
	 * Example:
	 * x += 12
	 *
	 */
	IC_ADD_CONST,

	/*
	 * Variable subtract operation with var
	 *
	 * tab_elem1: symbol var (dest)
	 * tab_elem2: symbol var (src)
	 *
	 * Example:
	 * x -= y
	 *
	 */
	IC_SUB_VAR,

	/*
	 * Variable subtract operation with const
	 *
	 * tab_elem1: symbol var (dest)
	 * tab_elem2: symbol var (src)
	 *
	 * Example:
	 * x -= 12
	 *
	 */
	IC_SUB_CONST,

	/*
	 * Variable multiply operation with var
	 *
	 * tab_elem1: symbol var (dest)
	 * tab_elem2: symbol var (src)
	 *
	 * Example:
	 * x *= y
	 *
	 */
	IC_MUL_VAR,

	/*
	 * Variable multiply operation with const
	 *
	 * tab_elem1: symbol var (dest)
	 * tab_elem2: symbol var (src)
	 *
	 * Example:
	 * x *= 12
	 *
	 */
	IC_MUL_CONST,

	/*
	 * Variable divide operation with variable. Must be both int or float
	 *
	 * tab_elem1: symbol var
	 * tab_elem2: symbol var
	 *
	 * Example:
	 * x /= y
	 */
	IC_DIV_VAR,

	/*
	 * Variable divide operation with const. Must be both int or float
	 *
	 * tab_elem1: symbol var
	 * tab_elem2: symbol var
	 *
	 * Example:
	 * x /= 12
	 */
	IC_DIV_CONST
};

#endif //CODEGEN_H
