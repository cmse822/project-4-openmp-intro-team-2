#include <iostream>
#include <fstream>
#include <string> 
#include <chrono>
#include <cstdlib>
#include "matrix_math.h"
#include <sstream>
#include <random>
#include <iomanip>
#include <vector>

using namespace std;



// Function to write a matrix to a CSV file
void writeMatrixToCSV(double** matrix, int rows, int cols, const std::string& filename, int precision) {
    std::ofstream file(filename);
    
    // Check if the file stream is open
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    
    // Set the precision for floating-point numbers
    file << std::fixed << std::setprecision(precision);
    
    // Iterate through the matrix and write to file
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            file << matrix[i][j];
            if (j < cols - 1) file << ","; // No comma after the last element
        }
        file << "\n"; // New line after each row
    }
    
    file.close();
}




int main(int argc, char** argv){
    // Declare variables
    int SIZE;
    int Iter;
    string FILE_NAME;
    fstream output_file;
    const char* NUM_THREADS;

    // Validate inputs 
    if (argc == 3) {
        // Store matrix size
        SIZE = atoi(argv[1]);
        // Store file name
        FILE_NAME = argv[2];
        // Store the number of iterations.
        Iter = 100;
        // Determine number of threads
        NUM_THREADS = getenv("OMP_NUM_THREADS");
        cout << "Threads: " << NUM_THREADS << endl;
        cout << "Matrix size: " << SIZE << "x" << SIZE << endl;
        cout << "# Iteration: " << Iter << endl;
        cout << "Output file name: " << FILE_NAME << endl;
    } else if (argc == 4) {
        // Store matrix size
        SIZE = atoi(argv[1]);
        // Store file name
        FILE_NAME = argv[2];
        // Store the number of iterations.
        Iter = atoi(argv[3]);
        // Determine number of threads
        NUM_THREADS = getenv("OMP_NUM_THREADS");
        cout << "Threads: " << NUM_THREADS << endl;
        cout << "Matrix size: " << SIZE << "x" << SIZE << endl;
        cout << "# Iteration: " << Iter << endl;
        cout << "Output file name: " << FILE_NAME << endl;
    } else {
        cout << "Invalid number of input arguments" << endl;
        return -1;
    }  

    // Specify the run type
    std::string RUN_TYPE = FILE_NAME.find("Serial") != std::string::npos ? "serial" : "parallel";

    // Allocate matrices
    double** A = allocate_matrix(SIZE);
    double** B = allocate_matrix(SIZE);
    double** C = allocate_matrix(SIZE);

    // Fill matrices
    fill_matrix(A,SIZE);
    fill_matrix(B,SIZE);

    // Open file stream
    output_file.open(FILE_NAME, ios::out | ios::app);

    // Time matrix multiplication
    for (int i = 0; i < Iter; i++){
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

    // Write the matrices for verification.
    std::ostringstream filenameA;
    filenameA << "Matrices/matA_" << SIZE << "_" << NUM_THREADS << "_" << RUN_TYPE << ".csv";
    writeMatrixToCSV(A, SIZE, SIZE, filenameA.str(), 20);
    std::ostringstream filenameB;
    filenameB << "Matrices/matB_" << SIZE << "_" << NUM_THREADS << "_" << RUN_TYPE << ".csv";
    writeMatrixToCSV(B, SIZE, SIZE, filenameB.str(), 20);
    std::ostringstream filenameC;
    filenameC << "Matrices/matC_" << SIZE << "_" << NUM_THREADS << "_" << RUN_TYPE << ".csv";
    writeMatrixToCSV(C, SIZE, SIZE, filenameC.str(), 20);

    // Deallocate matrices
    deallocate_matrix(A,SIZE);
    deallocate_matrix(B,SIZE);
    deallocate_matrix(C,SIZE);

    return 0;
}



