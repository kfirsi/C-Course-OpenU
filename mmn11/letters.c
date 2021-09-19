/*	
	Kfir Sibirsky	316317221
========================================================================================
	This file contains the main program,
	as well as all user interaction and input analysis activities.

	Notes:
	• I was inspired by the solution to Question 7.(Learning Guide, page 17)
	• As written in the question description:
	  1st rule - The first letter of every sentence must be in uppercase.
	  2nd rule - Every lowercase letter inside a quote must be in uppercase.
	  3rd rule - Elsewhere in the text (not according to Rules 1-2 above),
	  	     each uppercase letter must be converted to a lowercase letter.
	  4th rule - Digits ('0'-'9') must be skipped, and must not be printed.
	  5th rule - Every character that is not alphabetic letter or a digit,
	  	     must be printed without any changes.
========================================================================================
*/

#include<stdio.h>
#include<ctype.h>

#define IN  1 /* state = IN   ⇔  Inside  quote */
#define OUT 0 /* state = OUT  ⇔  Outside quote */

/*--------------------------------------------------------------------------------------
	main:	This function starts the program.
--------------------------------------------------------------------------------------*/
int main()
{   
	
	int read_char, prev_char = '\0', state = OUT;
	
	/* Quick description */
	printf ("┌──────────────────────────────────────────────────────────────────────────────┐\n");
	printf ("│\tHello and welcome! ☻ 							│\n");
	printf ("│  										│\n");
	printf ("│\tTo use the program - text (in the form of ASCII characters sequence	│\n");
	printf ("│  										│\n");
	printf ("│\tis needed to be entered. the program will make changes to the text	│\n");
	printf ("│  										│\n");
	printf ("│\tand print the altered text.						│\n");
	printf ("└──────────────────────────────────────────────────────────────────────────────┘\n");

	printf("\nPlease enter some text:\n\n");
	
	while((read_char = getchar()) != EOF)
	{
		/* The entered character is a digit (0-9). */
		if(isdigit(read_char))/* Rule number 4. */
		{
			prev_char = read_char;
			continue; 
		}
		/* The entered character is a letter (a-z / A-Z). */
		else if(isalpha(read_char))/* Rules number 1, 2, 3. */
		{
			/* Rules number 1, 2. */
			if(prev_char == '\0' || prev_char == '.' || state == IN)/* First condition is for the very first letter in the text. */
			{
				putchar(toupper(read_char));
			}
			else/* Rule number 3. */
			{
				putchar(tolower(read_char));			
			}
		}
		/* The entered character is neither a digit nor a letter. */
		else/* Rule number 5. */
		{	
			/* Check state of quotation marks. */
			if(read_char == '"' && state == OUT)
			{
				state = IN;/* Change the state to inside quote. */
			}
			else if(read_char == '"' && state == IN)
			{
				state = OUT;/* Change the state to outside quote. */
			}
			putchar(read_char);
		}
		if(!isspace(read_char))/* Holding on to the last character that is not a white space. */
		{
			prev_char = read_char;
		}
	}
	
	printf("\n┌──────────────────────────────────────────────────────────────────────────────┐\n");
	printf  ("│\tyou have chosen to stop (or it may have been chosen for you :D)		│\n");
	printf  ("│\tprogram ended. thank you for your time :)					│\n");
	printf  ("└──────────────────────────────────────────────────────────────────────────────┘\n");

return 0;
}
