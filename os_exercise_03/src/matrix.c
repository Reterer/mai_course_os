#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "matrix.h"
#include "io.h"

Matrix* m_init(int n, int m) {
    Matrix* matrix = malloc(sizeof(Matrix));
    if(matrix == NULL)
        return matrix;
    
    matrix->n = n;
    matrix->m = m;
    matrix->buf = malloc(sizeof(double complex)*n*m);
    
    if(matrix->buf == NULL) {
        free(matrix);
        return NULL;
    }

    {
        int size = n*m;
        for(int i = 0; i < size; i++) {
            matrix->buf[i] = 0;
        }
    }

    return matrix;
}

void m_destroy(Matrix* matrix) {
    if(matrix == NULL)
        return;

    free(matrix->buf);
    free(matrix);
}

double complex *m_get(Matrix* matrix, int i, int j) {
    if(matrix == NULL)
        return NULL;
    if(i < 0 || i >= matrix->n ||
       j < 0 || j >= matrix->m) {
        write_str(STDOUT_FILENO, "выход за границы матрицы! %d %d", i, j);
        return NULL;
    }
    return &matrix->buf[i*matrix->m + j];
}

typedef struct {
    Matrix* A;
    Matrix* B;
    Matrix* C;

    int i_start;
    int i_end;
} m_product_arg;
void* m_product_thread(void* arg) {
    m_product_arg* prod_arg = (m_product_arg*)arg;
    for(int i = prod_arg->i_start; i < prod_arg->i_end; i++) {
        for(int j = 0; j < prod_arg->C->m; j++) {
            for(int k = 0; k < prod_arg->B->n; k++) {
                double complex z1 = prod_arg->A->buf[i * prod_arg->A->m + k];
                double complex z2 = prod_arg->B->buf[k * prod_arg->B->m + j];
                prod_arg->C->buf[i * prod_arg->C->m + j] += z1 * z2;
            }
        }
    }
    return NULL;
}

Matrix* m_product(int max_threads, Matrix* A, Matrix* B) {
    if(A->m != B->n)
        return NULL;
    if(A->n == 0 || B->m == 0)
        return m_init(A->n, B->m);
    Matrix* C = m_init(A->n, B->m);
    /*
    Берем кол-во строчек (A->n + max_threads - 1) / max_threads = Кол-во строчек на поток.
    threads = A->n / Кол-во строчек на поток.
    для каждого потока{
        Определить начало и конец.
        Запустить поток!
    }

    для каждого потока{
        дождаться завершения этого потока
    }
    */
    int row_pre_thread = (A->n + max_threads - 1) / max_threads;
    int threads = (A->n + row_pre_thread - 1) / row_pre_thread;
    m_product_arg thread_args[threads];
    pthread_t thread_id[threads];
    for(int thread = 0; thread < threads; thread++) {
        thread_args[thread].A = A;
        thread_args[thread].B = B;
        thread_args[thread].C = C;
        thread_args[thread].i_start = thread * row_pre_thread;
        
        thread_args[thread].i_end = (thread + 1) * row_pre_thread;
        if(thread_args[thread].i_end > A->n)
            thread_args[thread].i_end = A->n;

        if(pthread_create(&thread_id[thread], NULL, m_product_thread, (void*)&thread_args[thread]) != 0) {
            m_destroy(C);
            return NULL;
        }
    }
    for(int thread = 0; thread < threads; thread++) {
        pthread_join(thread_id[thread], NULL);
    }

    return C;
}