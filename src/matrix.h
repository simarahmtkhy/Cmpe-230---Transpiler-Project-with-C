#include <stdlib.h>
#include <stdio.h>

typedef struct Matrix Matrix;

struct Matrix
{
    int matrix_i;
    int matrix_j;
    float **arr;
};

void create_matrix(Matrix *m, int i, int j)
{
    m->matrix_i = i;
    m->matrix_j = j;
    m->arr = (float **)malloc(i * sizeof(float *));
    for (int i = 0; i < i; i++)
    {
        m->arr[i] = (float *)malloc(j * sizeof(float));
    }
    for (int k; k < i; k++)
    {
        for (int n; n < j; n++)
        {
            m->arr[k][n] = 0;
        }
    }
}

Matrix multiply(Matrix *m1, Matrix *m2)
{
    Matrix multiplication;
    create_matrix(&multiplication, m1->matrix_i, m2->matrix_j);
    for (int i = 0; i < multiplication.matrix_i; i++)
    {
        for (int j = 0; j < multiplication.matrix_j; j++)
        {
            for (int k = 0; k < m2->matrix_i; k++)
            {
                multiplication.arr[i][j] += m1->arr[i][k] * m2->arr[k][j];
            }
        }
    }
    return multiplication;
}

Matrix add(Matrix *m1, Matrix *m2)
{
    Matrix sum;
    create_matrix(&sum, m1->matrix_i, m1->matrix_j);
    for (int i = 0; i < m1->matrix_i; i++)
    {
        for (int j = 0; m1->matrix_j; j++)
        {
            sum.arr[i][j] = m1->arr[i][j] + m2->arr[i][j];
        }
    }
    return sum;
}

Matrix subtraction(Matrix *m1, Matrix *m2)
{
    Matrix difference;
    create_matrix(&difference, m1->matrix_i, m1->matrix_j);
    for (int i = 0; i < m1->matrix_i; i++)
    {
        for (int j = 0; m1->matrix_j; j++)
        {
            difference.arr[i][j] = m1->arr[i][j] + m2->arr[i][j];
        }
    }
    return difference;
}

Matrix transpose(Matrix *m)
{
    Matrix transpose;
    create_matrix(&transpose, m->matrix_j, m->matrix_i);
    for (int i = 0; i < m->matrix_i; i++)
    {
        for (int j = 0; j < m->matrix_j; j++)
        {
            transpose.arr[j][i] = m->arr[i][j];
        }
    }
    return transpose;
}