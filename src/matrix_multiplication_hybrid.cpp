#include <iostream>
#include <fstream>
#include <string> 
#include <random>
#include <chrono>
#include <cstdlib>
#include "matrix_math.h"

using namespace std;

int main(int argc, char** argv){
    // Declare variables
    int SIZE;
    string FILE_NAME;
    string file_matrix_A;
    string file_matrix_B;
    fstream output_file;
    const char* NUM_THREADS;

    // Validate inputs 
    if (argc != 5){
        cout << "Invalid number of input arguments" << endl;
        cout << "USAGE: matrix_multiplication {size} {A matrix file} {B matrix file} {output file}" << endl;
        return -1;
    } else{
        // Store matrix size
        SIZE = atoi(argv[1]);

        // Store file name
        file_matrix_A = argv[2];
        file_matrix_B = argv[3];
        FILE_NAME = argv[4];

        // Determine number of threads
        NUM_THREADS = getenv("OMP_NUM_THREADS");

        cout << "Threads: " << NUM_THREADS << endl;
        cout << "Matrix size: " << SIZE << "x" << SIZE << endl;
        cout << "Output file name: " << FILE_NAME << endl;
    }
    

    // Allocate matrices
    double** A = allocate_matrix(SIZE);
    double** B = allocate_matrix(SIZE);
    double** C = allocate_matrix(SIZE);

    // Fill matrices
    //fill_matrix(A,SIZE);
    load_matrix(A,SIZE,file_matrix_A);
    //fill_matrix(B,SIZE);
    load_matrix(B,SIZE,file_matrix_B);

    // Open file stream
    output_file.open(FILE_NAME, ios::out | ios::app);

    // Time matrix multiplication
    for (int i = 0; i < 10; i++){
        // Initialize result matrix with zeros
        fill_matrix(C,SIZE,true);

        // Perform matrix multiplication
        auto start = chrono::system_clock::now();
        matrix_multiplication(A,B,C,SIZE);
        auto end = chrono::system_clock::now();
        chrono::duration<double> time = (end - start);

        // Write to file
        output_file << NUM_THREADS << ","  << SIZE << "," << time.count() << endl;
    }

    // Close file_stream
    output_file.close();

    // Deallocate matrices
    deallocate_matrix(A,SIZE);
    deallocate_matrix(B,SIZE);
    deallocate_matrix(C,SIZE);

    return 0;
}
