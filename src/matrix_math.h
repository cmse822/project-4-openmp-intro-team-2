#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H

double** allocate_matrix(int);
void fill_matrix(double** A, int size, bool flag=false);
void deallocate_matrix(double** &, int);
void print_matrix(double** A, int);
void matrix_multiplication(double**, double**,double**, int);
#endif