#include <iostream>
#include <fstream>
#include <string> 
#include <random>
#include <chrono>
#include <cstdlib>

using namespace std;

double** allocate_matrix(int size){
    double** A;
    
    // Allocate memory for each row
    A = new double*[size];

    for(int i = 0; i < size; i++){
        A[i] = new double[size];
    }

    return A;
}

void fill_matrix(double** A, int size, bool flag=false){
    double val;

    // Initialie random device
    random_device           rd;
    mt19937                 eng(rd());
    uniform_real_distribution<>     distribution(-1,1);

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
            cout << A[i][j] << " ";
        }

        cout << endl;
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

int main(int argc, char** argv){
    // Declare variables
    int SIZE;
    string FILE_NAME;
    fstream output_file;
    const char* NUM_THREADS;

    // Validate inputs 
    if (argc != 3){
        cout << "Invalid number of input arguments" << endl;

        return -1;
    } else{
        // Store matrix size
        SIZE = atoi(argv[1]);

        // Store file name
        FILE_NAME = argv[2];

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
    fill_matrix(A,SIZE);
    fill_matrix(B,SIZE);

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
