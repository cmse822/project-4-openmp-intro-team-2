#include <iostream>
#include <fstream>
#include <string> 
#include <random>
#include <chrono>

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

void load_matrix(double** matrix, int size, string filename){
    fstream matrix_file;
    matrix_file.open(filename);
    string line;
    string delimiter = ",";
    size_t pos = 0;
    string token;
    int i = 0;
    int j = 0;
    double val;
    while ( matrix_file && i < size) {
        getline(matrix_file, line);
        delimiter = ",";
        pos = 0;
        j = 0;
        while ((pos = line.find(delimiter)) != string::npos) {
            token = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
            val = stod(token);
            matrix[i][j] = val;
            j++;
        }
        val = stod(token);
        matrix[i][j] = val;
        i++;
    }
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
    int i;

    // Create a parallel region
    #pragma omp parallel shared(A,B,C) private(i)
    {
        #pragma omp for schedule(dynamic)
        for(i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                for(int k = 0; k < size; k ++){
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        } 
    }
}

int main(int argc, char** argv){
    // Declare variables
    int SIZE;
    string FILE_NAME;
    string file_matrix_A;
    string file_matrix_B;
    fstream output_file;

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
        output_file << SIZE << "," << time.count() << endl;
    }

    // Close file_stream
    output_file.close();

    // Deallocate matrices
    deallocate_matrix(A,SIZE);
    deallocate_matrix(B,SIZE);
    deallocate_matrix(C,SIZE);

    return 0;
}
