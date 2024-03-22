#include "matrix_math.h"
#include <random>
#include <cstdlib>
#include <iostream>

double** allocate_matrix(int size){
    double** A;
    
    // Allocate memory for each row
    A = new double*[size];

    for(int i = 0; i < size; i++){
        A[i] = new double[size];
    }

    return A;
}

void fill_matrix(double** A, int size, bool flag){
    double val;

    // Initialie random device
    std::random_device           rd;
    std::mt19937                 eng(rd());
    std::uniform_real_distribution<>     distribution(-1,1);

    // Fill matrix
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            if (flag  == true){
                val = 0;
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

void print_matrix(double** A, int size){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            std::cout << A[i][j] << " ";
        }

        std::cout << std::endl;
    }   
}

void matrix_multiplication(double** A, double** B, double** C, int size){
    double sum;

    // Create a parallel region
    #pragma omp parallel for default(none) shared(A,B,C,size) private(sum)
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            sum = 0;

            for(int k = 0; k < size; k ++){
                sum += A[i][k] * B[k][j];
            }

            C[i][j] = sum;
        }
    } 
}