/*
Kfir Sibirsky	316317221
Eyal Haimov     316316118
=====================================================================================================
This file contains the functions we are using in the first pass in which we are checking for:
• Labels.
• Symbols.
• Instruction / directives.
• Amount of memory to allocate.
• Error checking.
-----------------------------------------------------------------------------------------------------
Included files:
---------------
• "utils.h" - Required data structures, definitions and function prototypes. 
• "first_pass.h" - Required libraries and function prototypes. 
-----------------------------------------------------------------------------------------------------
Global parameters:
------------------
• (int) IC - Instruction counter.
• (int) DC - Data counter.
• (int) ICF - Final value of instruction counter.
• (int) DCF - Final value of data counter.          
• (symbol_list) *symbol_table -  points to the symbol table.
• (instruction_list) *code_image - points to the code image: I, J, R.
• (directive_list) *data_image - points to the data image: .db / .asciz - 8 bytes, .dh - 16 bytes,
                                 .dw - 32 bytes.              
=====================================================================================================
*/

#include "utils.h"
#include "first_pass.h"

int IC, DC, ICF, DCF;
symbol_list *symbol_table;
instruction_list *code_image;
directive_list *data_image;

/*--------------------------------------------------------------------------------------------
instructions: Machine instruction table from page 19 in the course booklet.
--------------------------------------------------------------------------------------------*/
machine_instruction instructions[NUM_OF_INSTRUCTIONS] = {			
{"add",'R',1,0},
{"sub",'R',2,0},
{"and",'R',3,0},
{"or",'R',4,0},
{"nor",'R',5,0},
{"move",'R',1,1},
{"mvhi",'R',2,1},
{"mvlo",'R',3,1},
{"addi",'I',0,10},
{"subi",'I',0,11},
{"andi",'I',0,12},
{"ori",'I',0,13},
{"nori",'I',0,14},
{"bne",'I',0,15},
{"beq",'I',0,16},
{"blt",'I',0,17},
{"bgt",'I',0,18},
{"lb",'I',0,19},
{"sb",'I',0,20},
{"lw",'I',0,21},
{"sw",'I',0,22},
{"lh",'I',0,23},
{"sh",'I',0,24},
{"jmp",'J',0,30},
{"la",'J',0,31},
{"call",'J',0,32},
{"stop",'J',0,63}
};
/*--------------------------------------------------------------------------------------------
directives: Directives names.
--------------------------------------------------------------------------------------------*/
char *directives[NUM_OF_DIRECTIVES] = {".db",".dh",".dw",".asciz",".entry",".extern"};




/*--------------------------------------------------------------------------------------------
add_to_code_image: Create a new instruction node of given type (i_type = R / I / J) according
                   to a given index (i_idx) to the instruction table.
                   The new node consists of the corresponding given operands 
                   (oprnd1 / oprnd2 / oprnd 3 / label) to the expected operands of the 
                   instruction at i_idx.
                   Afterwards, append the new node to the end of the linked list of
                   instructions (ci = code image)
--------------------------------------------------------------------------------------------*/
void add_to_code_image (instruction_list *ci, int i_type,int i_idx, int oprnd1, int oprnd2, int oprnd3,symbol_list * symbols, char * label)
{
	int cur = 0;
    instruction_node *new_node = (instruction_node*) malloc(sizeof(instruction_node));
    instruction_node *last = ci->head;
    new_node->partial.bit = 0;
    if(i_type == 'R')
    {
        R_instruction * r = (R_instruction*) malloc(sizeof(R_instruction));
        r->opcode=instructions[i_idx].opcode;
        if(i_idx>=0 && i_idx<=4)
        {
            r->rs=oprnd1;
            r->rt=oprnd2;
            r->rd=oprnd3;
        }
        else/*5-7*/
        {
            r->rs=oprnd2;
            r->rd=oprnd1;
            r->rt=0;
        }
        r->funct=instructions[i_idx].funct;
        r->not_used=0;
        new_node->R = r;
        new_node->I = NULL;
        new_node->J = NULL;
    }
    else if(i_type == 'I')
    {
        I_instruction * i = (I_instruction*) malloc(sizeof(I_instruction));
        i->opcode=instructions[i_idx].opcode;
        if(i_idx>=13 && i_idx<=16 )
        {
            int label_addr = ERROR;
        	symbol * sptr = symbols->head;
        	while(sptr)
        	{
        		if(!strcmp(label, sptr->name))
        		{
                    label_addr=sptr->address;
                    break;
        		}
        		sptr=sptr->next;
        	}
    	    /* Check if symbol not found in symbol table in the first pass. */
        	if(label_addr == ERROR)
				new_node->partial.bit = 1;
        	else
	            i->immed = label_addr-IC;
            i->rs=oprnd1;
            i->rt=oprnd2;
        }
        else/*8-12 OR 17-22*/
        {
            i->rs=oprnd1;
            i->rt=oprnd3;
            i->immed=oprnd2;
        }
        new_node->I = i;
        new_node->R = NULL;
        new_node->J = NULL;
    }
    else if(i_type == 'J')
    {
        J_instruction * j = (J_instruction*) malloc(sizeof(J_instruction));
        j->opcode=instructions[i_idx].opcode;
        j->reg=0;
        if(i_idx>=23 && i_idx<=25)
        {
            if(label[0]!='\0')/* Meaning: reg = 0 */
            {
                int label_addr = ERROR;
            	symbol * sptr = symbols->head;
            	while(sptr)
            	{
            		if(!strcmp(label, sptr->name))
            		{
                        label_addr=sptr->address;
                        break;
            		}
            		sptr=sptr->next;
            	}
	    	    /* Check if symbol not found in symbol table in the first pass. */
	        	if(label_addr == ERROR)
					new_node->partial.bit = 1;
            	else
                    j->address = (sptr->type == EXTERNAL? 0 : label_addr);
            }
            else
            {
                if(i_idx==23)
                {
                    j->reg=1;
                    j->address=oprnd1;
                }
            }
        }
        else/*26*/
            j->address=0;
        new_node->J = j;
        new_node->R = NULL;
        new_node->I = NULL;
    }    
    new_node->address = IC;
    if (ci->count == 0)
       ci->head = new_node;
    else 
    {
        for(cur=0;cur<(ci->count)-1;cur++,last = last->next);
        last->next = new_node;
    }
    ci->count++;
}
/*--------------------------------------------------------------------------------------------
add_to_data_image: Create a new directive node of (8 bit / 16 bit / 32 bit) according
                   to a given index (d_idx) to the directive table.
                   The new node consists of the corresponding given number (to_store) 
                   to the expected operand of the directive at d_idx.
                   Afterwards, append the new node to the end of the linked list of
                   directives (di = data image)
--------------------------------------------------------------------------------------------*/
void add_to_data_image (directive_list *di, int d_idx, int to_store)
{
	int cur =0;
    directive_node *new_node = (directive_node*) malloc(sizeof(directive_node));
    directive_node *last = di->head;
	
    if(d_idx == 0 || d_idx == 3)/* .db / .asciz */
    {

        _8_bit_directive  * _8b = (_8_bit_directive*) malloc(sizeof(_8_bit_directive));
        _8b->data = to_store;
        new_node->_8_bit = _8b;
        new_node->_16_bit = NULL;
        new_node->_32_bit = NULL;
    }
    else if(d_idx == 1)/* .dh */
    {
        _16_bit_directive  * _16b = (_16_bit_directive*) malloc(sizeof(_16_bit_directive));
        _16b->data = to_store;
        new_node->_8_bit = NULL;
        new_node->_16_bit = _16b;
        new_node->_32_bit = NULL;
    }
    else if(d_idx == 2)/* .dw */
    {
        _32_bit_directive  * _32b = (_32_bit_directive*) malloc(sizeof(_32_bit_directive));
        _32b->data = to_store;
        new_node->_8_bit = NULL;
        new_node->_16_bit = NULL;
        new_node->_32_bit = _32b;
    }

    new_node->address = DC;
    if (di->count == 0)
       di->head = new_node;
    else
    {
        for(cur=0;cur<(di->count)-1;cur++,last = last->next);
        last->next = new_node;
    }
    di->count++;
}

/*--------------------------------------------------------------------------------------------
add_to_symbol_table: Create a new symbol node, which consists of the corresponding given 
                     operands (name = name, address = addr, type = ty).
                     Afterwards, append the new node to the end of the linked list of
                     symbols (st = symbol table)
--------------------------------------------------------------------------------------------*/
void add_to_symbol_table (symbol_list *st, char * name, int addr, sym_type ty)
{
    symbol * new_node = (symbol*) malloc(sizeof(symbol));
    symbol *last = st->head;
    int cur = 0;
    strcpy(new_node->name,name);
    new_node->address  = addr;
    new_node->type  = ty;
    new_node->next = NULL;
 
     if (st->count == 0)
       st->head = new_node;
	else
	{
        for(cur=0;cur<(st->count)-1;cur++,last = last->next);
        last->next = new_node;
    }
    st->count++;
    return;  
    
}

/*--------------------------------------------------------------------------------------------
increase_address_to_data: Adds the value of ICF to the address of each symbol in the symbol
                          table which characterized as "data". also, add the value of ICF to
                          the address of each node in the linked list of directives 
                          (data image)
--------------------------------------------------------------------------------------------*/
void increase_address_to_data(symbol_list *st, directive_list *di)
{
    symbol *s = st->head;
	directive_node *d = di->head;
	int cur;
    for(cur=0;cur<st->count;cur++,s = s->next)
    {
        if(s->type==DATA)
        	s->address+=ICF;
    }
    for(cur=0;cur<di->count;cur++,d = d->next)
	{
		d->address+=ICF;
	}
}

/*--------------------------------------------------------------------------------------------
first_pass: performs the first pass of the assembler on a given file (fptr).
            Returns 1 passed successfully, -1 if not.
--------------------------------------------------------------------------------------------*/
int first_pass(FILE *fptr)
{
    char line[MAX_LINE_LEN]="", cpy_str[MAX_LINE_LEN]="", *cpy_ptr;
    char first[MAX_LABEL_LEN]="",second[MAX_LABEL_LEN]="", *sus;
    int c=0,count = 0, status = 0, error = 0,i=0,first_char_after_colon='\0',line_number=1,line_err=0;
    long operands[3];
    int empty_file = 1;
	symbol_table = calloc(1, sizeof(symbol_list));
	code_image = calloc(1, sizeof(instruction_list));
	data_image = calloc(1, sizeof(directive_list));
    IC = IC_START_ADDR;
    DC = DC_START_ADDR;
    /* Read lines until end of file. */
    for(line_number = 1; fgets(line, MAX_LINE_LEN, fptr); line_number++) 
    {
    	/* Reset variables. */
		for(c=0;c<MAX_LABEL_LEN;c++)
		{
			first[c]='\0';
			second[c]='\0';
			cpy_str[c]='\0';
		}
    	cpy_ptr=NULL;
    	sus=NULL;
    	line_err=0;
    	count = 0;
    	status = 0; 
        strcpy(cpy_str,line);
		cpy_ptr = cpy_str;
		/* Skip unnecessary white spaces at the start of the input. */
		cpy_ptr += skip_white_spaces(cpy_ptr,0);
		/* Check if the input is a blank line. */
		if (cpy_ptr[0] == '\n' || cpy_ptr[0] == '\0' || cpy_ptr[0] == ';') 
		{
			continue;
		}
		empty_file = 0;
		/* Gets first token. */
		count = skip_non_white_spaces(cpy_ptr,0);
		strncpy(first,cpy_ptr,count);
		/* Checks for a valid label. */
		status = is_label(first ,instructions, directives, symbol_table,0,line_number);
		/* Skip the first token. */
		cpy_ptr += count;
		first_char_after_colon = cpy_ptr[0];
		/* Skip unnecessary white spaces between first and second token. */
		cpy_ptr += skip_white_spaces(cpy_ptr,0);
		/* Gets second token. */
		count = skip_non_white_spaces(cpy_ptr,0);
		strncpy(second,cpy_ptr,count);
		/* First token is suspected for being instruction / directive name. */
		sus = first;
		/* Checks for an invalid label. */
		if(status == -1)
		{
			error=ERROR;
			line_err=ERROR;
			continue;
		}
		if(status == 1)
		{
			/* Expecting white space (one or more). */
			if(!isspace(first_char_after_colon))
			{
				printf("ERROR in file %s at line %d:\n",file_name,line_number);
				printf("Non white space character was found right after label's colon.\n");
				printf("%s:%c\n",first,first_char_after_colon);
				printf("%*c\n",strlen(first)+2,'^');/* 2 = 1 for colon + 1 one char more. */
				error=ERROR;
				line_err=ERROR;
				continue;
			}
			/* Skip the second token. */
			cpy_ptr += count;
			/* Skip unnecessary white spaces between second and third token. */
			cpy_ptr += skip_white_spaces(cpy_ptr,0);
			/* Second token is suspected for being instruction / directive name. */
			sus = second;
			/* Remove colon after label end. */
	        first[strlen(first)-1]='\0';
		}
		/* Now, sus is suspected for being instruction / directive name. */
		if(!strcmp(sus, ".db") || !strcmp(sus, ".dh") || !strcmp(sus, ".dw") || !strcmp(sus, ".asciz"))
		{
		    long temp_num=0;
			if (status == 1) 
			{
				add_to_symbol_table(symbol_table, first, DC, DATA);
	   		}
	   		if (strcmp(sus, ".asciz"))/*.db / .dh / .dw*/
	   		{
				while(*cpy_ptr)
				{
					cpy_ptr += get_immed(cpy_ptr,&temp_num, &line_err,line_number);
					if(line_err==ERROR)
					{
						error=ERROR;
						break;
					}
				
	    	   		if(!strcmp(sus, ".db"))
	    	   		{	                
	                  if(temp_num > CHAR_MAX || temp_num < CHAR_MIN)
	                    {
            				printf("ERROR in file %s at line %d:\n",file_name,line_number);
	                        printf("Data to store is out of bounds of Two’s Complement conversion.\n");
	                        printf("%ld (decimal) is not in the range [%d,%d].\n",temp_num,CHAR_MIN,CHAR_MAX);
	                        error=ERROR;
	                        line_err=ERROR;
							break;
	                    }
	    	   		    add_to_data_image (data_image, 0, temp_num);
	   	                DC++;
	    	   		}
		   		    else if(!strcmp(sus, ".dh"))
	    	   		{
	                  if(temp_num > SHRT_MAX || temp_num < SHRT_MIN)
	                    {
            				printf("ERROR in file %s at line %d:\n",file_name,line_number);
	                        printf("Data to store is out of bounds of Two’s Complement conversion.\n");
	                        printf("%ld (decimal) is not in the range [%d,%d].\n",temp_num,SHRT_MIN,SHRT_MAX);
	                        error=ERROR;
	                        line_err=ERROR;
							break;
	                    }
	    	   		    add_to_data_image (data_image, 1, temp_num);
	   	                DC+=2;
		   		    }
	    	   		else if(!strcmp(sus, ".dw"))
	    	   		{
	                    if(temp_num > INT_MAX || temp_num < INT_MIN)
	                    {
            				printf("ERROR in file %s at line %d:\n",file_name,line_number);
	                        printf("Data to store is out of bounds of Two’s Complement conversion.\n");
	                        printf("%ld (decimal) is not in the range [%d,%d].\n",temp_num,INT_MIN,INT_MAX);
	                        error=ERROR;
	                        line_err=ERROR;
							break;
	                    }
	    	   		    add_to_data_image (data_image, 2, temp_num);
	   	                DC+=4;
		   		    }
					cpy_ptr += skip_white_spaces(cpy_ptr,0);
					cpy_ptr += get_comma(cpy_ptr, &line_err,line_number);
					if(line_err==ERROR)
					{
						error=ERROR;
						break;
					}
	    	    }/*end while*/
	    	    if(line_err == ERROR)
	    	    	continue;
   			}
   			else 
   			{ /*asciz*/
	   	        int j=0;
		    	if(cpy_ptr[j]=='\"')/* opening quotation mark found. */
		    	{
		            j++;
		            while(j < strlen(cpy_ptr) && cpy_ptr[j]!='\"')
		            {
		                if(!isprint(cpy_ptr[j]))
		            	{
            				printf("ERROR in file %s at line %d:\n",file_name,line_number);
		                    printf("Character that has the value %d (decimal) is not a printable ASCII character.\n",cpy_ptr[j]);
		                    error=ERROR;
		                    line_err=ERROR;
							break;
		            	}
        	   		    add_to_data_image (data_image, 3, cpy_ptr[j]);
            	   		/* add cpy_ptr[j] to DATA IMAGE (MACHINE CODE). */
		                DC++;
		                j++;
		            }
    	    	    if(line_err == ERROR)
	    	    		continue;
		       	    if(cpy_ptr[j]!='\"')
		            {
        				printf("ERROR in file %s at line %d:\n",file_name,line_number);
		                printf("Missing closing quotation mark after last printable character.\n");
		                error=ERROR;
		                line_err=ERROR;
						continue;
		            }
    	   		    add_to_data_image (data_image, 0,0);
	                DC++;
		            j++;/* closing quotation mark found. */
		        	while(j<strlen(cpy_ptr) && isspace(cpy_ptr[j]))
		        	{
		        	    j++;
		        	}
		            if(cpy_ptr[j]!='\0' && cpy_ptr[j]!='\n')
		            {
        				printf("ERROR in file %s at line %d:\n",file_name,line_number);
		                printf("Extraneous text after comand.\n");
		                error=ERROR;
		                line_err=ERROR;
						continue;
		            }
		    	}
		    	else
		    	{
    				printf("ERROR in file %s at line %d:\n",file_name,line_number);
		            if(isprint(cpy_ptr[j]))
		                printf("Missing opening quotation mark before first printable character.\n");
		            else
		                printf("Missing parameter, non printable character found instead.\n");
	                error=ERROR;
	                line_err=ERROR;
					continue;
		    	}
	   		}
	   		/* Continue to next line. */
			continue;
		}
		else if(!strcmp(sus, ".entry"))
		{
			/* This is handled in second pass.
			 * Continue to next line. */
 			continue;
		}
		else if(!strcmp(sus, ".extern"))
		{
			if (status == -2)
				sus = second;
			if (status == 1) 
			{
	   			/* VALID label but need to skip it, not relevant. 
	   			 * Add to symbol table (TYPE = EXTERNAL , ADDR = 0).
	   			 * Now, expecting whitespace (one or more). */
				char third[MAX_LABEL_LEN]="";
				/* Skip the second token. */
				cpy_ptr += count;
				/* Skip unnecessary white spaces between second and third token. */
				cpy_ptr += skip_white_spaces(cpy_ptr,0);
				/* Gets third token. */
				count = skip_non_white_spaces(cpy_ptr,0);
				strncpy(third,cpy_ptr,count);
				/* First token is suspected for being operand. */
				sus = third;
   			}
			add_to_symbol_table(symbol_table, sus, 0, EXTERNAL);
   			continue;
		}
		else /* Instruction is expected. */
		{	
			int found = 0;
			/* check which instruction is it, if even valid? */
			for(i=0;i<NUM_OF_INSTRUCTIONS;i++)
			{
				if(!strcmp(sus, instructions[i].name))
				{
					found = 1;
					break;
				}
			}
			if(found == 0)
			{
				printf("ERROR in file %s at line %d:\n",file_name,line_number);
				printf("Undefined instruction / directive name.\n");
				error=ERROR;
				line_err=ERROR;
				continue;
			}
			else
			{
		        char label_operand[MAX_LABEL_LEN]="";
				if (status == 1) 
				{
					add_to_symbol_table(symbol_table, first, IC, CODE);
		        }
		        switch (i)
		        {
		            case 0: /* add */
		            case 1: /* sub */
		            case 2: /* and */
		            case 3: /* or */
		            case 4: /* nor */
		                /* (3 operands: $reg,$reg,$reg) */
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
		                /* (3 operands: $reg,immed,$reg) */
						
						
						cpy_ptr += get_reg(cpy_ptr,&operands[0], &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						cpy_ptr += skip_white_spaces(cpy_ptr,0);
						cpy_ptr += get_comma(cpy_ptr, &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						if(i >= 0 && i <= 5)
							cpy_ptr += get_reg(cpy_ptr,&operands[1], &line_err,line_number);
						else
							cpy_ptr += get_immed(cpy_ptr,&operands[1], &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						cpy_ptr += skip_white_spaces(cpy_ptr,0);
						cpy_ptr += get_comma(cpy_ptr, &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						cpy_ptr += get_reg(cpy_ptr,&operands[2], &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						cpy_ptr += skip_white_spaces(cpy_ptr,0);
						/* Check extraneous text. */
					    if(cpy_ptr[0]!='\n' && cpy_ptr[0]!='\0')
					    {
		    				printf("ERROR in file %s at line %d:\n",file_name,line_number);
					        printf("Extraneous text after comand.\n");
					        error=ERROR;
					        continue;
					    }
						if(i >= 0 && i <= 5)
	                    	add_to_code_image(code_image,'R',i,operands[0],operands[1],operands[2],symbol_table,label_operand);
	                    else
                    		add_to_code_image(code_image,'I',i,operands[0],operands[1],operands[2],symbol_table,label_operand);
			            IC+=4;
					    continue;
		            case 5: /* move */
		            case 6: /* mvhi */
		            case 7: /* mvlo */
		                /* (2 operands: $reg,$reg) */
		            case 13: /* bne */
		            case 14: /* beq */
		            case 15: /* blt */
		            case 16: /* bgt */
		                /* (3 operands: $reg,$reg,label) */
						cpy_ptr += get_reg(cpy_ptr,&operands[0], &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						cpy_ptr += skip_white_spaces(cpy_ptr,0);
						cpy_ptr += get_comma(cpy_ptr, &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						cpy_ptr += get_reg(cpy_ptr,&operands[1], &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						cpy_ptr += skip_white_spaces(cpy_ptr,0);
						if(i >= 5 && i <= 7)
						{
							/* Check extraneous text. */
							if(*cpy_ptr && cpy_ptr[0]!='\n' && cpy_ptr[0]!='\0')
							{
								printf("ERROR in file %s at line %d:\n",file_name,line_number);
							    printf("Extraneous text after comand.\n");
							    error=ERROR;
							    continue;
							}
			                add_to_code_image(code_image,'R',i,operands[0],operands[1],0,symbol_table,label_operand);
					        IC+=4;
							continue;
						}
						cpy_ptr += get_comma(cpy_ptr, &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						cpy_ptr += skip_white_spaces(cpy_ptr,0);
						cpy_ptr += get_label(cpy_ptr, label_operand, &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						/* Check extraneous text. */
					    if(!isspace(cpy_ptr[0]) && cpy_ptr[0]!='\0')
					    {
		    				printf("ERROR in file %s at line %d:\n",file_name,line_number);
					        printf("Extraneous text after comand.\n");
					        error=ERROR;
					        continue;
					    }
	                    add_to_code_image(code_image,'I',i,operands[0],operands[1],0,symbol_table,label_operand);
			            IC+=4;
					    continue;
					case 23: /* jmp */
				       /* (1 operand: $reg/label) */
		    		case 24: /* la */
				    case 25: /* call */
				       /* (1 operand: label) */
						if(i == 23)
							cpy_ptr += get_label_or_reg(cpy_ptr, &operands[0], label_operand, &line_err,line_number);
						else
							cpy_ptr += get_label(cpy_ptr, label_operand, &line_err,line_number);
						if(line_err==ERROR)
						{
							error=ERROR;
							continue;
						}
						cpy_ptr += skip_white_spaces(cpy_ptr,0);
				    case 26: /* stop */
						/* Check extraneous text. */
					    if(cpy_ptr[0]!='\n' && cpy_ptr[0]!='\0')
					    {
		    				printf("ERROR in file %s at line %d:\n",file_name,line_number);
					        printf("Extraneous text after comand.\n");
					        error=ERROR;
					        continue;
					    }
					    if(i==23)
					    	add_to_code_image(code_image,'J',i,operands[0],0,0,symbol_table,label_operand);
		                else if(i==24||i==25) /*label */
	                    	add_to_code_image(code_image,'J',i,0,0,0,symbol_table,label_operand);
	                    else
		                    add_to_code_image(code_image,'J',i,0,0,0,symbol_table,label_operand);
			            IC+=4;
					    continue;
			    }/*end of switch(i)*/
			}/*end of else (just like found == 1, where found is good insturction or not.)*/
		}/*end else MUST BE AN INSTRUCTION. */
	}/* end for loop of lines in files. */
	/* Saving the last value of the counters. */
	ICF = IC;
	DCF = DC;
	if(empty_file)
		error=ERROR;
	if(!error) /* If the code in the file valid so far, we increase the addresses of the data variables */
	    increase_address_to_data(symbol_table, data_image);

	rewind(fptr); /* Rewinds the file for the second pass and returns a flag regarding a spotted error in the file. */
	return error;
}
/*--------------------------------------------------------------------------------------------
skip_white_spaces: Returns the number of white space characters from a given index (start_at)
                   to the first non-white space character for a given string (str).
--------------------------------------------------------------------------------------------*/
int skip_white_spaces(char *str ,int start_at)
{
	int count=start_at;
   	while(count<strlen(str) && isspace(str[count]))
	{
		count++;
	}
	return count;
}
/*--------------------------------------------------------------------------------------------
skip_non_white_spaces: Returns the number of non-white space characters from a given index 
                       (start_at) to the first white space character for a given string (str).
--------------------------------------------------------------------------------------------*/
int skip_non_white_spaces(char *str,int start_at)
{
	int count=start_at;
   	while(count<strlen(str) && !isspace(str[count]))
	{
		count++;
	}
	return count;
}
/*--------------------------------------------------------------------------------------------
is_label: checks whether a given label (tkn) is valid.
          returns 1 if so, -1 if not and -2 if there was no label at all.
--------------------------------------------------------------------------------------------*/
int is_label(char * tkn ,machine_instruction instructions[] ,char * directives[] ,symbol_list * symbols, int is_parameter_check, int line_number)
{
    int i=0;
	char str[MAX_LABEL_LEN]="";
	symbol * sptr = symbols->head;
	int col=0;
	if(tkn[strlen(tkn)-1]==':')
	{
	    col = 1;
	    strncpy(str,tkn,strlen(tkn)-1);
	}
	else
	    strncpy(str,tkn,strlen(tkn));
    if(col == 1 && strlen(str)==0)
    {
		printf("ERROR in file %s at line %d:\n",file_name,line_number);
		printf("Blank label.");
        return -1;
    }
    if(col == 1 && strlen(str)>MAX_LABEL_LEN)
    {
		printf("ERROR in file %s at line %d:\n",file_name,line_number);
		printf("Label has exceeded maximum valid length (%d characters long)",MAX_LABEL_LEN);
        return -1;
    }
	for(i=0;i<NUM_OF_INSTRUCTIONS;i++)
	{
		if(!strcmp(str, instructions[i].name) || (i<NUM_OF_DIRECTIVES && !strcmp(str, directives[i])))
		{
		    if(col == 1)
		    {
				printf("ERROR in file %s at line %d:\n",file_name,line_number);
		    	printf("Symbol cannot be a reserved word.\n");
    	        return -1;
		    }
	        return -2;
        }
	}
    if(!isalpha(str[0]))
	{
		printf("ERROR in file %s at line %d:\n",file_name,line_number);
		if(col == 1)
    		printf("First char of label is not alphabetic (lower/upper letter).\n");
    	else
		    printf("Undefined instruction / directive name.\n");
        return -1;
	}
	for(i=1;i<strlen(str);i++)
	{
		if(!isalnum(str[i]))
    	{
			printf("ERROR in file %s at line %d:\n",file_name,line_number);
    		if(col == 1)
    			printf("%d%s character of label is not alphanumeric (letter / digit)\n",i+1,eng_ordinal_nums(i+1));
        	else
    		    printf("Undefined instruction / directive name.\n");
	        return -1;
    	}
    }
	while(sptr)
	{
		if(!strcmp(str, sptr->name) && sptr->type != EXTERNAL )
		{
		    if(col == 1)
		    {    
		        if(is_parameter_check == 1)
		        {
		            return 1;
		        }
	    		printf("ERROR in file %s at line %d:\n",file_name,line_number);
	    	    printf("Symbol already exist in symbol table.\n");
		    }
		    else
		    {
	    		printf("ERROR in file %s at line %d:\n",file_name,line_number);
    		    printf("Missing colon after label.\n");
		    }
            return -1;
		}
		sptr=sptr->next;
	}
	if(col == 0)
	{
		printf("ERROR in file %s at line %d:\n",file_name,line_number);
		printf("Missing colon after label.\n");
	    return -1;
	}
	return 1;
}
/*--------------------------------------------------------------------------------------------
get_reg: Check whether there is a register at the start of a given string (cpy_ptr).
         If so, (reg_num) gets the register number, else (error) gets the error status.
         returns the length of the string containing the '$' sign and the register number 
         (e.g., "$25" returns 3).
--------------------------------------------------------------------------------------------*/
int get_reg(char * cpy_ptr, long *reg_num , int *error, int line_number )
{
    int cur_idx=0;
    char cur_char[1]="", temp_num[82] ="";
    if(cpy_ptr[cur_idx]=='$')
	{
	    cur_idx++;
	    if(!isdigit(cpy_ptr[cur_idx]))
	    {
	        printf("ERROR in file %s at line %d:\n",file_name,line_number);
	        printf("Invalid parameter - expected digit after '$' but found '%c'.\n",cpy_ptr[cur_idx]);
	        *error=ERROR;
	    }
	    while(isdigit(cpy_ptr[cur_idx]))
	    {
		    cur_char[0]=cpy_ptr[cur_idx];
		    strncat(temp_num,cur_char,1);
		    cur_idx++;
	    }
	    if(cpy_ptr[cur_idx]=='.')
	    {
	        printf("ERROR in file %s at line %d:\n",file_name,line_number);
	        printf("Invalid parameter - register must be integer.\n");
	        *error=ERROR;
	    }
	    if(atol(temp_num)>31 || atol(temp_num)<0)
	    {
	        printf("ERROR in file %s at line %d:\n",file_name,line_number);
	        printf("Invalid parameter - register must be $0-$31.\n");
	        *error=ERROR;
	    }
	}
	else
	{
        printf("ERROR in file %s at line %d:\n",file_name,line_number);
        printf("Invalid parameter - register must start with '$'.\n");
        *error=ERROR;
	}
    *reg_num = atol(temp_num);
    return cur_idx;
}
/*--------------------------------------------------------------------------------------------
get_immed: Check whether there is an immediate number at the start of a given string (cpy_ptr).
           If so, (num) gets the immediate number, else (error) gets the error status. 
           returns the length of the string containing the '+' / '-' sign and the number 
           (e.g., "-533" returns 3).
--------------------------------------------------------------------------------------------*/
int get_immed(char * cpy_ptr, long *num , int *error, int line_number )
{
    int cur_idx=0;
    char cur_char[1]="", temp_num[82] ="";
    
    if(isdigit(cpy_ptr[cur_idx])||cpy_ptr[cur_idx]=='-'||cpy_ptr[cur_idx]=='+')
	{
        if(cpy_ptr[cur_idx]=='-'||cpy_ptr[cur_idx]=='+')
	    {
		    cur_char[0]=cpy_ptr[cur_idx];
		    strncat(temp_num,cur_char,1);
		    cur_idx++;
            if(cpy_ptr[cur_idx]=='-'||cpy_ptr[cur_idx]=='+')
	        {
    	        printf("ERROR in file %s at line %d:\n",file_name,line_number);
    	        printf("Invalid parameter - more than one '+' / '-' sign.\n");
	            *error=ERROR;
	        }
	        if(isspace(cpy_ptr[cur_idx]))
            {
                printf("ERROR in file %s at line %d:\n",file_name,line_number);
                printf("White space between '%c' sign and digit.\n",cpy_ptr[cur_idx-1]);
                *error=ERROR;
            }  
	        if(!isdigit(cpy_ptr[cur_idx]))
            {
	            printf("ERROR in file %s at line %d:\n",file_name,line_number);
	            printf("Invalid parameter - expected digit after '%c' but found '%c'.\n",cpy_ptr[cur_idx-1],cpy_ptr[cur_idx]);
                *error=ERROR;
            }  
	    }
	    while(isdigit(cpy_ptr[cur_idx]))
	    {
		    cur_char[0]=cpy_ptr[cur_idx];
		    strncat(temp_num,cur_char,1);
		    cur_idx++;
	    }
	    if(cpy_ptr[cur_idx]=='.')
	    {
	        printf("ERROR in file %s at line %d:\n",file_name,line_number);
	        printf("Invalid parameter - non-integers are not supported.\n");
	        *error=ERROR;
	    }
	}
	else
	{
        printf("ERROR in file %s at line %d:\n",file_name,line_number);
        printf("Invalid parameter - expected '+' / '-' sign or digit but found '%c'.\n",cpy_ptr[cur_idx]);
        *error=ERROR;
	}
    *num = atol(temp_num);
    return cur_idx;
}
/*--------------------------------------------------------------------------------------------
get_comma: Check whether there is a comma at the start of a given string (cpy_ptr),
           and whether there are multiple consecutive commas or missing commas.
           (error) gets the error status. 
           Skips to the first non-white space character after the comma, and returns that
           index.  
--------------------------------------------------------------------------------------------*/
int get_comma(char * cpy_ptr , int *error, int line_number )
{
    int cur_idx=0;
    if(cpy_ptr[cur_idx]==',')
	{
	    cur_idx++;
	    cur_idx += skip_white_spaces(cpy_ptr,cur_idx)-1;
        if(cpy_ptr[cur_idx]==',')
	    {				                
	        printf("ERROR in file %s at line %d:\n",file_name,line_number);
	        printf("Multiple consecutive commas.\n");
            *error=ERROR;
	    }
	}
	else
	{
        if(isdigit(cpy_ptr[cur_idx]) || cpy_ptr[cur_idx] == '$')/* ALSO + / -*/
        {
            printf("ERROR in file %s at line %d:\n",file_name,line_number);
            printf("Missing comma.\n");
            *error=ERROR;
            return cur_idx;
        }
        if(!isspace(cpy_ptr[cur_idx]) && cpy_ptr[cur_idx]!='\0')
        {
            printf("ERROR in file %s at line %d:\n",file_name,line_number);
            printf("Not an integer in or after number list.\n");
            *error=ERROR;
            return cur_idx;
        }
	}
    return cur_idx;
}

/*--------------------------------------------------------------------------------------------
get_label_or_reg: Check whether there is a label or a register at the start of a given string
                  (cpy_ptr). Accordingly (label_operand) gets the label or (reg_num) gets the 
                  register number. Else, (error) gets the error status. 
                  Returns the length of the string containing the label 
                  (e.g., "example_label" returns 13). or string containing the '$' sign and 
                  the register number (e.g., "$25" returns 3).
--------------------------------------------------------------------------------------------*/
int get_label_or_reg(char * cpy_ptr, long *reg_num, char *label_operand, int *error, int line_number )
{
    int cur_idx=0;
    char cur_char[1]="", temp[82] ="";
    while(!isspace(cpy_ptr[cur_idx]))
    {
	    cur_char[0]=cpy_ptr[cur_idx];
	    strncat(temp,cur_char,1);
	    cur_idx++;
    }
    cur_idx=0;
    if(temp[cur_idx] != '$')
    {
        if(isdigit(temp[cur_idx]))
    	{
            printf("ERROR in file %s at line %d:\n",file_name,line_number);
            printf("Missing '$' sign before register number.\n");
            *error=ERROR;
            return strlen(temp);
    	}
        if(is_label(strncat(temp,":",1) ,instructions, directives, symbol_table,1,line_number)!=1)
        {
			printf("ERROR in file %s at line %d:\n",file_name,line_number);
            printf("Invalid parameter - invalid label.\n");
            *error=ERROR;
            return strlen(temp);
        }
	    strncat(label_operand,temp,strlen(temp)-1);
        return strlen(label_operand);
    }
    else
    {
        cur_idx++;
        if(isdigit(temp[cur_idx]))
        {
    	    char temp_num[82] ="";
    	    
    	    while(isdigit(temp[cur_idx]))
			{
				cur_char[0]=temp[cur_idx];
				strncat(temp_num,cur_char,1);
				cur_idx++;
			}
			if(temp[cur_idx]=='.')
			{
				printf("ERROR in file %s at line %d:\n",file_name,line_number);
			    printf("Invalid parameter - register must be integer.\n");
			    *error=ERROR;
			}
			if(atol(temp_num)>31 || atol(temp_num)<0)
			{
				printf("ERROR in file %s at line %d:\n",file_name,line_number);
			    printf("Invalid parameter - register must be $0-$31.\n");
			    *error=ERROR;
			}
            *reg_num = atol(temp_num);
            return strlen(temp);
        }
		printf("ERROR in file %s at line %d:\n",file_name,line_number);
        printf("Invalid parameter - invalid label.\n");
        *error=ERROR;
    }
    return strlen(temp);
}

/*--------------------------------------------------------------------------------------------
get_label: Check whether there is a label at the start of a given string (cpy_ptr).
           If so, (label_operand) gets the label, else (error) gets the error status. 
           returns the length of the label. (e.g., "example_label" returns 13).
--------------------------------------------------------------------------------------------*/
int get_label(char * cpy_ptr, char *label_operand, int *error, int line_number )
{
    int cur_idx=0;
    char cur_char[1]="", temp[82] ="";
    
    while(!isspace(cpy_ptr[cur_idx]))
    {
	    cur_char[0]=cpy_ptr[cur_idx];
	    strncat(temp,cur_char,1);
	    cur_idx++;
    }
    if(is_label(strncat(temp,":",1) ,instructions, directives, symbol_table,1,line_number)!=1)
    {
		printf("ERROR in file %s at line %d:\n",file_name,line_number);
        printf("Invalid parameter - invalid label.\n");
        *error=ERROR;
    }
    strncat(label_operand,temp,strlen(temp)-1);
    return strlen(label_operand);
}
