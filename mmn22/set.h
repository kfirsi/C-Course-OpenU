/*
Kfir Sibirsky	316317221
=====================================================================================================
This file contains the definition for the types set and set array,
As well as prototypes for the algebra of sets functions.

Notes:
• There are 6 set variable (SETA-SETF), all of which are set to {} = ∅ at the start of the program.
• For ease of use, I created an array of sets and their names as strings.
• I was inspired by program 22 (matrix calculator) in pages (220-225) in the learning guide.
• There are 128 integers in the range [0,127]. Hence, those 128 numbers can be represented by using
  128 bits. Each bit represent one number as follows:
  ┌───┬─────────────────────────────────────┐
  │bit│               meaning	            │
  ├───┼─────────────────────────────────────┤
  │ 0 │ number does not belongs to the set. │ 
  │ 1 │ number belongs to the set.          │
  └───┴─────────────────────────────────────┘
  If an int takes 4 bytes of memory (meaning 4 * 8 = 32 bits)
  Then, 128 / 32 = 4 integers are needed. (or an array of integers of length 4,
  where each element of the array converted to binary represents 32 numbers)
  The placement of each number can be calculated using division and modulo.
  e.g.,	 
	If:     
	 	we were to add the number X to SETA.
	Then:
		X / 32 = i , where i is the array index (SETA->elements[i]).
		X % 32 = k , where k is the bit location inside SETA->elements[i]. 
		
	Let:
		SETA = {} = ∅    and    SETB = { 0, 1, 3, 5, 12, 15, 31, 53, 72,...,79, 88,...,95, 100, 111 }.
		
		If and only if:
		
		SETA->elements[0] = 00000000 00000000 00000000 00000000 base 2
		SETA->elements[1] = 00000000 00000000 00000000 00000000 base 2 
		SETA->elements[2] = 00000000 00000000 00000000 00000000 base 2
		SETA->elements[3] = 00000000 00000000 00000000 00000000 base 2
		
		SETB->elements[0] = 10000000 00000000 10010000 00101011 base 2
		SETB->elements[1] = 00000000 00100000 00000000 00000000 base 2
		SETB->elements[2] = 11111111 00000000 11111111 00000000 base 2
		SETB->elements[3] = 00000000 00000000 10000000 00010000 base 2
=====================================================================================================
*/
#include <stdio.h>
#include <stdlib.h> /* Required for "atoi" function. */

#define ERROR -1
#define LOWER_BOUND 0
#define UPPER_BOUND 127
/* 128 integers in the range [0,127] / 32 bits per int = int array of length 4 = sizeof(int). */
#define CELLS_REQUIRED 4
#define BITS_IN_INT 32

/*--------------------------------------------------------------------------------------------
set: struct that represent a set.
--------------------------------------------------------------------------------------------*/
typedef struct set
{
	int elements[CELLS_REQUIRED];
	int cardinality;
}set;
/*--------------------------------------------------------------------------------------------
set_array: struct that represent an array of sets.
--------------------------------------------------------------------------------------------*/
typedef struct 
{
	char name[5];          
	set *s;
}set_array;

/*-------------------------------------------------------------------------------------------------------------------
read_set: gets the elements of the set.
e.g.,	 
	  Let:
		SETA = {} = ∅
	  If:
		read_set SETA, 1, 2, 3, 4, 5, -1
	  Then:
		SETA = { 1, 2, 3, 4 ,5 }
-------------------------------------------------------------------------------------------------------------------*/
void read_set(set* s, int * vals, int len);

/*------------------------------------------------------------------------------------------------------------------
print_set: prints the elements of a set (s).	
e.g.,
		Let:
			SETA = { 1, 2, 3, 4 ,5 }
		If:
			print_set SETA
		Then:
			input:	SETA	
			output:	1, 2 ,3, 4, 5
-------------------------------------------------------------------------------------------------------------------*/
void print_set(set *s);

/*------------------------------------------------------------------------------------------------------------------
union_set: Performes the union of two sets A and B, and stores the result in set C.
	   	   The result of the union is the set containing all elements which are in A, in B, or in both A and B.
		   mathematical notation:		A ∪ B = { x | x ∈ A or x ∈ B }
e.g.,	 
		Let:
			SETA = { 1, 2, 3, 4 ,5 } and SETB = { 4, 5, 6, 7, 8, 9, 10 } and SETC = {} = ∅
		If:
			union_set SETA, SETB, SETC
		Then:
			SETC = SETA ∪ SETB = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
-------------------------------------------------------------------------------------------------------------------*/
void union_set(set* a, set* b, set * c);

/*------------------------------------------------------------------------------------------------------------------
intersect_set: Performes the intersection of two sets A and B, and stores the result in set C.
		   	   The result of the intersection is the set containing all elements of A that also belong to B.
		   	   mathematical notation:		A ∩ B = { x | x ∈ A and x ∈ B }
e.g.,	 
		Let:
			SETA = { 1, 2, 3, 4 ,5 } and SETB = { 4, 5, 6, 7, 8, 9, 10 } and SETC = {} = ∅
		If:
			intersect_set SETA, SETB, SETC
		Then:
			SETC = SETA ∩ SETB = { 4, 5 }
-------------------------------------------------------------------------------------------------------------------*/
void intersect_set(set* a, set* b, set * c);

/*------------------------------------------------------------------------------------------------------------------
sub_set:  Performs subtraction of set B from set A, and stores the result in set C.
	   	  The result of the subtraction is the set of all the elements that are in set A and are not in set B.
	   	  mathematical notation:		A − B = { x | x ∈ A, x ∉ B }
e.g.,	 
		Let:
			SETA = { 1, 2, 3, 4 ,5 } and SETB = { 4, 5, 6, 7, 8, 9, 10 } and SETC = {} = ∅
		If:
			sub_set SETA, SETB, SETC
		Then:
			SETC = SETA − SETB = { 1, 2, 3 }
-------------------------------------------------------------------------------------------------------------------*/
void sub_set(set* a, set* b, set * c);

/*------------------------------------------------------------------------------------------------------------------
symdiff_set: Calculates the symmetrical difference of set A and set B, and stores the result in set C.
	   	     The symmetrical difference is the set of all the elements that are in set A or set B, 
	   	     but are not in the intersection of sets A and B.
	   	     mathematical notation:	A ∆ B =  (A − B) ∪ (B − A) = { x | x ∈ A, x ∈ B, x ∉ (A ∩ B) }
e.g.,	 
		Let:
			SETA = { 1, 2, 3, 4 ,5 } and SETB = { 4, 5, 6, 7, 8, 9, 10 } and SETC = {} = ∅
	    If:
			symdiff_set SETA, SETB, SETC
		Then:
			SETC = SETA ∆ SETB = { 1, 2, 3, 6, 7, 8, 9, 10 }
-------------------------------------------------------------------------------------------------------------------*/
void symdiff_set(set* a, set* b, set * c);

/*------------------------------------------------------------------------------------------------------------------
set_bit: sets to '1' the Kth bit in a bit array (arr).
e.g.,                                    ┌──────────────────────── Kth bit
		Before:			arr = [0,0,0,...,0,...,0,0,0]             
                                         ┌──────────────────────── Kth bit
		After:			arr = [0,0,0,...,1,...,0,0,0]
-------------------------------------------------------------------------------------------------------------------*/
int set_bit(int arr[], int num2add);

/*------------------------------------------------------------------------------------------------------------------
count_ones: count the number of ones in a binary number.
e.g.,	 
		Let:
			num = 533 base 10 = 0000 0010 0001 0101 base 2
		If:				
			count_ones ( num )
		Then:
			count = 4, as there are 4 ones in the binary version.
-------------------------------------------------------------------------------------------------------------------*/
int count_ones(int num);

/*------------------------------------------------------------------------------------------------------------------
reset_set: sets a given set (s) as the empty set ( {} = ∅ ).
e.g.,	 
		Let:
			SETA = { 1, 2, 3, 4 ,5 }
		If:
			reset_set( SETA )
		Then:
			SETA = {} = ∅
-------------------------------------------------------------------------------------------------------------------*/
void reset_set(set * s);
