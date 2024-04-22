#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H


double** allocate_matrix(int m, int n=-1);
void fill_matrix(double** A, int m, bool flag=false, int n=-1);
void deallocate_matrix(double** &, int);
void print_matrix(double** A, int m, int n=-1);
void matrix_multiplication(double**, double**,double**, int n, int m=-1);
//void matrix_multiplication_hybrid(double**, double**,double**, int);
void flatten(double** matrix, double* matrix_flat, int m, int n=-1);
void unflatten(double* matrix_flat, double** matrix, int m, int n=-1);
#endif