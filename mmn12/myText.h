/*
	Kfir Sibirsky	316317221
=====================================================================================================
	This file contains the inclusion of the needed libraries, definitions for the needed
	constants and linked list structure and also function prototypes.
=====================================================================================================
*/

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

#define MAX_LINE_LENGTH  60
#define ERROR -1
#define BUFFER 1
#define LINKED_LIST 2


/*------------------------------------------------------------------------------------------------------------------
	Defines a node in a linked list.
	Every node in the list contains a fixed MAX_LINE_LENGTH sized array and pointer to the next node.
-------------------------------------------------------------------------------------------------------------------*/
typedef struct link_node {
	char buf[MAX_LINE_LENGTH+1];/* Strings are actually one-dimensional array of characters terminated by a null character '\0' */
	struct link_node *next;
} node;


/*--------------------------------------------------------------------------------------------
printText: gets the method (method) and pointers to buffer and linked list
	   data structures (bf, h), and prints text stored in the corresponding data
	   structure in the following format:
	   • Each row will be printed as such, 
	   
	   1st character  2nd character  3rd character  ⋯  MAX_LINE_LENGTHth character 
	   │┌─────────────┘              │                 │
	   ││┌───────────────────────────┘                 │
	   │││                                             │
	   │││			⋱			      │
	   │││                                             │
	   │││          ┌──────────────────────────────────┘
	   │││          │ 
	   xxx     ⋯    x
	   
	   • There may be more than one row.
--------------------------------------------------------------------------------------------*/
void printText(int method, char * bf, node * h);


/*--------------------------------------------------------------------------------------------
readText: gets the method (method) and pointers to buffer and linked list
	  data structures (bf, h), reads text from the user and stores it in the 
	  corresponding data structure.
	  If an error was detected an error code is returned, otherwise (method) is returned.
--------------------------------------------------------------------------------------------*/
int readText(int method, char * bf, node * h );

