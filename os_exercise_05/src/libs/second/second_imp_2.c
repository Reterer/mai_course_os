// Формула Валлиса
#include <stdlib.h>
#include <stdbool.h> 
#include "second.h"

float Pi(int K)
{
    float sum = 1;
    for(int i = 1; i < K; ++i)
    {
        float a = 4 * i * i;
        sum *= a / (a - 1);
    }

    return 2 * sum;
}
