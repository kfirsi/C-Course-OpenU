/*
Kfir Sibirsky	316317221
Eyal Haimov     316316118
=====================================================================================================
This file contains the functions we are using in the second pass in which we are checking for:
• Symbols (externs)
• Instruction / directives.
• Amount of memory to allocate.
• Error checking.
-----------------------------------------------------------------------------------------------------
Included files:
---------------
• "utils.h" - Required data structures, definitions and function prototypes. 
• "second_pass.h" - Required libraries and function prototypes. 
-----------------------------------------------------------------------------------------------------
Global Parameters:
------------------
• (ext_list) *external_list -  points to the external symbol table.
• (extern char) *directives[] -  points to the directives names.
• (extern machine_instruction) *instructions[] -  points to the machine instructions names.
=====================================================================================================
*/
#include "utils.h"
#include "second_pass.h"

ext_list *external_list;
extern char *directives[];
extern machine_instruction instructions[];

/*--------------------------------------------------------------------------------------------
add_to_ext_list: Create a new ext node (external symbol), which consists of the corresponding given 
                 operands (name = name, address = addr).
                 Afterwards, append the new node to the end of the linked list of
                 external symbols (e = external list)
--------------------------------------------------------------------------------------------*/
void add_to_ext_list (ext_list * e, char * name, int addr)
{
    ext * new_node = (ext*) malloc(sizeof(ext));
    ext *last = e->head;
    int cur = 0;
    strcpy(new_node->name,name);
    new_node->address  = addr;
    new_node->next = NULL;
 
	if (e->count == 0)
       e->head = new_node;
    else 
    {
        for(cur=0;cur<(e->count)-1;cur++,last = last->next);
        last->next = new_node;
    }
    e->count++;
    return;   
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
/*--------------------------------------------------------------------------------------------
second_pass: performs the second pass of the assembler on a given file (fptr).
            Returns 1 passed successfully, -1 if not.
--------------------------------------------------------------------------------------------*/
int second_pass(FILE * fptr)
{
    char line[MAX_LINE_LEN],token[MAX_LABEL_LEN]="",label_operand[MAX_LABEL_LEN]="", cpy_str[MAX_LINE_LEN]="", *cpy_ptr;
    int c=0,count = 0, status = 0, error = 0,i,line_number;
	symbol * sptr = symbol_table->head;
	int found = 0;
	external_list = calloc(1, sizeof(ext_list));
    IC = IC_START_ADDR;
    DC = DC_START_ADDR;
    for(line_number = 1; fgets(line, MAX_LINE_LEN, fptr); line_number++) /* Scanning through each line of the file */
    {
		for(c=0;c<MAX_LABEL_LEN;c++)
		{
			token[c]='\0';
			label_operand[c]='\0';
			cpy_str[c]='\0';
		}
		for(c=0;c<MAX_LINE_LEN;c++)
		{
			cpy_str[c]='\0';
		}
    	cpy_ptr=NULL;
    	count = 0;
    	status = 0; 
    	sptr = symbol_table->head;
        strcpy(cpy_str,line);
		cpy_ptr = cpy_str;
		/* Skip unnecessary white spaces at the start of the input. */
		cpy_ptr += skip_white_spaces(cpy_ptr,0);
		/* Check if the input is a blank line. */
		if (cpy_ptr[0] == '\n' || cpy_ptr[0] == '\0' || cpy_ptr[0] == ';') 
		{
			continue;
		}
		/* Get first token. */
		count = skip_non_white_spaces(cpy_ptr,0);
		strncpy(token,cpy_ptr,count);
		status = is_label(token ,instructions, directives, symbol_table,1,line_number);
		if(status == 1)/* VALID LABEL, SKIP to second token. */
		{
			/* Skip the label. */
			cpy_ptr += count;
			/* Skip unnecessary white spaces between label and instruction/directive name. */
			cpy_ptr += skip_white_spaces(cpy_ptr,0);
			/* Get second token. */
			count = skip_non_white_spaces(cpy_ptr,0);
			for(c=0;c<MAX_LABEL_LEN;c++)
			{
				token[c]='\0';
			}
			strncpy(token,cpy_ptr,count);
		}
		/* Skip the instruction / directive name. */
		cpy_ptr += count;
		/* Skip unnecessary white spaces between instruction/directive name and first operand. */
		cpy_ptr += skip_white_spaces(cpy_ptr,0);
		/* Get label operand. */
		count = skip_non_white_spaces(cpy_ptr,0);
		strncpy(label_operand,cpy_ptr,count);
 
		if(!strcmp(token, ".db") || !strcmp(token, ".dh") || !strcmp(token, ".dw") || !strcmp(token, ".asciz") || !strcmp(token, ".extern"))
		{
			continue;
		}
		else if(!strcmp(token, ".entry"))
		{
			while(sptr)
			{
				if(!strcmp(label_operand, sptr->name))
				{
					found = 1;
					if(sptr->type == DATA)
						sptr->type = DATA_AND_ENTRY;
					else if(sptr->type == CODE)
							sptr->type = CODE_AND_ENTRY;
				}
				sptr=sptr->next;
			}	
			if(!found)
			{
		        printf("ERROR in file %s at line %d:\n",file_name,line_number);
				printf("The label %s was not found in symbol table.\n",label_operand);
				error=ERROR;
			}
 			continue;
		}
		else /* Instruction is expected. */
		{	
	        instruction_node *temp = code_image->head;
			for(i=0;i<NUM_OF_INSTRUCTIONS;i++)
			{
				if(!strcmp(token, instructions[i].name))
				{
					break;
				}
			}
	        switch (i)
	        {
	            case 0: /* add */
	            case 1: /* sub */
	            case 2: /* and */
	            case 3: /* or */
	            case 4: /* nor */
	                /* (3 operands: $reg,$reg,$reg) */
	            case 5: /* move */
	            case 6: /* mvhi */
	            case 7: /* mvlo */
	                /* (2 operands: $reg,$reg) */
	            case 13: /* bne */
	            case 14: /* beq */
	            case 15: /* blt */
	            case 16: /* bgt */
	                /* (3 operands: $reg,$reg,label) */
			    case 23: /* jmp */
			 	   /* (1 operand: $reg/label) */
			    case 24: /* la */
			    case 25: /* call */
			       /* (1 operand: label) */
				case 26: /* stop */
				    /* i = 0-4   reached here no problem.
				     * i = 5-7   reached here no problem. 
				     * i = 13-16 reached here no problem. 
				     * i = 23    reached here no problem. 
				     * i = 24-25 reached here no problem. 				        
				     * i = 26    reached here no problem.*/ 

					while (temp != NULL)/* code image list not ended */
					{
						if(temp->address==IC)/* found corresponding instruction node in code image to current line. */
						{
							if(temp->partial.bit == 1)/* machine code in the instruction node is partial. */
							{
								if(i >= 13 && i <= 16)/* bne / beq / blt / bgt */
								{
									/* Since all of the lines are grammatically correct, we can remove all white spaces.  */
									remove_white_spaces(cpy_ptr);
									/* Get string from last comma. */
									cpy_ptr=strrchr(cpy_ptr, ',');
									/* Skip ',' at start. */
									cpy_ptr++;
									/* getting label operand. */
									strcpy(label_operand,cpy_ptr);
									while(sptr)
									{
										if(!strcmp(label_operand, sptr->name))
										{
											found = 1;
											if(sptr->type == EXTERNAL)
											{
										        printf("ERROR in file %s at line %d:\n",file_name,line_number);
												printf("The label %s is external and is used in %s instruction.\n",label_operand,token);
												error=ERROR;
												continue;
											}
											temp->I->immed = sptr->address-IC;
											temp->partial.bit = 0;
										}
										sptr=sptr->next;
									}	
									if(!found)
									{
								        printf("ERROR in file %s at line %d:\n",file_name,line_number);
										printf("The label %s was not found in symbol table.\n",label_operand);
										error=ERROR;
									}
								}
								else if(i >= 23 && i <= 25)/* jmp / la / call */
								{
									while(sptr)
									{
										if(!strcmp(label_operand, sptr->name))
										{
											found = 1;
											temp->J->address = sptr->address;
											temp->partial.bit = 0;
											if(sptr->type == EXTERNAL)
											{
												add_to_ext_list (external_list, label_operand, IC);
											}
										}
										sptr=sptr->next;
									}	
									if(!found)
									{
								        printf("ERROR in file %s at line %d:\n",file_name,line_number);
										printf("The label %s was not found in symbol table.\n",label_operand);
										error=ERROR;
									}
								}
							}
							if(i >= 23 && i <= 25)/* jmp / la / call */
								{
									while(sptr)
									{
										if(!strcmp(label_operand, sptr->name))
										{
											if(sptr->type == EXTERNAL)
											{
												add_to_ext_list (external_list, label_operand, IC);

											}
										}
										sptr=sptr->next;
									}
								}	
							break;
						}
						temp = temp->next;
					}
	            case 8:  /* addi */
	            case 9:  /* subi */
	            case 10: /* andi */
	            case 11: /* ori */
	            case 12: /* nori */
	            case 17: /* lb */
	            case 18: /* sb */
	            case 19: /* lw */
	            case 20: /* sw */
	            case 21: /* lh */
	            case 22: /* sh */
		            IC+=4;
				    continue;
		    }/*end of switch(i)*/
		}/*end else MUST BE AN INSTRUCTION. */
	}/* end for loop of lines in files. */
	return error;
}
/*--------------------------------------------------------------------------------------------
int_to_bin_str: Converts a decimal number (n) to a (len) bits binary number.
                Stores the result as a string (in str).
--------------------------------------------------------------------------------------------*/
void int_to_bin_str(int n, int len, char *str)
{
    char bin[33]="";
    int i = len-1;
	while(i>=0)
    {
        (n&(1u<<i--))?strcat(bin, "1"):strcat(bin, "0"); 
    }
    strncpy(str,bin,len);
}
/*--------------------------------------------------------------------------------------------
bin_str_to_hex_str: Converts a binary string (bin) to hexadecimal string (hex) in which the
                    order of the hexa digits is as described in page 40 in the course booklet. 
--------------------------------------------------------------------------------------------*/
void bin_str_to_hex_str(char *bin, char *hex)
{
    char temp[4]="";
    int hex_right=0, hex_left=0, mult;
    int i=strlen(bin)-1,j=0,k;
    /* binary to hexadecimal conversion is as follows:
     * 0111 -> 0*(2^3) + 1*(2^2) + 1*(2^1) + 1*(2^0) = 
            -> 0*8     + 1*4     + 1*2     + 1*1     = 
            -> 0       + 4       + 2       + 1       =
            -> 7
    */
    while(i>=0)
    {
    	/* Start from the right most bit. */
        k=i;
        /* Calculating the hex value of the right most group */
        for(mult=1, j=3; j>=0; j--, k--)
        {
            hex_right += (bin[k]=='0'? 0 : mult);
            mult *= 2;
        }
        /* Calculating the hex value of the group that is left to the right most group */
        for(mult=1, j=3; j>=0; j--, k--)
        {
            hex_left += (bin[k]=='0'? 0 : mult);
            mult *= 2;
        }
    	/* Skipping left over the bits that was checked. */
        i=k;
        /* Concatenate left group then right group to hex string. */
        sprintf(temp,"%X%X ",hex_left,hex_right);
        strcat(hex,temp);
        hex_right=0;
        hex_left=0;
    }
}
