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
#include <string.h>
#include <stdbool.h>

#include "symtable.h"
#include "error.h"
#include "escape_format.h"

#include "codegen_stack.h"

typedef struct instruction instr_t;

// Define intermediate code instructions
typedef enum instr_type {
	/*
	 * Variables are stored in symtable in format <function>:<num_of_call><function>:<variable>
	 * num_of_call and function can repeat with another function
	 *
	 * num_of_call is need for multiple different function calls (different inputs)
	 *
	 * Example for variable:
	 * main:0init:var
	 * bar:0fun:0hello:var
	 *
	 * Example for function:
	 * main:0some_fun:0another_fun
	 *
	 * Example for recursive function:
	 * main:0hello:1hello:2hello
	 *
	 */

	/*
	 * Symbol types:
	 *
	 * symbol var:   stores variable of int || float64 || string
	 * symbol const: stores const variables of int || float64 || string or its reference
	 *               to const in program, eg. 42
	 *               Update: this is not actually symbol, but represent const boolean stored in var
	 * symbol func:  stores function with its parameters and returns (variables or const)
	 *
	 */

	// ===================== DECL/DEFINE INSTRUCTIONS =====================

	/*
	 * Variable/variables declaration
	 *
	 * elem_dest: symbol var
	 * elem_1: NULL
	 * elem_2: NULL
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
	 * elem_dest: symbol var_list (dest)
	 * elem_1: symbol var_list (src)
	 * elem_2: NULL
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

	// ===================== FUNCTION INSTRUCTIONS =====================

	/*
	 * Function definition indicator, instructions after creates separate
	 * block with label to jump on call
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * Example:
	 * fun_foo() {
	 *  ...
	 * }
	 *
	 */
	IC_DEF_FUN,

	/*
	 * Function call with or without single/multiple values return
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
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
	 * ========================
	 * Note 2:
	 *
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
	 *
	 * ========================
	 * Note 3:
	 *
	 * Destination variable names are stored in returns
	 *
	 */
	IC_CALL_FUN,

	/*
	 * Function return call, only indicates end of current function
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * Example:
	 * return
	 * return x
	 * return x, y, z, 42
	 *
	 * ==========================
	 * Note:
	 *
	 * Need symbol func for name of func, for main
	 * Return parameters are stored in symbol func and they will be set to variables from
	 * IC_CALL_FUN in case that function has assigned at least 1 return value.
	 * Otherwise it was called without assigning return value.
	 *
	 */
	IC_RET_FUN,

	/*
	 * Helper instruction. Indicates end of function definition.
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * ==========================
	 * Note:
	 *
	 * Need symbol func for name of func, for main
	 *
	 */
	IC_END_FUN,

	// ===================== ARITHMETIC INSTRUCTIONS =====================

	/*
	 * Variable add operation with var
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: symbol var || symbol const (src2)
	 *
	 * Example:
	 * x = a + b
	 * x = a + 42
	 * x += c (x = x + c)
	 *
	 * ==========================
	 * Note:
	 *
	 * If src elements are not same data type - Error
	 *
	 */
	IC_ADD_VAR,

	/*
	 * Variable subtract operation with var
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: symbol var || symbol const (src2)
	 *
	 * Example:
	 * x = a - b
	 * x = a - 42
	 * x -= c (x = x - c)
	 *
	 * ==========================
	 * Note:
	 *
	 * If src elements are not same data type - Error
	 *
	 */
	IC_SUB_VAR,

	/*
	 * Variable multiply operation with var
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: symbol var || symbol const (src2)
	 *
	 * Example:
	 * x = a * b
	 * x = a * 42
	 * x *= c (x = x * c)
	 *
	 * ==========================
	 * Note:
	 *
	 * If src elements are not same data type - Error
	 *
	 */
	IC_MUL_VAR,

	/*
	 * Variable divide operation with variable. Must be both int or float
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: symbol var || symbol const (src2)
	 *
	 * Example:
	 * x = a / b
	 * x = a / 42
	 * x /= c (x = x / c)
	 *
	 * ==========================
	 * Note:
	 *
	 * If both src elements are float - DIV instruction will be called
	 * If both src elements are int - IDIV instruction will be called
	 * If src elements are not same data type - Error
	 *
	 */
	IC_DIV_VAR,

	// ===================== LOGICAL RELATION INSTRUCTIONS =====================

	/*
	 * Variable/const less than other variable/const
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: symbol var || symbol const (src2)
	 *
	 * Example:
	 * x = a < b
	 * x = 24 < b
	 * x = a < 42
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not bool - Error
	 * If src elements are not same data type - Error
	 *
	 */
	IC_LT_VAR,

	/*
	 * Variable/const greater than other variable/const
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: symbol var || symbol const (src2)
	 *
	 * Example:
	 * x = a > b
	 * x = 24 > b
	 * x = a > 42
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not bool - Error
	 * If src elements are not same data type - Error
	 *
	 */
	IC_GT_VAR,

	/*
	 * Variable/const equal other variable/const
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: symbol var || symbol const (src2)
	 *
	 * Example:
	 * x = a == b
	 * x = 24 == b
	 * x = a == 42
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not bool - Error
	 * If src elements are not same data type - Error
	 *
	 */
	IC_EQ_VAR,

	// ===================== LOGICAL BOOLEAN INSTRUCTIONS =====================

	/*
	 * Logical AND upon 2 variables
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: symbol var || symbol const (src2)
	 *
	 * Example:
	 * x = a && b
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not bool - Error
	 * If src elements are not bool - Error
	 *
	 */
	IC_AND_VAR,

	/*
	 * Logical OR upon 2 variables
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: symbol var || symbol const (src2)
	 *
	 * Example:
	 * x = a || b
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not bool - Error
	 * If src elements are not bool - Error
	 *
	 */
	IC_OR_VAR,

	/*
	 * Logical NOT upon 1 variable
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var || symbol const (src1)
	 * elem_2: NULL
	 *
	 * Example:
	 * x = !a
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not bool - Error
	 * If src element is not bool - Error
	 *
	 */
	IC_NOT_VAR,

	// ===================== TYPE CASTING INSTRUCTIONS =====================

	/*
	 * Int to float type cast
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 1: [symbol var || symbol const (src)]
	 *                  returns -> 1: [symbol var (dest)]
	 *
	 * Example:
	 * x = int2float(a)
	 * x = int2float(42)
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not float - Error
	 * If src element is not int - Error
	 *
	 */
	IC_INT2FLOAT_VAR,

	/*
	 * Float to int type cast
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 1: [symbol var || symbol const (src)]
	 *                  returns -> 1: [symbol var (dest)]
	 *
	 * Example:
	 * x = float2int(a)
	 * x = float2int(12.761)
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not int - Error
	 * If src element is not float - Error
	 *
	 */
	IC_FLOAT2INT_VAR,

	/*
	 * Get character (ascii 0-255) from string on selected index position
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 2: [symbol var || symbol const (src1),
	 *                                 symbol var || symbol const (src2)]
	 *                  returns -> 2: [symbol var (dest), symbol var (err)]
	 *
	 * Example:
	 * x = str2int(a, b)
	 * x = str2int(a, 2)
	 *
	 * ==========================
	 * Note 1:
	 *
	 * If dest element is not int - Error
	 * If err element is not int - Error
	 * If src1 element is not string - Error
	 * If src2 element is not int - Error
	 * If src2 int value is out of index of src1 string - Error
	 * If dest int value is out of interval <0,255> - Error
	 *
	 * ==========================
	 * Note 2:
	 *
	 * If index out of bounds, err is set to 1 and returned
	 *
	 */
	IC_STR2INT_VAR,

	// ===================== I/O INSTRUCTIONS =====================

	/*
	 * Read variable from build in functions
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 0: []
	 *                  returns -> 1: [symbol var (dest)]
	 *
	 * Example:
	 * x = inputi(22)
	 *
	 *
	 */
	IC_READ_INT,

	/*
	 * Read variable from build in functions
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 0: []
	 *                  returns -> 1: [symbol var (dest)]
	 *
	 * Example:
	 * x = inputf(22.2)
	 *
	 *
	 */
	IC_READ_FLOAT,

	/*
	 * Read variable from build in functions
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 0: []
	 *                  returns -> 1: [symbol var (dest)]
	 *
	 * Example:
	 * x = inputi("HelloWorld)
	 *
	 *
	 */
	IC_READ_STRING,

	/*
	 * Read variable from build in functions
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 0: []
	 *                  returns -> 1: [symbol var (dest)]
	 *
	 * Example:
	 * x = inputi(true)
	 *
	 *
	 */
	IC_READ_BOOL,

	/*
	 * Write variables to output from build in functions
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> n: [symbol var list]
	 *                  returns -> 0: []
	 *
	 * Example:
	 * x = print("Hello world!")
	 * x = print("Result number: ", some_var, "\n")
	 *
	 *
	 */
	IC_WRITE_VAR,

	// ===================== STRING INSTRUCTIONS =====================

	/*
	 * Concatenate two strings to one
	 *
	 * elem_dest: symbol var (dest)
	 * elem_1: symbol var (string1)
	 * elem_2: symbol var (string2)
	 *
	 * symbol func:     params  -> 2: [symbol var || symbol const (src1),
	 *                                 symbol var || symbol const (src2)]
	 *                  returns -> 1: [symbol var (dest)]
	 *
	 * Example:
	 * x = concat("Hello ", "world!")
	 * x = concat(a, b)
	 * x = "string1" + "string2"
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not string - Error
	 * If src1 element is not string - Error
	 * If src2 element is not string - Error
	 *
	 */
	IC_CONCAT_STR,

	/*
	 * Get length of string
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 1: [symbol var || symbol const (src)]
	 *                  returns -> 1: [symbol var (dest)]
	 *
	 * Example:
	 * x = strlen("Hello")
	 * x = strlen(a)
	 *
	 * ==========================
	 * Note:
	 *
	 * If dest element is not int - Error
	 * If src1 element is not string - Error
	 *
	 */
	IC_STRLEN_STR,

	/*
	 * Get char from string on specific index
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 2: [symbol var || symbol const (src1),
	 *                                 symbol var || symbol const (src2)]
	 *                  returns -> 2: [symbol var (dest), symbol var (err)]
	 *
	 * Example:
	 * x = getc("Hello", 2)
	 * x = getc(a, 2)
	 *
	 * ==========================
	 * Note 1:
	 *
	 * If dest element is not string - Error
	 * If err element is not int - Error
	 * If src1 element is not string - Error
	 * If src2 element is not int - Error
	 * If src2 int value is out of index of src1 string - Error
	 *
	 * ==========================
	 * Note 2:
	 *
	 * If index out of bounds, err is set to 1 and returned
	 *
	 */
	IC_GETCHAR_STR,

	/*
	 * Returns substring of some string based on params
	 *
	 * elem_dest: symbol func
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 * symbol func:     params  -> 3: [symbol var || symbol const (src1),
	 *                                 symbol var || symbol const (src2),
	 *                                 symbol var || symbol const (src3)]
	 *                  returns -> 2: [symbol var (dest), symbol var (err)]
	 *
	 * Example:
	 * x = substr("HelloWorld", 2, 5)
	 * x = substr(a, 0, y)
	 *
	 * ==========================
	 * Note 1:
	 *
	 * If dest element is not string - Error
	 * If err element is not int - Error
	 * If src1 element is not string - Error
	 * If src2 element is not int - Error
	 * If src2 element is not int - Error
	 * If src2 int value is out of index of dest string - Error
	 *
	 * ==========================
	 * Note 2:
	 *
	 * If index out bounds, err is set to 1 and returned
	 *
	 * Note 3:
	 *
	 * If n > strlen(src1) - src2, only chars to end are returned
	 *
	 *
	 */
	IC_SUBSTR_STR,

	// ===================== CONDITION INSTRUCTIONS =====================

	/*
	 * Helper instruction. Indicates incoming condition instructions for if
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_IF_DEF,

	/*
	 * Helper instruction. Indicates start of if perform instructions
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_IF_START,

	/*
	 * Helper instruction. Indicates end of if perform instructions
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_IF_END,

	/*
	 * Helper instruction. Indicates start of else perform instructions
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_ELSE_START,

	/*
	 * Helper instruction. Indicates end of else perform instructions
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_ELSE_END,

	// ===================== CYCLE INSTRUCTIONS =====================

	/*
	 * Helper instruction. Indicates incoming instructions for for cycle init
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_FOR_DEF,

	/*
	 * Helper instruction. Indicates incoming instructions for for cycle condition
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_FOR_COND,

	/*
	 * Helper instruction. Indicates incoming instructions for for cycle post iteration expression
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_FOR_STEP,

	/*
	 * Helper instruction. Indicates start of incoming instructions for for cycle body
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_FOR_BODY_START,

	/*
	 * Helper instruction. Indicates end of incoming instructions for for cycle body
	 *
	 * elem_dest: NULL
	 * elem_1: NULL
	 * elem_2: NULL
	 *
	 */
	IC_FOR_BODY_END,

} instr_type_t;

struct instruction {
	instr_type_t type;
	elem_t *elem_dest_ptr;
	elem_t *elem1_ptr;
	elem_t *elem2_ptr;
	instr_t *next;
};

// Generate program start and prepare required jmp label stacks
void codegen_init();

// Get scope of variable
char *get_frame(sym_var_item_t *sym_var_item);

// Used after condition instruction, check next instruction and prepare label
void try_create_jump(instr_t instr);

bool is_condition_instr(instr_t instr);

// Generators for specific instruction
void declr_var(instr_t instr);
void def_var(instr_t instr);
void call_fun(instr_t instr);
void ret_fun(instr_t instr);
void add_var(instr_t instr);
void sub_var(instr_t instr);
void mul_var(instr_t instr);
void div_var(instr_t instr);
void lt_var(instr_t instr);
void gt_var(instr_t instr);
void eq_var(instr_t instr);
void and_var(instr_t instr);
void or_var(instr_t instr);
void not_var(instr_t instr);
void int2float(instr_t instr);
void float2int(instr_t instr);
void str2int(instr_t instr);
void read_int(instr_t instr);
void read_float(instr_t instr);
void read_string(instr_t instr);
void read_bool(instr_t instr);
void write_var(instr_t instr);
void concat_str(instr_t instr);
void strlen_str(instr_t instr);
void getchar_str(instr_t instr);
void substr_str(instr_t instr);
void substr_str_def();
void getchar_str_def();
void str2int_def();
void read_int_def();
void read_float_def();
void read_string_def();
void read_bool_def();
void if_def();
void if_start(instr_t instr);
void if_end(instr_t instr);
void else_start();
void else_end();
void for_def_codegen();
void for_cond();
void for_step(instr_t instr);
void for_body_start();
void for_body_end();

// Cycle whole ll of instructions, get instruction, resolve and call appropriate function
void codegen_generate_instr();

// Check if build-in functions (those that requires some handle from IFJ20code)
// have been called at least once and if so call generates for each of them
void codegen_post_generate();

// Start codegen generation. Expects filled ll with instructions
void codegen();

#endif //CODEGEN_H
