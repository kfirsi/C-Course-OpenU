/*
Kfir Sibirsky	316317221
=====================================================================================================
This file contains the implementation of the algebra of sets functions.

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

#include "set.h"


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
void read_set(set * s, int vals[], int len)
{
	int i; 
	for(i=0;i<len && vals[i]!= -1;i++)
	{
	    /* Each number from the list of number to add get sent to "set_bit" function. */
		if(set_bit(s->elements,vals[i])==1)
			s->cardinality++;	
	}
}
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
void print_set(set * s)
{
	int i, printed_in_sequence = 0,found=0, loc=0;     /* Location of the bit inside arr[i]. */
	unsigned int mask = 1;  /* mask = 0000.....00001*/
	if(s->cardinality==0)
	{
		puts("The set is empty");
		return;
	}
	for(i = 0;i<CELLS_REQUIRED;i++)
	{
	    
		while(loc<BITS_IN_INT)
		{

			if(s->elements[i] & mask)
			{
				found++;
				if(printed_in_sequence==16)
				{
					puts("");
					printed_in_sequence=0;
				}
				if(found < s->cardinality)
					printf("%d, ",((i*BITS_IN_INT)+loc));
				else/* Last number in the set to print. */
					printf("%d\n",((i*BITS_IN_INT)+loc));
				printed_in_sequence++;
			}
			mask <<= 1;
			loc++;    
		}
		mask = 1;   /* mask = 0000....00001*/
		loc = 0;	/* . */
	}
}
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
void union_set(set * s1, set * s2, set * s3)
{
	int i;
	if(s1->cardinality == 0 && s2->cardinality == 0)
	{	
		reset_set(s3);
		return;
	}
	if(s1->cardinality == 0 && s2->cardinality > 0)
	{
		for(i=0;i<CELLS_REQUIRED;i++)
			s3->elements[i] = s2->elements[i];
		s3->cardinality = s2->cardinality;
		return;
	}
	if(s1->cardinality > 0 && s2->cardinality == 0)		
	{
		for(i=0;i<CELLS_REQUIRED;i++)
			s3->elements[i] = s1->elements[i];
		s3->cardinality = s1->cardinality;
		return;
	}
	s3->cardinality = 0;
	for(i=0;i<CELLS_REQUIRED;i++)
	{
		s3->elements[i]= s1->elements[i] | s2->elements[i];
		s3->cardinality += count_ones(s3->elements[i]);
	}
}
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
void intersect_set(set * s1, set * s2, set * s3)
{
	int i;
	if(s1->cardinality == 0 || s2->cardinality == 0)
	{	
		reset_set(s3);
		return;
	}
	s3->cardinality = 0;
	for(i=0;i<CELLS_REQUIRED;i++)
	{
		s3->elements[i]= s1->elements[i] & s2->elements[i];
		s3->cardinality += count_ones(s3->elements[i]);
	}
}
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
void sub_set(set * s1, set * s2, set * s3)/* a-b */
{
	int i;
	if(s1->cardinality == 0)
	{	
		reset_set(s3);
		return;
	}
	if(s2->cardinality == 0)
	{
		for(i=0;i<CELLS_REQUIRED;i++)
			s3->elements[i] = s1->elements[i];
		s3->cardinality = s1->cardinality;
		return;
	}
	s3->cardinality = 0;
	for(i=0;i<CELLS_REQUIRED;i++)
	{
		s3->elements[i]= s1->elements[i] & ~s2->elements[i]; /* A-B = A AND NOT B */
		s3->cardinality += count_ones(s3->elements[i]);
	}
}
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
void symdiff_set(set * s1, set * s2, set * s3)
{
	int i;
	s3->cardinality = 0;
	for(i=0;i<CELLS_REQUIRED;i++)
	{
		s3->elements[i]= s1->elements[i] ^ s2->elements[i];
		s3->cardinality += count_ones(s3->elements[i]);
	}
}
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
void reset_set(set * s)
{
	int i;
	for(i=0;i<CELLS_REQUIRED;i++)
		s->elements[i]=0;
	s->cardinality = 0;
}
/*------------------------------------------------------------------------------------------------------------------
set_bit: sets to '1' the Kth bit in a bit array (arr).
e.g.,                                    ┌──────────────────────── Kth bit
		Before:			arr = [0,0,0,...,0,...,0,0,0]             
                                         ┌──────────────────────── Kth bit
		After:			arr = [0,0,0,...,1,...,0,0,0]
-------------------------------------------------------------------------------------------------------------------*/
int set_bit(int arr[], int num_to_add)
{
	int i = num_to_add/BITS_IN_INT, loc = num_to_add % BITS_IN_INT;	/* Location of the bit inside arr[i]. */
	unsigned int mask = 1;					/* mask = 0000.....0001. */
	mask <<= loc;							/* shift left k times and now,  mask = 0000...010...000 */
	
	/* A set is a collection of distinct elements,
	 * Hence, no need to set a member in a set more than once. */
	if(arr[i]==(arr[i] | mask))
	{
		return ERROR;
	}
	/* Bitwise OR to set the bit in the correct location. */
	arr[i] |= mask;      
	return 1;
}
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
int count_ones(int number)
{
	int count = 0;
	int mask = 1;
	int i;
	for (i = 0; i < BITS_IN_INT; i++)
	{
		if ((mask & number) == mask)
		{
			count++;
		}
		mask = mask << 1;
	}
	return count;
}
