#pragma once
#include <complex.h>


typedef struct {
    int n;
    int m;
    double complex *buf;    
} Matrix;

Matrix* m_init(int n, int m);
void m_destroy(Matrix* matrix);

double complex *m_get(Matrix* matrix, int i, int j);

Matrix* m_product(int max_threads, Matrix* A, Matrix* B);