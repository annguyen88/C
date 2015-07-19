//
//  main.c
//  cubeFunction
//
//  Created by Andrew Nguyen on 6/22/15.
//  Copyright (c) 2015 Andrew Nguyen. All rights reserved.
//

#include <stdio.h>

int cube(int a, int b, int c);


int main(int argc, const char * argv[]) {
  
    // insert code here...
   int num1, num2, num3;

    printf("Please enter First number: \n");
    scanf("%d", &num1);
    
    printf("Please enter Second number: \n");
    scanf("%d", &num2);
    
    printf("Please enter Third number: \n");
    scanf("%d", &num3);

    printf("%d", cube(num1, num2, num3));
    return 0;
}

int cube(int a, int b, int c)
{
    printf("Summing and cubing the inputted numbers\n");
    int sum = a + b + c;
    return(sum*sum*sum);
}
