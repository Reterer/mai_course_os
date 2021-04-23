// Ряд Лейбница
#include <stdlib.h>
#include <stdbool.h> 
#include "second.h"

float Pi(int K)
{
    float sum = 0;
    int one = -1;
    for(int i = 0; i < K; ++i)
    {
        one *= -1;
        sum += one / (2. * i + 1.);
    }

    return 4 * sum;
}
