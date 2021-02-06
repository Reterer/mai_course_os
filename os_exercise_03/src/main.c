#include <time.h>
#include <stdio.h>
#include <complex.h>

#include "io.h"
#include "matrix.h"


Matrix *readMatrix() {
    const int STR_SIZE = 256;
    int n;
    int m;
    char str[STR_SIZE];
    get_line(str, STR_SIZE, '\n');

    sscanf(str, "%d %d", &n, &m);
    Matrix *matrix = m_init(n, m);

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            double real;
            double img;
            
            get_line(str, STR_SIZE, ' ');
            sscanf(str, "%lf", &real);

            get_line(str, STR_SIZE, 'i');
            sscanf(str, "%lfi", &img);   
            get_line(str, STR_SIZE, (j + 1 < m) ? ' ' : '\n');         

            complex double *el = m_get(matrix, i, j);
            *el = real + img * I;
        }
    }

    return matrix;
}

void printMatrix(Matrix* matrix) {
    for(int i = 0; i < matrix->n; i++) {
        for(int j = 0; j < matrix->m; j++) {
            complex double z =  *m_get(matrix, i ,j);
            write_str(STDOUT_FILENO, "%.2lf %+.2lfi\t", creal(z), cimag(z));
        }
        write_str(STDOUT_FILENO, "\n");
    }
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        // USAGE;
        return 1;
    }
    int max_threads;
    if(sscanf(argv[1], "%d", &max_threads) != 1) {
        // ERROR INPUT
    }

    Matrix *matrixA = readMatrix();
    Matrix *matrixB = readMatrix();
    // struct timespec t_start, t_end;
    // clock_gettime (CLOCK_REALTIME, &t_start);

    Matrix *matrixC = m_product(max_threads, matrixA, matrixB);
    
    // clock_gettime (CLOCK_REALTIME, &t_end);
    // write_str(STDERR_FILENO, "Вермени заняло: %lf\n", 
    //     (double)(t_end.tv_nsec - t_start.tv_nsec) / 1000000000 + (double)(t_end.tv_sec - t_start.tv_sec));
    
    printMatrix(matrixC);
    
    m_destroy(matrixC);
    m_destroy(matrixA);
    m_destroy(matrixB);
}
