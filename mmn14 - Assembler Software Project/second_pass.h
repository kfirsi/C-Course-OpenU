/*
Kfir Sibirsky	316317221
Eyal Haimov     316316118
=====================================================================================================
This file contains the inclusion of the required libraries, definitions, and function prototypes
for the second pass.
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
• #ifndef SECOND_PASS_H - temporary condition checks if the macro SECOND_PASS_H exist.
• #define SECOND_PASS_H - defining the macro SECOND_PASS_H.
• #endif - each ifndef must be closed by using #endif.   
=====================================================================================================
*/
#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>

/*--------------------------------------------------------------------------------------------
add_to_ext_list: Create a new ext node (external symbol), which consists of the corresponding given 
                 operands (name = name, address = addr).
                 Afterwards, append the new node to the end of the linked list of
                 external symbols (e = external list)
--------------------------------------------------------------------------------------------*/
void add_to_ext_list (ext_list * e, char * name, int addr);

/*--------------------------------------------------------------------------------------------
remove_white_spaces: removes all white spaces from a given string (str).
--------------------------------------------------------------------------------------------*/
void remove_white_spaces(char *str);

#endif
