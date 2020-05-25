/**
 * @file
 * Compute eigen values and eigen vectors of a matrix using QR decomposition method.
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "qr_decompose.h"
#include <function_timer.h>

#define LIMS 9

/**
 * create a square matrix of given size with random elements
 */
void create_matrix(double **A, /**< matrix to create (must be pre-allocated in memory) */
                   int N       /**< size of matrix to create */
)
{
    int i, j, tmp, lim2 = LIMS >> 1;
    srand(time(NULL));

    for (i = 0; i < N; i++)
    {
        A[i][i] = (rand() % LIMS) - lim2;
        for (j = i + 1; j < N; j++)
        {
            tmp = (rand() % LIMS) - lim2;
            A[i][j] = tmp;
            A[j][i] = tmp;
        }
    }
}

/**
 * Perform multiplication of two matrices.
 * * R2 must be equal to C1
 * * Resultant matrix size should be R1xC2
 * \returns pointer to resultant matrix
 */
double **mat_mul(double **A,   /**< first matrix to multiply  */
                 double **B,   /**< second matrix to multiply */
                 double **OUT, /**< output matrix (must be pre-allocated) */
                 int R1,       /**< number of rows of first matrix */
                 int C1,       /**< number of columns of first matrix */
                 int R2,       /**< number of rows of second matrix */
                 int C2        /**< number of columns of second matrix */
)
{
    if (C1 != R2)
    {
        perror("Matrix dimensions mismatch!");
        return OUT;
    }
    for (int i = 0; i < R1; i++)
        for (int j = 0; j < C2; j++)
        {
            OUT[i][j] = 0.f;
            for (int k = 0; k < C1; k++)
                OUT[i][j] += A[i][k] * B[k][j];
        }
    return OUT;
}

/**
 * main function
 */
int main(int argc, char **argv)
{
    int mat_size = 5;
    if (argc == 2)
        mat_size = atoi(argv[1]);

    if (mat_size < 2)
    {
        fprintf(stderr, "Matrix size should be > 2\n");
        return -1;
    }

    int i, rows = mat_size, columns = mat_size;

    double **A = (double **)malloc(sizeof(double) * mat_size);
    double **R = (double **)malloc(sizeof(double) * mat_size);
    double **Q = (double **)malloc(sizeof(double) * mat_size);
    double *eigen_vals = (double *)malloc(sizeof(double) * mat_size);
    if (!Q || !R || !eigen_vals)
    {
        perror("Unable to allocate memory for Q & R!");
        return -1;
    }
    for (i = 0; i < mat_size; i++)
    {
        A[i] = (double *)malloc(sizeof(double) * mat_size);
        R[i] = (double *)malloc(sizeof(double) * mat_size);
        Q[i] = (double *)malloc(sizeof(double) * mat_size);
        if (!Q[i] || !R[i])
        {
            perror("Unable to allocate memory for Q & R.");
            return -1;
        }
    }

    /* create a random matrix */
    create_matrix(A, mat_size);

    print_matrix(A, mat_size, mat_size);

    int counter = 0, num_eigs = rows - 1;
    double last_eig = 0;

    function_timer *t1 = new_timer();
    start_timer(t1);
    while (num_eigs > 0)
    {
        while (fabs(A[num_eigs][num_eigs - 1]) > 1e-10)
        {
            last_eig = A[num_eigs][num_eigs];
            for (int i = 0; i < rows; i++)
                A[i][i] -= last_eig; /* A - cI */
            qr_decompose(A, Q, R, rows, columns);

#if defined(DEBUG) || !defined(NDEBUG)
            print_matrix(A, rows, columns);
            print_matrix(Q, rows, columns);
            print_matrix(R, columns, columns);
            printf("-------------------- %d ---------------------\n", ++counter);
#endif
            mat_mul(R, Q, A, columns, columns, rows, columns);
            for (int i = 0; i < rows; i++)
                A[i][i] += last_eig; /* A + cI */
        }

        eigen_vals[num_eigs] = A[num_eigs][num_eigs];
#if defined(DEBUG) || !defined(NDEBUG)
        printf("========================\n");
        printf("Eigen value: % g,\n", last_eig);
        printf("========================\n");
#endif
        num_eigs--;
        rows--;
        columns--;
    }
    eigen_vals[0] = A[0][0];
    double dtime = end_timer_delete(t1);

#if defined(DEBUG) || !defined(NDEBUG)
    print_matrix(R, mat_size, mat_size);
    print_matrix(Q, mat_size, mat_size);
#endif
    printf("Eigen vals: ");
    for (i = 0; i < mat_size; i++)
        printf("% 9.4g\t", eigen_vals[i]);
    printf("\nTime taken to compute: % .4g sec\n", dtime);

    for (int i = 0; i < mat_size; i++)
    {
        free(A[i]);
        free(R[i]);
        free(Q[i]);
    }
    free(A);
    free(R);
    free(Q);
    free(eigen_vals);
    return 0;
}