/*	
	Kfir Sibirsky	316317221
========================================================================================
	This file contains the main program,
	as well as all user interaction and input analysis activities.
========================================================================================
*/

#include "myText.h"


/*--------------------------------------------------------------------------------------
	main: This function starts the program.
--------------------------------------------------------------------------------------*/
int main()
{   
	
	int method;
	int is_error;
	char * buff = NULL;
	node * head = NULL;
	
	/* Quick description. */
	printf ("┌──────────────────────────────────────────────────────────────────────────────┐\n");
	printf ("│\tHello and welcome! ☻ 							│\n");
	printf ("│  										│\n");
	printf ("│\tTo use the program - text (in the form of ASCII characters sequence		│\n");
	printf ("│\tis needed to be entered. The program will divide the text into rows		│\n");
	printf ("│\tcontaining 60 characters, and print the altered text.			│\n");
	printf ("│  										│\n");
	printf ("│\tThere are two methods with which the program works:				│\n");
	printf ("│\t1) A single bufffer that will contain all the text.				│\n");
	printf ("│\t2) A linked list of nodes, with each node being a fixed size bufffer.	│\n");
	printf ("└──────────────────────────────────────────────────────────────────────────────┘\n");


	printf("\nPlease enter the desired method: [1 / 2]\n\n");
	scanf("%d",&method);
	
	if(method == BUFFER)
	{
		/* Allocating memory for the buffer. */
		buff = (char *)calloc(MAX_LINE_LENGTH+1,sizeof(char));
		if(!buff)
		{
			puts("\nError! - Memory allocation for the buffer failed.");
		}
	}
	else if(method == LINKED_LIST)
	{
		/* Allocating memory for the first node in the linked list. */
		head = (node*)calloc(1,sizeof(node));
		if(!head)
		{
			puts("\nError! - Memory allocation for the head of a linked list failed.");
		}
	}
	else
	{
		puts("\nERROR! - Neither method 1 nor 2 had been selected, please try again.");
		exit(0);
	}
	
	/* Getting input from user by calling "readText" function,
	   saving its returned value for later error checking. */
	printf("\nPlease enter some text:\n\n");
	is_error = readText(method, buff, head);
	/* Output the changed text by calling "printText" function. */
	printText(method, buff, head);
	/* Error checking for "readText" function. */
	if(is_error == ERROR)
	{
		puts("\nError! - Memory allocation during \"readText\" function failed.\n");
	}
	/* Free data structure allocated memory. */
	if(buff)
		free(buff);
	if(head)
		free(head);
	printf("\n┌──────────────────────────────────────────────────────────────────────────────┐\n");
	printf  ("│\tyou have chosen to stop (or it may have been chosen for you :D)		│\n");
	printf  ("│\tprogram ended. thank you for your time :)					│\n");
	printf  ("└──────────────────────────────────────────────────────────────────────────────┘\n");

return 0;
}

/*--------------------------------------------------------------------------------------------
readText: gets the method (method) and pointers to buffer and linked list
	  data structures (bf, h), reads text from the user and stores it in the 
	  corresponding data structure.
	  If an error was detected an error code is returned, otherwise (method) is returned.
--------------------------------------------------------------------------------------------*/
int readText(int method, char * bf, node * h )
{
	char * temp, c;
	int i, read_char, cur_len = MAX_LINE_LENGTH;
	node * newNode, * current = h;
	if(method == BUFFER)
	{
		i=1;
		while((read_char = getchar()) != EOF)
		{
			c = (char)read_char;
			/* Skipping spaces. */
			if(c == '\n')
			{
				continue;
			}
			/* The following block of code handles the case in which
			   the end of the current array has been reached. */
			if(i>cur_len)
			{
				/* Reallocates new buffer and increases current length of buffer. */
				temp=(char*)realloc(bf,(cur_len+MAX_LINE_LENGTH)*sizeof(char)+1);
				if(!temp)
				{
					return ERROR;
				}
				bf=temp;
				cur_len += MAX_LINE_LENGTH;
			}
			/* Concatenates the read char to the end of the buffer. */
			strncat(bf, &c, 1);
			i++;
		}
	}
	else if(method == LINKED_LIST)
	{
		i=0;
		while((read_char = getchar()) != EOF)
		{
			/* Skipping spaces. */
			if(read_char=='\n')
			{
				continue;
			}
			/* The following block of code handles the case in which
			   the end of the current node's array has been reached. */
			if(i==MAX_LINE_LENGTH)
			{
				/* Creates, allocates and initializes a new node. */
				newNode = (node*)calloc(1,sizeof(node)); 
				if(!newNode)
				{
					return ERROR;
				}
				newNode->next = NULL;
				current->next=newNode;
				/* Traverse to the next node and resets the index. */
				current=current->next;
				i=0;
			}
			/* Insert the read char to the ith index of the buffer. */
			(current->buf)[i]=read_char;
			i++;
		}
		/* Free temporary allocated memory of helping node. */
		if(newNode)
			free(newNode);
	}
	return method;
}

/*--------------------------------------------------------------------------------------------
printText: gets the method (method) and pointers to buffer and linked list
	   data structures (bf, h), and prints text stored in the corresponding data
	   structure in the following format:
	   • Each row will be printed as such, 
	   
	     1st character  2nd character  3rd character  ⋯  MAX_LINE_LENGTHth character 
	     │┌─────────────┘              │                 │
	     ││┌───────────────────────────┘                 │
	     │││                                             │
	     │││			⋱	  	          │
	     │││                                             │
	     │││          ┌──────────────────────────────────┘
	     │││          │ 
	     xxx     ⋯    x
	   
	   • There may be more than one row.
--------------------------------------------------------------------------------------------*/
void printText(int method, char * bf, node * h)
{
	node * current = h;
	int i, count;
	puts("");
	if(method==BUFFER)
	{
		for(i=0,count=1;i<strlen(bf);i++,count++)
		{
			/* Every MAX_LINE_LENGTH prints, go down one line and reset counter. */
			if(count > MAX_LINE_LENGTH)
			{
				puts("");
				count=1;
			}
			printf("%c",bf[i]);
		}
		return;
	}
	/* If reached here then method must be LINKED_LIST. */
	for(;current;current=current->next)
	{
		puts(current->buf);
	}
}

