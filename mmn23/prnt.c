/*
Kfir Sibirsky	316317221
=====================================================
This file contains the main program,
in which this file is printed to the standard output.
=====================================================
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*---------------------------------------------------
	main:	This function starts the program.
---------------------------------------------------*/
int main(int argc, char *argv[])
{
    char * path;
    char read_char;
    FILE* fptr;
    /* Allocating the required amount of memeory for the path. */
    path = (char *)malloc(strlen(argv[0]) + strlen(".c"));
    /* Check if memory allocated correctly. */
    if(!path)
    {
        puts("Error! memory allocation failed.");
        exit(0);
    }
    /* Copy the "./" and the following file's name from argv[0]. */
    strcpy(path, argv[0]);
    /* Add the ".c" extension. */
    strcat(path,".c");
    /* Open the file at path for read. */
    fptr = fopen(path, "r");
    /* Check if file opened correctly. */
    if (!fptr)
    {
        printf("Error! cannot open for read the file in the path %s", path);
        exit(0);
    }
    /* Since fgetc advances the position indicator to the next character,
     * All that is left to do is to print the character as is 
     * and check for end of file. */
    while(!feof(fptr) && (read_char = fgetc(fptr)) != EOF)
    {
        printf("%c", read_char);
    }
    /* Close the file. */
    fclose(fptr);
    /* Free allocated memory. */
    free(path);
    puts("───────────────────────────────────────");
    puts("You have chosen to quit");
    puts("(or it may have been chosen for you :D)");
    puts("Have a nice day :)");
    puts("───────────────────────────────────────");
    return 0;
}
