/*
Kfir Sibirsky	316317221
Eyal Haimov     316316118
=====================================================================================================
This file contains the inclusion of the required libraries, definitions, and function prototypes
and data structures used by the main program (assembler) and both passes.
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
• #ifndef UTILS_H - temporary condition checks if the macro UTILS_H exist.
• #define UTILS_H - defining the macro UTILS_H.
• #endif - each ifndef must be closed by using #endif.   
• #endif - each ifndef must be closed by using #endif.                                          
• #define MAX_LINE_LEN - the max size of each line.                                              
• #define MAX_LABEL_LEN - the max size of each label.                                           
• #define NUM_OF_INSTRUCTIONS - the number of instructions.                                    
• #define NUM_OF_DIRECTIVES - the number of directives.                                        
• #define IC_START_ADDR - The starting address of instruction counter.                        
• #define DC_START_ADDR - The starting address of data counter.                      
• #define ERROR - flag to detect if there is an error.            
=====================================================================================================
*/

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#define MAX_LINE_LEN 82
#define MAX_LABEL_LEN 31
#define NUM_OF_INSTRUCTIONS 27
#define NUM_OF_DIRECTIVES 6
#define IC_START_ADDR 100
#define DC_START_ADDR 0
#define NUM_OF_INSTRUCTIONS 27
#define ERROR -1

/*--------------------------------------------------------------------------------------------
machine_instruction: Represent a machine instruction from page 19 in the course booklet.
                     (used to create the commands table from that page)
--------------------------------------------------------------------------------------------*/
typedef struct machine_instruction {
	char * name;
	char type;
	int funct;
	int opcode;
}machine_instruction;

/*--------------------------------------------------------------------------------------------
sym_type: Represent the type of a symbol.
--------------------------------------------------------------------------------------------*/
typedef enum symbol_type {EXTERNAL, DATA, CODE, CODE_AND_ENTRY,DATA_AND_ENTRY} sym_type;

/*--------------------------------------------------------------------------------------------
symbol: Represent a single symbol. (= node in a linked list of symbols)
--------------------------------------------------------------------------------------------*/
typedef struct symbol { 
	char name[MAX_LABEL_LEN]; 
	int address; 
	sym_type type;
	struct symbol *next; 
}symbol;

/*--------------------------------------------------------------------------------------------
symbol_list: Represent a linked list of symbols.
--------------------------------------------------------------------------------------------*/
typedef struct symbol_list {
    symbol *head;
    int count;
} symbol_list;
extern symbol_list *symbol_table;

/*--------------------------------------------------------------------------------------------
bit: Represent a single bit.
--------------------------------------------------------------------------------------------*/
typedef struct bit {
    unsigned int bit:1;
} bit;

/*--------------------------------------------------------------------------------------------
R_instruction: Represent a single instruction of type R.
               The R instruction set includes the following instructions:
               • Arithmetic and logical instructions of type R: add, sub, and, or, nor.
               • Copy instructions: move, mvhi, mvlo.
--------------------------------------------------------------------------------------------*/
typedef struct R_instruction { 
	unsigned int not_used:6;
	unsigned int funct:5;
	unsigned int rd:5;
	unsigned int rt:5;
	unsigned int rs:5;
	unsigned int opcode:6;
}R_instruction;
/*--------------------------------------------------------------------------------------------
I_instruction: Represent a single instruction of type I.
               The I instruction set includes the following instructions:
               • Arithmetic and logical instructions of type I: addi, subi, andi, ori, nori.
               • Conditional branching instructions: beq, bne, blt, bgt.
               • Memory loading and storage instructions: lb, sb, lh, sh, lw, sw.
--------------------------------------------------------------------------------------------*/
typedef struct I_instruction { 
	unsigned int immed:16;
	unsigned int rt:5;
	unsigned int rs:5;
	unsigned int opcode:6;
}I_instruction;

/*--------------------------------------------------------------------------------------------
J_instruction: Represent a single instruction of type J.
               The J instruction set includes the following instructions:
               • Jump to another place in the program to continue running: jmp.
               • Load a specific label address to $0: la.
               • Jump to another place in the program to continue running,
                 the following instruction address is stored in $0: call.
               • Stop the program: stop.
--------------------------------------------------------------------------------------------*/
typedef struct J_instruction { 
	unsigned int address:25;
	unsigned int reg:1;
	unsigned int opcode:6;
}J_instruction;
/*--------------------------------------------------------------------------------------------
instruction_node: Represent a single general instruction.
                  (= node in a linked list of instructions)
--------------------------------------------------------------------------------------------*/
typedef struct instruction_node {
	int address;
	R_instruction *R;
	I_instruction *I;
	J_instruction *J;
	bit partial;
	struct instruction_node *next; 
}instruction_node;
/*--------------------------------------------------------------------------------------------
instruction_list: Represent a linked list of instrcutions.
--------------------------------------------------------------------------------------------*/
typedef struct instruction_list {
    instruction_node *head;
    int count;
} instruction_list;
extern instruction_list *code_image;

/*--------------------------------------------------------------------------------------------
_8_bit_directive: Represent a single 8 bit directive.
                  The 8 bit directive set includes the following directives:
                  • store in the data image byte size numbers: .db
                  • store in the data image byte size characters: .asciz
--------------------------------------------------------------------------------------------*/
typedef struct _8_bit_directive { 
	unsigned int data:8;
}_8_bit_directive;
/*--------------------------------------------------------------------------------------------
_16_bit_directive: Represent a single 16 bit directive.
                   The 16 bit directive set includes the following directive:
                   • store in the data image 2 bytes size numbers: .dh
--------------------------------------------------------------------------------------------*/
typedef struct _16_bit_directive { 
	unsigned int data:16;
}_16_bit_directive;
/*--------------------------------------------------------------------------------------------
_32_bit_directive: Represent a single 32 bit directive.
                   The 32 bit directive set includes the following directive:
                   • store in the data image 4 bytes size numbers: .dw
--------------------------------------------------------------------------------------------*/
typedef struct _32_bit_directive { 
	unsigned int data:32;
}_32_bit_directive;
/*--------------------------------------------------------------------------------------------
directive_node: Represent a single general directive. (= node in a linked list of directives)
--------------------------------------------------------------------------------------------*/
typedef struct directive_node { 
	int address;
	_8_bit_directive  *_8_bit;
	_16_bit_directive *_16_bit;
	_32_bit_directive *_32_bit;
	struct directive_node *next; 
}directive_node;
/*--------------------------------------------------------------------------------------------
directive_list: Represent a linked list of directives.
--------------------------------------------------------------------------------------------*/
typedef struct directive_list {
    directive_node *head;
    int count;
} directive_list;

extern directive_list *data_image;

/*--------------------------------------------------------------------------------------------
ext: Represent a single external symbol . (= node in a linked list of external symbols)
--------------------------------------------------------------------------------------------*/
typedef struct ext { 
	char name[MAX_LABEL_LEN];
	int address;
	struct ext *next; 
}ext;
/*--------------------------------------------------------------------------------------------
ext_list: Represent a linked list of external symbols.
--------------------------------------------------------------------------------------------*/
typedef struct ext_list {
    ext *head;
    int count;
} ext_list;
extern ext_list *external_list;

extern int IC, DC, ICF, DCF;
extern char *file_name;
/*--------------------------------------------------------------------------------------------
skip_white_spaces: Returns the number of white space characters from a given index (start_at)
                   to the first non-white space character for a given string (str).
--------------------------------------------------------------------------------------------*/
int skip_white_spaces(char *str,int start_at);

/*--------------------------------------------------------------------------------------------
skip_non_white_spaces: Returns the number of non-white space characters from a given index 
                       (start_at) to the first white space character for a given string (str).
--------------------------------------------------------------------------------------------*/
int skip_non_white_spaces(char *str,int start_at);

/*--------------------------------------------------------------------------------------------
is_label: checks whether a given label (tkn) is valid.
          returns 1 if so, -1 if not and -2 if there was no label at all.
--------------------------------------------------------------------------------------------*/
int is_label(char * tkn ,machine_instruction instructions[] ,char * directives[]
            ,symbol_list * symbols, int is_parameter_check, int line_number);

/*--------------------------------------------------------------------------------------------
int_to_bin_str: Converts a decimal number (n) to a (len) bits binary number.
                Stores the result as a string (in str).
--------------------------------------------------------------------------------------------*/
void int_to_bin_str(int n, int len, char *str);

/*--------------------------------------------------------------------------------------------
bin_str_to_hex_str: Converts a binary string (bin) to hexadecimal string (hex) in which the
                    order of the hexa digits is as desribed in page 40 in the course booklet. 
--------------------------------------------------------------------------------------------*/
void bin_str_to_hex_str(char *bin, char *hex);

/*--------------------------------------------------------------------------------------------
eng_ordinal_nums: Returns the proper english ordinal numeration suffix for a given number (n).
--------------------------------------------------------------------------------------------*/
char * eng_ordinal_nums(int n);

#endif
