/*	
	Kfir Sibirsky	316317221
========================================================================================
	This file contains the main program,
	as well as all user interaction and input analysis activities.
	
	Notes:
	======
	• Instead of calculating factorials and powers again and again 
	  (due to its inefficiency), I used the ratio between two consecutive terms.
	  (A more detailed explanation is listed below)
	• Just to be clear, it is possible to implement a Taylor series using
	  calculations of factorials and powers.
	  In such implementation, each term = (pow(-1,i) * pow(x,2*i+1)) / (fact(2*i+1))
========================================================================================
*/

#include <stdio.h>
#include <math.h>

/* Function prototype */
double abst(double x);
double my_sin(double x);

/*--------------------------------------------------------------------------------------
	main: This function starts the program.
--------------------------------------------------------------------------------------*/
int main()
{
	double x;
	
	/* Quick description. */
	printf ("┌────────────────────────────────────────────────────────────────────────────┐\n");
	printf ("│  Hello and Welcome ☻							     │\n");
	printf ("│  									     │\n");
	printf ("│  This program calculates sin(X) for a given number X using Taylor series,  │\n");
	printf ("│  									     │\n");
	printf ("│  Where X ∈  [-25.0,25.0]. 						     │\n");
	printf ("│  									     │\n");
	printf ("│  Taylor series for calculating sine is defined as:			     │\n");	
	printf ("│  									     │\n");	
	printf ("│  sin(X) = ⅀ (-1)^i * x^(2i+1)/((2i+1)!),from i = 0 to ထ  =                 │\n");
	printf ("│  									     │\n");
	printf ("│  = (X^1)/0! - (x^3)/3! + (x^5)/5! - (x^7)/7! + (x^9)/9! ⋯		     │\n");
	printf ("│  									     │\n");
	printf ("│  To use this program - A number in is needed to be entered.	    	     │\n");
	printf ("└────────────────────────────────────────────────────────────────────────────┘\n");

	printf("\nPlease enter the desired number in radians to calculate it's sine:\n");
	scanf("%lf",&x);
	/* Checks range of input.
	   First, checks whether range is exceeded from above,
	   Second, checks whether range is exceeded from below, otherwise x remains unchanged.  */
	x = (x > 25 ? 25 : (x < -25 ? -25 : x));
	printf("\nEntered number\t\t= %f\nmy_sin(%f)\t= %f\nmath.sin(%f)\t= %f\n",x,x,my_sin(x),x,sin(x));
	
	printf("\n┌──────────────────────────────────────────────────────────────────────────────┐\n");
	printf  ("│\tyou have chosen to stop (or it may have been chosen for you :D)		│\n");
	printf  ("│\tprogram ended. thank you for your time :)					│\n");
	printf  ("└──────────────────────────────────────────────────────────────────────────────┘\n");

return 0; 
}

/*--------------------------------------------------------------------------------------------
	abst: gets a floating point number (x) and returns its absolute value (|x|).
	e.g.,	abst(-1)
	input:	-1	
	output:	 1
--------------------------------------------------------------------------------------------*/
double abst(double x)
{
	return (x < 0 ? x * -1 : x);
}
/*--------------------------------------------------------------------------------------------
	my_sin: gets a floating point number (x) representing amount of degrees in radians,
	and returns the value of sine of x (sin(x)).
	e.g.,	my_sin(1.234567)
	input:	1.234567	
	output:	0.944005
--------------------------------------------------------------------------------------------*/
double my_sin(double x)
{
	int i;
	double sum  = x; /* Minimal sum is only the first term (which is x). */
	double term = x; /* A general term to be added to the sum. */

	 
	/*┌──────────────────────────────────────────────────────────────────────────┐
	  │ Calculating factorials and powers again and again as such:		     │
	  │ ┌──────────────────────────────────────────┐			     │
	  │ │(pow(-1,i) * pow(x,2*i+1)) / (fact(2*i+1))│ is not efficient.	     │
	  │ └──────────────────────────────────────────┘			     │
	  │  Instead, lets divide two consecutive terms to calculate the ratio (R):  │
	  │  R = ((x^5)/5!) 	/ ((-x^3)/3!) 	   = 				     │
	  │    = ((x^5)/5!) 	* (3!/(-x^3)) 	   = 				     │
	  │    = ((x^5)/5*4*3!) * (3!/(-x^3)) 	   = 				     │
	  │    = ((x^5)/5*4) 	* (1/(-x^3)) 	   = -(x^2)/5*4 		     │
	  │    			   	       ┌─────────────────────┐		     │
	  │  So, in general, the ratio (R) is  │ R = -(x*x)/((i+1)*i)│.		     │
	  │  e.g., 3rd term * R = 4th term:    └─────────────────────┘		     │
	  │  	 ((x^5)/5!) 	* -(x*x)/((6+1)*6) = 				     │ 
	  │    = ((x^5 * -x^2)/5!*7*6) 		 				     │
	  │    = ((-x^7)/7!)			 				     │
	  └──────────────────────────────────────────────────────────────────────────┘*/
 
	for(i = 2; abst(term) > 0.000001; i += 2) 
	{
		/* Every next term equals to the previous term times the ratio (R). */
		term *= -(x*x)/((i+1)*i); 
		sum  += term;
	}

	return sum;
}

