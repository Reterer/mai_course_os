#include <stdio.h>      /* Standard Library of Input and Output */
#include <complex.h>    /* Standard Library of Complex Numbers */

void operationOnComplexNumbers() {
    double complex z1 = 1.0 + 3.0 * I;
    double complex z2 = 1.0 - 4.0 * I;

    printf("Working with complex numbers:\n\v");

    printf("Starting values: Z1 = %.2f + %.2fi\tZ2 = %.2f %+.2fi\n", creal(z1), cimag(z1), creal(z2), cimag(z2));

    double complex sum = z1 + z2;
    printf("The sum: Z1 + Z2 = %.2f %+.2fi\n", creal(sum), cimag(sum));

    double complex difference = z1 - z2;
    printf("The difference: Z1 - Z2 = %.2f %+.2fi\n", creal(difference), cimag(difference));

    double complex product = z1 * z2;
    printf("The product: Z1 x Z2 = %.2f %+.2fi\n", creal(product), cimag(product));

    double complex quotient = z1 / z2;
    printf("The quotient: Z1 / Z2 = %.2f %+.2fi\n", creal(quotient), cimag(quotient));

    double complex conjugate = conj(z1);
    printf("The conjugate of Z1 = %.2f %+.2fi\n", creal(conjugate), cimag(conjugate));
}

#include "io.h"
#include <string.h>
void inputComplexNumber() {
    const int size_str = 256;
    char str[size_str];
    int len_str = get_line(str, size_str, '\n');
    // printf("Len str: %d | strlen(): %ld | string: %s\n", len_str, strlen(str), str);
    double real;
    double img;
    sscanf(str, "%lf %lfi", &real, &img);
    printf("%.2lf %+.2lfi\n", real, img);
    double complex z = real + img * I;
    printf("Starting values: Z1 = %.2f %+.2fi\n", creal(z), cimag(z));
}

#include <pthread.h>
void justThread(void* args) {
    
}
void threads() {

}
int main() {
    operationOnComplexNumbers();
    inputComplexNumber();
    threads();
    return 0;
}