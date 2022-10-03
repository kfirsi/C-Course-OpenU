/*
Kfir Sibirsky	316317221
Eyal Haimov     316316118
=====================================================================================================
This file contains the main program, as well as all file handling.
(If a file passed as command line argument is valid, Then output files are created (.ext, .ent, .ob))
-----------------------------------------------------------------------------------------------------
Included files:
---------------
• "utils.h" - Required data structures, definitions and function prototypes. 
• "assembler.h" - Required function prototypes. 
=====================================================================================================
*/
#include "utils.h"
#include "assembler.h"
/*--------------------------------------------------------------------------------------
	main: This function starts the program.
--------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{    
    FILE *fptr;
    int file_number, both_passes;
    for(file_number = 1; file_number < argc; file_number++)
    {
        /* Allocate memory for the file's name  including the ".as" ending, to it */
        file_name = (char *)calloc(1,strlen(argv[file_number]) + strlen(".as"));
        if(!file_name)
        {
			printf("ERROR! Memory allocation for the %d%s file name failed.",file_number,eng_ordinal_nums(file_number));
        }
        strcpy(file_name, argv[file_number]);
        strcat(file_name,".as");
        /* Opens the file, if it exists, for reading */
        fptr = fopen(file_name, "r");
        if(!fptr)
        {
			printf("ERROR! Opening the %d%s file (%s) failed.",file_number,eng_ordinal_nums(file_number),file_name);
            continue;
        }
        both_passes = 0;
        if(!first_pass(fptr) && !second_pass(fptr))
        {
		    write_output_files(argv[file_number]);
		    both_passes = 1;
        }
        /* Free all allocated memory for linked lists. */
		free_lists(both_passes);
        /* Closes the file after reading and frees the file_name string for the next file name */
        fclose(fptr); 
        free(file_name);
    }
	return 0;
}
/*--------------------------------------------------------------------------------------------
eng_ordinal_nums: Returns the proper english ordinal numeration suffix for a given number (n).
--------------------------------------------------------------------------------------------*/
char * eng_ordinal_nums(int n)
{
	if(((n/10)%10)==1)
		return "th";
	switch(n%10)
	{
		case 1: 
			return "st";
		case 2: 
			return "nd";
		case 3: 
			return "rd";
		default:
			return "th";
	}
	return "";
}
/*--------------------------------------------------------------------------------------------
write_output_files: Open files to write the 3 output files (ob, ext, ent) with the same name
                    (name) as the current file being analyzed.
--------------------------------------------------------------------------------------------*/
void write_output_files(char *name)
{
    FILE *fptr;
    char *cur_file_name = (char *)calloc(1,strlen(name) + strlen(".ob"));
    if(!cur_file_name)  /*Allocates memory for the file's name and copies the name and the ending into it */
        printf("memory allocation failed");
    strcpy(cur_file_name, name);
    strcat(cur_file_name, ".ob");
    fptr = fopen(cur_file_name, "w"); /* Opens the file, with writing permission */
    write_ob_file(fptr);
    
    cur_file_name = (char *)calloc(1,strlen(name) + strlen(".ent"));
    if(!cur_file_name)  /*Allocates memory for the file's name and copies the name and the ending into it */
        printf("memory allocation failed");
    strcpy(cur_file_name, name);
    strcat(cur_file_name, ".ent");
    fptr = fopen(cur_file_name, "w"); /* Opens the file, with writing permission */
	write_ent_file(fptr);
    
    cur_file_name = (char *)calloc(1,strlen(name) + strlen(".ext"));
    if(!cur_file_name)  /*Allocates memory for the file's name and copies the name and the ending into it */
        printf("memory allocation failed");
    strcpy(cur_file_name, name);
    strcat(cur_file_name, ".ext");
    fptr = fopen(cur_file_name, "w"); /* Opens the file, with writing permission */
	write_ext_file(fptr);
    free(cur_file_name);  /* The file's name isn't needed anymore */

}
/*--------------------------------------------------------------------------------------------
write_ext_file: Write to the output file (f) the list places (addresses) in the machine code
                in which a symbol that is declared as an external is used.
                (all symbols that appeared as an operand of .external,
                And is characterized in the symbol table as "external") 
--------------------------------------------------------------------------------------------*/
void write_ext_file(FILE * file_to_write_in)
{
    ext *e = external_list->head;
	while(e)
	{
		fprintf(file_to_write_in,"%s %04d\n",e->name,e->address);
		e=e->next;
	}
	fclose(file_to_write_in);
}
/*--------------------------------------------------------------------------------------------
write_ent_file: Write to the output file (f) the list of symbols which declared as an entry
                point (all symbols that appeared as an operand of .entry,
                And is characterized in the symbol table as "data, entry" or "code, entry") 
--------------------------------------------------------------------------------------------*/
void write_ent_file(FILE * file_to_write_in)
{
    symbol *sptr = symbol_table->head;
	while(sptr)
	{
		if(sptr->type == DATA_AND_ENTRY || sptr->type == CODE_AND_ENTRY)
		{
			fprintf(file_to_write_in,"%s %04d\n",sptr->name,sptr->address);
		}
		sptr=sptr->next;
	}
	fclose(file_to_write_in);
}
/*--------------------------------------------------------------------------------------------
write_ob_file: Write to the output file (f) the machine code.
--------------------------------------------------------------------------------------------*/
void write_ob_file(FILE * file_to_write_in)
{
    instruction_node *in = code_image->head;
    directive_node *dn = data_image->head;
    char num_bin[32]="",num_hex[32]="",*pair_hex;
    int pairs_in_line=0,i=0,expected_address=0, parsing  = 0, cur;
    /* print length of code image and length of data image. */
    fprintf(file_to_write_in,"     %d %d\n",ICF-IC_START_ADDR,DCF);
    /* print code image. */
    for(cur=0;cur<(code_image->count);cur++)
    {
        if(in->R != NULL)
            int_to_bin_str(*(int*)in->R,32,num_bin);
        else if(in->I != NULL)
            int_to_bin_str(*(int*)in->I,32,num_bin);
        else if(in->J != NULL)
            int_to_bin_str(*(int*)in->J,32,num_bin);
        bin_str_to_hex_str(num_bin,num_hex);
        fprintf(file_to_write_in,"%04d %s\n",in->address,num_hex);
        for(i=0;i<32;i++)
        {
        	num_bin[i]='\0';
        	num_hex[i]='\0';
        }
        in = in->next;
    }
  	/* print data image. */
	expected_address=dn->address;
    for(cur=0;cur<(data_image->count);cur++)
    {
        if(dn->_8_bit != NULL)
            int_to_bin_str(*(int*)dn->_8_bit,8,num_bin);
        else if(dn->_16_bit != NULL)
            int_to_bin_str(*(int*)dn->_16_bit,16,num_bin);
        else if(dn->_32_bit != NULL)
            int_to_bin_str(*(int*)dn->_32_bit,32,num_bin);
    	bin_str_to_hex_str(num_bin,num_hex);
        if(pairs_in_line == 0)
        {
			fprintf(file_to_write_in,"%04d ",expected_address);
        }
        /* get the first token */
        pair_hex = strtok(num_hex, " ");
        /* walk through other tokens */
        while(pair_hex != NULL)
        {
		
            if(pairs_in_line<4)
            {
            	if(parsing == 1)
				{
					fprintf(file_to_write_in,"%04d ",expected_address);
				}
                fprintf(file_to_write_in,"%s ",pair_hex);
                pairs_in_line++;
                expected_address++;
            }
            if(pairs_in_line==4)
            {
                pairs_in_line=0;
				parsing = 1;
                fprintf(file_to_write_in,"\n");
            }
            pair_hex = strtok(NULL, " ");
        }
        for(i=0;i<32;i++)
        {
        	num_bin[i]='\0';
        	num_hex[i]='\0';
        }
        parsing  = 0;
        dn = dn->next;
    }
	fclose(file_to_write_in);
}
/*--------------------------------------------------------------------------------------------
free_lists: Free all allocated memory for linked lists according to the status of the program.
            (both_passes = 1 means both passes successfully completed and the external list
            need to freed also).
--------------------------------------------------------------------------------------------*/
void free_lists(int both_passes)
{

	symbol *cur_symbol_node = symbol_table->head, *temp_symbol_node;
	instruction_node *cur_code_node = code_image->head, *temp_code_node;
	directive_node *cur_data_node = data_image->head, *temp_data_node;
	ext *cur_ext_node, *temp_ext_node;
	int cur;
    for(cur=0;cur<(symbol_table->count);cur++)
	{
	    /*saving the next element in temp*/
		temp_symbol_node=cur_symbol_node->next;
		/*releasing the memory of the current Element*/
		free(cur_symbol_node);
		/*moving to the next element to release the memory*/
		cur_symbol_node=temp_symbol_node;
		symbol_table->head=cur_symbol_node;
	}
    if(symbol_table!=NULL)
    {	
    	free(symbol_table);
    	symbol_table=NULL;
    }
    for(cur=0;cur<(code_image->count);cur++)
	{
	    
	    /*saving the next element in temp*/
		temp_code_node=cur_code_node->next;
		/*releasing the memory of the current Element type node*/
		if(cur_code_node->R != NULL)
		{
		    free(cur_code_node->R);
            cur_code_node->R=NULL;
		}
		else if(cur_code_node->I != NULL)
		{
		    free(cur_code_node->I);
            cur_code_node->I=NULL;
		}
		else if(cur_code_node->J != NULL)
		{
		    free(cur_code_node->J);
            cur_code_node->J=NULL;
		}
		/*releasing the memory of the current Element*/
		free(cur_code_node);
		/*moving to the next element to release the memory*/
		cur_code_node=temp_code_node;
		code_image->head=cur_code_node;
	}
    if(code_image!=NULL)
    {	
		free(code_image);
    	code_image=NULL;
    }
    
    for(cur=0;cur<(data_image->count);cur++)
	{
	    
	    /*saving the next element in temp*/
		temp_data_node=cur_data_node->next;
		/*releasing the memory of the current Element type node*/
		if(cur_data_node->_8_bit != NULL)
		{
		    free(cur_data_node->_8_bit);
            cur_data_node->_8_bit=NULL;
		}
		else if(cur_data_node->_16_bit != NULL)
		{
		    free(cur_data_node->_16_bit);
            cur_data_node->_16_bit=NULL;
		}
		else if(cur_data_node->_32_bit != NULL)
		{
		    free(cur_data_node->_32_bit);
            cur_data_node->_32_bit=NULL;
		}
		/*releasing the memory of the current Element*/
		free(cur_data_node);
		/*moving to the next element to release the memory*/
		cur_data_node=temp_data_node;
		data_image->head=cur_data_node;
	}
    if(data_image!=NULL)
    {	
		free(data_image);
    	data_image=NULL;
    }
	if(both_passes == 1)    
	{
		cur_ext_node = external_list->head;
		for(cur=0;cur<(external_list->count);cur++)
		{
			/*saving the next element in temp*/
			temp_ext_node=cur_ext_node->next;
			/*releasing the memory of the current Element*/
			free(cur_ext_node);
			/*moving to the next element to release the memory*/
			cur_ext_node=temp_ext_node;
			external_list->head=cur_ext_node;
		}
		if(external_list!=NULL)
		{	
			free(external_list);
			external_list=NULL;
		}
	}
}
