#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fib.h"

int main (int argc, const char * argv[]) {
	int n = 0;

/* Check for user input */
	
	if(argc==2) {

/* If input is "-h" print out help line and exit */
	
		if (strcmp( argv[1], "-h") == 0){
		printf("Usage: fibonacci [-h] <non-zero integer>\n");
		return 0;
		}
		else {

/* If there is an input string convert to signed integer */

		n = atoi(argv[1]);
		}
	}	
	
	else {

/* If no input ask user for input value */

   	 printf("Input value for Fibonacci Sequence:");
	scanf("%d", &n);
	}

/* Calls fib() to calculate fibonacci series to the n term */

	n = fib(n);

/*If the fib() routine returns -1 then it's an overflow */

	if ( n == -1 ) {
		printf("Overflow\n");
		return -1;
	}
   	 printf("%d\n\n", n);
    return 0;
}
