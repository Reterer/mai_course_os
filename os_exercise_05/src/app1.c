#include <stdio.h>
#include <stdlib.h>
#include "libs/first/first.h"
#include "libs/second/second.h"

int main()
{
    char mod;

    while(scanf("%c", &mod) == 1) 
    {
        // Derivative
        if(mod == '1')
        {
            float A, deltaX;
            if(scanf("%f%f", &A, &deltaX) != 2)
            {
                fprintf(stderr, "Ошибка ввода\n");
                continue;
            }
            printf("Derivative: %f\n", Derivative(A, deltaX));
        }
        //Pi
        else if(mod == '2')
        {
            int K;
            if(scanf("%d", &K) != 1)
            {
                fprintf(stderr, "Ошибка ввода\n");
                continue;
            } 
            if(K < 1)
            {
                fprintf(stderr, "Введено некорректное значение\n");
                continue;
            }

            printf("Pi: %f\n", Pi(K));
        }
    }

    return 0;
}

