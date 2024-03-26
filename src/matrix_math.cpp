#include "matrix_math.h"
#include <random>
#include <iostream>
#include <fstream>
#include <mpi.h>

double** allocate_matrix(int m, int n){
    if (n == -1){
        n = m;
    }

    double** A;
    
    // Allocate memory for each row
    A = new double*[m];

    for(int i = 0; i < m; i++){
        A[i] = new double[n];
    }

    return A;
}

void fill_matrix(double** A, int m, bool flag, int n){
    if(n == -1){
        n = m;
    }
    double val;

    // Initialie random device
    std::random_device           rd;
    std::mt19937                 eng(rd());
    std::uniform_real_distribution<>     distribution(-1,1);

    // Fill matrix
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            if (flag  == true){
                val = 1;
            }
            
            A[i][j] = val;
        }
    }
}

void deallocate_matrix(double** &A, int size){
    for(int i = 0; i < size; i++){
        delete[] A[i];
    }

    delete[] A;
}

void print_matrix(double** A, int m,int n){
    if (n == -1){
        n = m; 
    }

    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            std::cout << A[i][j] << " ";
        }

        std::cout << std::endl;
    }   

    std::cout << std::endl;
}

void matrix_multiplication(double** A, double** B, double** C, int n, int m){
    double sum;
    if (m == -1){
        m = n;
    }

    // Create a parallel region
    #pragma omp parallel for default(none) shared(A,B,C,m,n) private(sum)
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            sum = 0;
            for(int k = 0; k < n; k ++){
                sum += A[i][k] * B[k][j];
            }

            C[i][j] = sum;
        }
    } 
}

void flatten(double** matrix, double* matrix_flat, int m, int n){
    int idx;
    if(n == -1){
        n = m;
    }

    for (int i = 0; i < m; i++){
        for (int j = 0; j < n; j++){
            // Calculate linear index
            idx = j + i * n;

            matrix_flat[idx] = matrix[i][j];
        }
    }
}

void unflatten(double* matrix_flat, double** matrix, int m, int n){
    int i, j, num_elements;
    if(n == -1){
        n = m;
    }
    
    // Determine number of elements
    num_elements = n * m;

    for (int idx = 0; idx < num_elements; idx++){
        // Calculate indices
        i = idx / n;
        j = idx % n;

        matrix[i][j] = matrix_flat[idx];
    }
}