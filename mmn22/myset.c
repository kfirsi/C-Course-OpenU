/*
Kfir Sibirsky	316317221
=====================================================================================================
This file contains the main program, as well as all user interaction and input analysis activities
(including error message printing). Also, within this file, the six set variables are declared.

Notes:
• There are 6 set variable (SETA-SETF), all of which are set to {} = ∅ at the start of the program.
• For ease of use, I created an array of sets and their names as strings.
• I was inspired by program 22 (matrix calculator) in pages (220-225) in the learning guide.
• In my gedit settings, Tab Width: 4. If the comments looks weird in yours then please change to.
=====================================================================================================
*/
#include "input.h"

set a,b,c,d,e,f;
set_array sets[6]={{"SETA",&a},{"SETB",&b},{"SETC",&c},{"SETD",&d},{"SETE",&e},{"SETF",&f}};



/*--------------------------------------------------------------------------------------
	main: This function starts the program.
--------------------------------------------------------------------------------------*/
int main()
{
    char input[MAX_LINE_LEN];
    
	reset_sets();
	
	/* Quick description. */
	printf ("┌──────────────────────────────────────────────────────────────────────────────┐\n");
	printf ("│    Hello, ☻                                                                  │\n");
	printf ("│    Welcome to essentially a calculator for sets.                             │\n");
	printf ("│    To use the program - commands are needed to be entered one at a time.     │\n");
	printf ("│    *** Tip:For a quick start guide - Please enter help. ;)                   │\n");
	printf ("└──────────────────────────────────────────────────────────────────────────────┘\n\n");
	
	/* Although this is an infinite loop, it will stop when the STOP command is scanned. */
    while (1)
    {
    	/* Gets the input from the user. (either via keyboard or redirected from a file) */
        get_input(input); 
    	/* Analyze the input from the user. */
        analyze_input(input);
    }
	return 0;
}
/*--------------------------------------------------------------------------------------------
reset_sets: Sets each of the set variables as the empty set = {} = ∅.
--------------------------------------------------------------------------------------------*/
void reset_sets()
{
	int i;
	for(i=0;i<6;i++)
	{
		reset_set(sets[i].s);
	}
}
/*--------------------------------------------------------------------------------------------
get_input: Gets the input from the user. (either via keyboard or redirected from a file)
	  	   If an error was detected then, an error message is printed.
--------------------------------------------------------------------------------------------*/
void get_input(char input[])
{   
	/* Prints a line for ease of reading on screen. */
	printf ("────────────────────────────────────────────────────────────────────────────────\n");
	printf ("Please enter a command: ");
    /* Check if end of the file was reached before stop command. */
    if (!fgets(input, MAX_LINE_LEN, stdin)) 
    {
        printf("\nEOF was reached before stop command.\n");
        exit(0);
    }
	printf ("\nThe line you entered is: %s",input);
    
}
/*--------------------------------------------------------------------------------------------
analyze_input: Analyze the input got from the user.
--------------------------------------------------------------------------------------------*/
void analyze_input(char input[])
{
    int current_command, idx=0, count = 0, add_to_set[128], temp_num;
    char *cpy_str;	/* Copy of the original input to work on. */
    char operand[5];/* Hold the name of the set. */
	char *token;	/* Required for "read_set". */
	char res_to_string[MAX_LINE_LEN];
	char *command_names[NUMBER_OF_FUNCTIONS] = {"read_set",	"print_set", "union_set", "intersect_set",
												"sub_set", "symdiff_set", "stop", "help"};
	temp_parm.first_parm = NULL;
	temp_parm.second_parm = NULL;
	temp_parm.result = NULL;

	/* At each stage, what that is expected to be the next section of the next section 
	 * will be checked against the actual next section.
	 * In case an error has been detected, a suitable prompt will occur,
	 * The erroneous command won't run, and prompt for a new one. */
	 
    cpy_str = input;
	  
    /* Skip unnecessary white spaces at the start of the input. */
    cpy_str += skip_white_spaces_start(cpy_str);
    /* Check if the input is a blank line. */
    if (cpy_str[0] == '\n') 
    {
        printf("Blank line was entered.\n");
        return;
    }
    /* Check which command required to be executed. */
    current_command = get_cmd(cpy_str,command_names); 
    /* Check validity for the name of the command required to be executed. */
    if (current_command == ERROR)
    {
        printf("Undefined command name.\n");
        return;
    }
    /* At this stage, a good command name has been confirmed.
	 * Hence, skip the command name. */
    cpy_str += strlen(command_names[current_command]);
    /* remove every white space (shrink string making it easier for various checks). */
    remove_white_spaces(cpy_str);
    /* Check for illegal comma after command name. */
    if(cpy_str[0] == ',')
    {
        printf("Illegal comma.\n");
        return;
    }
	if(current_command == HELP)
	{
		/* Calling the function only if passed check. */
		if(check_extraneous_text(cpy_str[0]))
     	   help();
   		return;
	}
    if (current_command == STOP)
    {
		/* Calling the function only if passed check. */
    	if(check_extraneous_text(cpy_str[0]))
     	   stop();
   		return;
    }
    
    /* At this stage, whichever the command may be, the first operand expected be one of SETA / ... / SETF.
     * Hence, in order to validate which is it, isolation of the next 4 characters is required.  */
       
    strncpy(operand, cpy_str, 4);
    temp_parm.first_parm = check_set(operand);
    
    if (!temp_parm.first_parm) 
    {
        printf("Undefined set name\n");
        return;
    }
    /* At this stage, a valid first set extracted from the corresponding operand has been confirmed.
     * Hence, skip the set name. */
    cpy_str += 4;
    
    if (current_command == PRINT_SET)
    {		
    	/* Calling the function only if passed check. */
    	if(check_extraneous_text(cpy_str[0]))
			print_set(temp_parm.first_parm);
        return;
    }

    /* At this stage, whichever the command may be, a comma is expected. */
    if(check_missing_comma(cpy_str[0])==ERROR)
    	return;
    	
    /* At this stage, a valid comma that is after the first operand has been confirmed.
	 * Hence, skip it. */
	   
    cpy_str+=1;
	
    /* At this stage, all commands expect additional operands.
     * READ_SET expect list of numbers, and the rest of the command expect 2 more sets.*/
       
    if (current_command == UNION_SET || current_command == INTERSECT_SET || current_command == SUB_SET || current_command == SYMDIFF_SET) 
    {
        /* Once more, the next operand expected be one of SETA / ... / SETF.
      	 * Hence, in order to validate which is it, isolation of the next 4 characters is required.  */
		strncpy(operand, cpy_str, 4);
		temp_parm.second_parm = check_set(operand);
		
	    /* Check for multiple consecutive commas between first and second operands. */
		if(check_multiple_consecutive_commas(cpy_str[0])==ERROR)
			return;
			
		if (!temp_parm.second_parm)
		{
		    printf("Undefined set name\n");
		    return;
		}
	    /* At this stage, a valid second set extracted from the corresponding operand has been confirmed.
       	 * Hence, skip the set name. */
		cpy_str += 4;
		
		/* At this stage, whichever the command may be, a comma is expected. */
		if(check_missing_comma(cpy_str[0])==ERROR)
			return;
			
	    /* At this stage, a valid comma that is after the first operand has been confirmed.
	     * Hence, skip it. */
		cpy_str+=1;
		
        /* Once more, the next operand expected be one of SETA / ... / SETF.
      	 * Hence, in order to validate which is it, isolation of the next 4 characters is required.  */
		strncpy(operand, cpy_str, 4);
		temp_parm.result = check_set(operand);
		
	    /* Check for multiple consecutive commas between second and third operands. */
		if(check_multiple_consecutive_commas(cpy_str[0])==ERROR)
				return;
		if (!temp_parm.result) 
		{
		    printf("Undefined set name\n");
		    return;
		}
	    /* At this stage, a valid third set extracted from the corresponding operand has been confirmed.
       	 * Hence, skip the set name. */
		cpy_str += 4;
        /* Check for extraneous text after third operand. */
    	if(check_extraneous_text(cpy_str[0])==ERROR)
			return;
        
	    /* At this stage, a valid complete input has been confirmed.
       	 * Hence, calling the corresponding command. */
        if (current_command == UNION_SET)
            union_set(temp_parm.first_parm, temp_parm.second_parm, temp_parm.result);
        else if (current_command == INTERSECT_SET)
            intersect_set(temp_parm.first_parm, temp_parm.second_parm, temp_parm.result);
		else if (current_command == SUB_SET)
            sub_set(temp_parm.first_parm, temp_parm.second_parm, temp_parm.result);
        else
            symdiff_set(temp_parm.first_parm, temp_parm.second_parm, temp_parm.result);
        return;
    }
	/* At this stage, READ_SET has been confirmed.
	 * As mentioned, READ_SET expect a list of numbers.
	 * Hence, breaking the rest of the input into a series of tokens is required. */   
    
	token = strtok(cpy_str, ",");
   
	while( token != NULL ) 
	{        
		temp_num = atoi(token);
		/* Check validity of the result from "atoi" function. */
		sprintf(res_to_string, "%d", temp_num);
        if(strcmp(token,res_to_string))
        {
            printf("Invalid set member - not an integer.\n");
            return;
        }
        /* Check range validity of number from list. */
        if(temp_num != -1 && (temp_num<LOWER_BOUND || temp_num>UPPER_BOUND))
        {
            printf("Invalid set member - out of range.\n");
            return;
        }
        /* Only if passed the checks, add the number from the list to the
         * correct place in the array that will be added to the set. */
        add_to_set[idx]=temp_num;
        idx++;
        count++;
		token = strtok(NULL,",");
	}
    /* Check ending validity for the list of numbers. */
	if(add_to_set[idx-1] != -1)
    {
        printf("List of set members is not terminated correctly.\n");
		return;
    } 
    /* At this stage, a valid complete input has been confirmed.
   	 * Hence, calling the read_set function. */
    read_set(temp_parm.first_parm, add_to_set, count);
}
/*--------------------------------------------------------------------------------------------
skip_white_spaces_start: returns the number of white spaces from the start of a given string.
--------------------------------------------------------------------------------------------*/
int skip_white_spaces_start(char *str)
{
	int i=0;
	while(isspace(str[i]) && i<strlen(str))
	{
		i++;
    }
	return i;
}
/*--------------------------------------------------------------------------------------------
get_cmd: check if a given string (str) is a valid command from one of the commands in cmds[].
		 if found, returns the index in which the command at, else return an error code.
--------------------------------------------------------------------------------------------*/
int get_cmd(char * str,char * cmds[])
{
    int i=0, cmd_len = 0;
    
    for (;!isspace(str[i]) && str[i] != ','; i++);/* Count the length of the command name. */
    cmd_len = i;
    for (i = 0; i < NUMBER_OF_FUNCTIONS; i++)
    {
		if (!strncmp(cmds[i], str, cmd_len))
		{
            return i;
    	}
    }
    return ERROR;    
}
/*--------------------------------------------------------------------------------------------
remove_white_spaces: removes all white spaces from a given string (str).
--------------------------------------------------------------------------------------------*/
void remove_white_spaces(char *str)
{
	int i=0,j=0;
	while(i < strlen(str)) 
	{   
		if (!isspace(str[i]))
		{
		    /* Any non white space character gets moved back to the corresponding index. */
			str[j] = str[i];
			j++;
		}
		i++;
	}
	str[j] = '\0';
}
/*-----------------------------------------------------------------------------------------------------------------
check_extraneous_text: checks whether a character is extraneous.
					   returns an error code if it is and 1 otherwise.
-----------------------------------------------------------------------------------------------------------------*/
int check_extraneous_text(char c)
{
    if (c!='\0')
    {
        printf("Extraneous text after end of command\n");
        return ERROR;
    }
    return 1;
}
/*-----------------------------------------------------------------------------------------------------------------
check_missing_comma: checks whether a comma is missing.
					   returns an error code if it is and 1 otherwise.
-----------------------------------------------------------------------------------------------------------------*/
int check_missing_comma(char c) 
{
    if (c != ',')
	{
        printf("Missing comma\n");
        return ERROR;
    }
    return 1;
}
/*-----------------------------------------------------------------------------------------------------------------
check_multiple_consecutive_commas: checks whether a commas are consecutive.
					                   returns an error code if it is and 1 otherwise.
-----------------------------------------------------------------------------------------------------------------*/
int check_multiple_consecutive_commas(char c) 
{
    if (c == ',') 
    {
        printf("Multiple consecutive commas\n");
        return ERROR;
    }
    return 1;
}
/*-----------------------------------------------------------------------------------------------------------------
check_set: check if a given string (operand_to_check) is a valid set name from one of the names in sets[].
		 if found, returns the corresponding set, else return NULL.
-----------------------------------------------------------------------------------------------------------------*/
set *check_set(char * operand_to_check)
{
	int i;
	for(i = 0;i < 6;i++)
	{
		if(!strcmp(sets[i].name,operand_to_check))
		{
			return sets[i].s;
		}
	}
	return NULL;
}
/*------------------------------------------------------------------------------------------------------------------
stop: kills program
e.g.,	stop
	input:	no input	
	output:	you have chosen to quit (or it may have been chosen for you :D) Have a nice day :)
-------------------------------------------------------------------------------------------------------------------*/
void stop()
{
	printf ("\n────────────────────────────────────────────────────────────────────────────────\n");
	printf (" \tYou have chosen to quit ( or it may have been chosen for you :D )\n");
	printf (" \tHave a nice day :)\n");
	printf ("────────────────────────────────────────────────────────────────────────────────\n\n");
	exit(0);
}
/*--------------------------------------------------------------------------------------------
help: prints a quick start guide.
--------------------------------------------------------------------------------------------*/
void help()
{
	printf("\n╒══════════════════════════════════════════════════════════════════════════════╕");
	printf("\n│The available commands are:                                                   │");
	printf("\n╞══════════════════════════════════════════════════════════════════════════════╡");
	printf("\n│read_set set-name, list-of-values-separated-by-commas                         │");
	printf("\n│                                                                              │");
	printf("\n│Gets the members of a set into a given set.                                   │");
	printf("\n│The end of the list of values is marked by the negative number -1.            │");
	printf("\n│e.g., read_set SETA, 1, 2, 3, -1                                              │");
	printf("\n├──────────────────────────────────────────────────────────────────────────────┤");
	printf("\n│print_set set-name                                                            │");
	printf("\n│                                                                              │");
	printf("\n│Prints the members of the followed set.                                       │");
	printf("\n│e.g., print_set SETB                                                          │");
	printf("\n├──────────────────────────────────────────────────────────────────────────────┤");
	printf("\n│union_set set-name-a, set-name-b, set-name-c                                  │");
	printf("\n│                                                                              │");
	printf("\n│Performs union of two sets A and B, and stores the result in set C.           │");
	printf("\n│e.g., union_set SETA, SETB, SETC                                              │");
	printf("\n│Mathematical notation:       A ∪ B = { x | x ∈ A or x ∈ B }                   │");
	printf("\n├──────────────────────────────────────────────────────────────────────────────┤");
	printf("\n│intersect_ set-name-a, set-name-b, set-name-c                                 │");
	printf("\n│                                                                              │");
	printf("\n│Performs intersection of two sets A and B, and stores the result in set C.    │");
	printf("\n│e.g., intersect_set SETA, SETB, SETC                                          │");
	printf("\n│Mathematical notation:       A ∩ B = { x | x ∈ A and x ∈ B }                  │");
	printf("\n├──────────────────────────────────────────────────────────────────────────────┤");
	printf("\n│sub_set set-name-a, set-name-b, set-name-c                                    │");
	printf("\n│                                                                              │");
	printf("\n│Performs subtraction of set B from set A, and stores the result in set C.     │");
	printf("\n│e.g., sub_set SETA, SETB, SETC                                                │");
	printf("\n│Mathematical notation:       A − B = { x | x ∈ A, x ∉ B }                     │");
	printf("\n├──────────────────────────────────────────────────────────────────────────────┤");
	printf("\n│symdiff_set set-name-a, set-name-b, set-name-c                                │");
	printf("\n│                                                                              │");
	printf("\n│Calculates symmetrical difference of set A and set B, and stores the          │");
	printf("\n│result in set C.                                                              │");
	printf("\n│e.g., symdiff_set SETA, SETB, SETC                                            │");
	printf("\n│Mathematical notation:                                                        │");
	printf("\n│A ∆ B =  (A − B) ∪ (B − A) = { x | x ∈ A, x ∈ B, x ∉ (A ∩ B) }                │");
	printf("\n├──────────────────────────────────────────────────────────────────────────────┤");
	printf("\n│stop                                                                          │");
	printf("\n│                                                                              │");
	printf("\n│Kills the program.                                                            │");
	printf("\n│e.g., stop                                                                    │");
	printf("\n╞══════════════════════════════════════════════════════════════════════════════╡");
	printf("\n│Possible errors messages:                                                     │");
	printf("\n╞══════════════════════════════════════════════════════════════════════════════╡");
	printf("\n│• Extraneous text after end of command.                                       │");
	printf("\n│• Missing parameter/s.                                                        │");
	printf("\n│• EOF was reached before stop command.                                        │");
	printf("\n│• Blank line was entered.                                                     │");
	printf("\n│• Undefined command name.                                                     │");
	printf("\n│• Illegal comma.                                                              │");
	printf("\n│• Multiple consecutive commas.                                                │");
	printf("\n│• List of set members is not terminated correctly.                            │");
	printf("\n│• Missing comma.                                                              │");
	printf("\n│• Invalid set member - not an integer.                                        │");
	printf("\n│• Invalid set member - out of range.                                          │");
	printf("\n│• Undefined set name.                                                         │");
	printf("\n╘══════════════════════════════════════════════════════════════════════════════╛\n");
}
