/*
Kfir Sibirsky	316317221
Eyal Haimov     316316118
=====================================================================================================
This file contains the inclusion of the required libraries, definitions, and function prototypes
for the first pass.
-----------------------------------------------------------------------------------------------------
Included libraries:
-------------------
• <stdio.h> - for input and output commands, such as: scanf, printf.
• <stdlib.h> - for several functions, such as: atof, atoi, free, atol etc.
• <string.h> - for a string commands and NULL.
• <ctype.h> - for a char commands, such as: isdigit, isspace etc.
• <limits.h> - for defining the number of bits in the parameters.
-----------------------------------------------------------------------------------------------------
Definitions:
-------------------
• #ifndef FISRT_PASS_H - temporary condition checks if the macro FISRT_PASS_H exist.
• #define FISRT_PASS_H - defining the macro FISRT_PASS_H.
• #endif - each ifndef must be closed by using #endif.   
=====================================================================================================
*/
#ifndef FISRT_PASS_H
#define FISRT_PASS_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
/*--------------------------------------------------------------------------------------------
add_to_code_image: Create a new instruction node of given type (i_type = R / I / J) according
                   to a given index (i_idx) to the instruction table.
                   The new node consists of the corresponding given operands 
                   (oprnd1 / oprnd2 / oprnd 3 / label) to the expected operands of the 
                   instruction at i_idx.
                   Afterwards, append the new node to the end of the linked list of
                   instructions (ci = code image)
--------------------------------------------------------------------------------------------*/
void add_to_code_image (instruction_list *ci, int i_type,int i_idx, int oprnd1, int oprnd2,
                        int oprnd3,symbol_list * symbols, char * label);

/*--------------------------------------------------------------------------------------------
add_to_data_image: Create a new directive node of (8 bit / 16 bit / 32 bit) according
                   to a given index (d_idx) to the directive table.
                   The new node consists of the corresponding given number (to_store) 
                   to the expected operand of the directive at d_idx.
                   Afterwards, append the new node to the end of the linked list of
                   directives (di = data image)
--------------------------------------------------------------------------------------------*/
void add_to_data_image (directive_list *di, int d_idx, int to_store);

/*--------------------------------------------------------------------------------------------
add_to_symbol_table: Create a new symbol node, which consists of the corresponding given 
                     operands (name = name, address = addr, type = ty).
                     Afterwards, append the new node to the end of the linked list of
                     symbols (st = symbol table)
--------------------------------------------------------------------------------------------*/
void add_to_symbol_table (symbol_list *st, char * name, int addr, sym_type ty);

/*--------------------------------------------------------------------------------------------
increase_address_to_data: Adds the value of ICF to the address of each symbol in the symbol
                          table which characterized as "data". also, add the value of ICF to
                          the address of each node in the linked list of directives 
                          (data image)
--------------------------------------------------------------------------------------------*/
void increase_address_to_data(symbol_list *st, directive_list *di);

/*--------------------------------------------------------------------------------------------
get_comma: Check whether there is a comma at the start of a given string (cpy_ptr),
           and whether there are multiple consecutive commas or missing commas.
           (error) gets the error status. 
           Skips to the first non-white space character after the comma, and returns that
           index.  
--------------------------------------------------------------------------------------------*/
int get_comma(char * cpy_ptr , int *error, int line_number );

/*--------------------------------------------------------------------------------------------
get_immed: Check whether there is an immediate number at the start of a given string (cpy_ptr).
           If so, (num) gets the immediate number, else (error) gets the error status. 
           returns the length of the string containing the '+' / '-' sign and the number 
           (e.g., "-533" returns 3).
--------------------------------------------------------------------------------------------*/
int get_immed(char * cpy_ptr, long *num , int *error, int line_number );

/*--------------------------------------------------------------------------------------------
get_reg: Check whether there is a register at the start of a given string (cpy_ptr).
         If so, (reg_num) gets the register number, else (error) gets the error status.
         returns the length of the string containing the '$' sign and the register number 
         (e.g., "$25" returns 3).
--------------------------------------------------------------------------------------------*/
int get_reg(char * cpy_ptr, long *reg_num , int *error, int line_number );

/*--------------------------------------------------------------------------------------------
get_label: Check whether there is a label at the start of a given string (cpy_ptr).
           If so, (label_operand) gets the label, else (error) gets the error status. 
           returns the length of the label. (e.g., "example_label" returns 13).
--------------------------------------------------------------------------------------------*/
int get_label(char * cpy_ptr, char *label_operand, int *error, int line_number );

/*--------------------------------------------------------------------------------------------
get_label_or_reg: Check whether there is a label or a register at the start of a given string
                  (cpy_ptr). Accordingly (label_operand) gets the label or (reg_num) gets the 
                  register number. Else, (error) gets the error status. 
                  Returns the length of the string containing the label 
                  (e.g., "example_label" returns 13). or string containing the '$' sign and 
                  the register number (e.g., "$25" returns 3).
--------------------------------------------------------------------------------------------*/
int get_label_or_reg(char * cpy_ptr, long *reg_num, char *label_operand, int *error, int line_number );
#endif
