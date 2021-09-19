/*
Kfir Sibirsky	316317221
=====================================================================================================
This file contains the definition for the types parms and cmd_names and 
As well as prototypes for the input analysis.

Notes:
• There are 6 set variable (SETA-SETF), all of which are set to {} = ∅ at the start of the program.
• For ease of use, I created an array of sets and their names as strings.
• I was inspired by program 22 (matrix calculator) in pages (220-225) in the learning guide.
=====================================================================================================
*/
#include "set.h"
#include <ctype.h>
#include <string.h>

#define MAX_LINE_LEN 80
#define NUMBER_OF_FUNCTIONS 8 /* 7 + help = 8*/

/*--------------------------------------------------------------------------------------------
parms: struct that holds the corresponding parameters to the operands.
--------------------------------------------------------------------------------------------*/
typedef struct tnode {
     set *first_parm;
     set *second_parm;     
     set *result;     
}parms;
parms temp_parm;

/*--------------------------------------------------------------------------------------------
cmd_names: command names are enumerated for ease of usage.
--------------------------------------------------------------------------------------------*/
enum cmd_names
{
    READ_SET, PRINT_SET, UNION_SET, INTERSECT_SET, SUB_SET, SYMDIFF_SET, STOP, HELP
};

/*--------------------------------------------------------------------------------------------
reset_sets: Sets each of the set variables as the empty set = {} = ∅.
--------------------------------------------------------------------------------------------*/
void reset_sets(void);

/*--------------------------------------------------------------------------------------------
get_input: Gets the input from the user. (either via keyboard or redirected from a file)
	  	   If an error was detected then, an error message is printed.
--------------------------------------------------------------------------------------------*/
void get_input(char input[]);

/*--------------------------------------------------------------------------------------------
analyze_input: Analyze the input got from the user.
--------------------------------------------------------------------------------------------*/
void analyze_input(char input[]);

/*--------------------------------------------------------------------------------------------
stop: kills program
e.g.,	stop
	input:	no input	
	output:	you have chosen to quit (or it may have been chosen for you :D) Have a nice day :)
--------------------------------------------------------------------------------------------*/
void stop(void);

/*--------------------------------------------------------------------------------------------
get_cmd: check if a given string (str) is a valid command from one of the commands in cmds[].
		 if found, returns the index in which the command at, else return an error code.
--------------------------------------------------------------------------------------------*/
int get_cmd(char *line, char * cmds[]);

/*--------------------------------------------------------------------------------------------
skip_white_spaces_start: returns the number of white spaces from the start of a given string.
--------------------------------------------------------------------------------------------*/
int skip_white_spaces_start(char *s);

/*--------------------------------------------------------------------------------------------
check_set: check if a given string (operand_to_check) is a valid set name from one of the
        names in sets[]. if found, returns the corresponding set, else return NULL.
--------------------------------------------------------------------------------------------*/
set *check_set(char * str);

/*--------------------------------------------------------------------------------------------
remove_white_spaces: removes all white spaces from a given string (str).
--------------------------------------------------------------------------------------------*/
void remove_white_spaces(char *str);

/*--------------------------------------------------------------------------------------------
help: prints a quick start guide.
--------------------------------------------------------------------------------------------*/
void help(void);

/*--------------------------------------------------------------------------------------------
check_missing_comma: checks whether a comma is missing.
					   returns an error code if it is and 1 otherwise.
--------------------------------------------------------------------------------------------*/
int check_missing_comma(char c);

/*--------------------------------------------------------------------------------------------
check_extraneous_text: checks whether a character is extraneous.
					   returns an error code if it is and 1 otherwise.
--------------------------------------------------------------------------------------------*/
int check_extraneous_text(char c);

/*--------------------------------------------------------------------------------------------
check_multiple_consecutive_commas: checks whether a commas are consecutive.
					   returns an error code if it is and 1 otherwise.
--------------------------------------------------------------------------------------------*/
int check_multiple_consecutive_commas(char c);

