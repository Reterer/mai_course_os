#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>


void* load_function(void* lib, const char* name)
{
    char* error;
    void* fun;
    fun = dlsym(lib, name);
    if((error = dlerror()) != NULL)
    {
        fprintf(stderr, "Произошла ошибка загрузки функции: %s\n", error);
        exit(1);
    }

    return fun;
}

int main()
{
    char mod;

    void* libs[2];
    float (*Derivative)(float A, float deltaX);
    float (*Pi)(int K);
    
    if((libs[0] = dlopen("libs/libimp1.so", RTLD_LAZY)) == NULL)
    {
        fprintf(stderr, "Не удалось загрузить библиотеку libs/libimp1.so\n");
        exit(1);
    }
    if((libs[1] = dlopen("libs/libimp2.so", RTLD_LAZY)) == NULL)
    {
        fprintf(stderr, "Не удалось загрузить библиотеку libs/libimp2.so\n");
        exit(1);
    }

    Derivative = load_function(libs[0], "Derivative");
    Pi = load_function(libs[0], "Pi");
    printf("Используется первая реализация\n");

    while(scanf("%c", &mod) == 1) 
    {
        // Change imp
        if(mod == '0')
        {
            int number;
            if(scanf("%d", &number) != 1)
            {
                fprintf(stderr, "Ошибка ввода\n");
                continue;
            }
            if(number < 0 || number > 1)
            {
                fprintf(stderr, "Недопустимое значение\n");
                continue;
            }
            Derivative = load_function(libs[number], "Derivative");
            Pi = load_function(libs[number], "Pi");
        }

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

    dlclose(libs[0]);
    dlclose(libs[1]);
    return 0;
}

