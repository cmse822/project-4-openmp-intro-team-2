#include <iostream>
#include <fstream>
#include <string> 
#include <random>
#include <chrono>
#include <cstdlib>
#include "matrix_math.h"
#include "mpi.h"

using namespace std;

int main(int argc, char** argv){
    // Declare variables
    int SIZE;
    string FILE_NAME;
    string file_matrix_A;
    string file_matrix_B;
    fstream output_file;
    const char* NUM_THREADS;
    int num_tasks, rank, num_workers, *num_rows;

    // Obtain OpenMPI variables
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Validate inputs 
    if (argc != 3){
        if (rank == 0){
            cout << "Invalid number of input arguments" << endl;
            cout << "USAGE: matrix_multiplication {size} {output file}" << endl;
        }
        
        return -1;
    } else{
        // Store matrix size
        SIZE = atoi(argv[1]);

        // Store file name
        FILE_NAME = argv[2];

        // Determine number of threads
        //NUM_THREADS = getenv("OMP_NUM_THREADS");

        if (rank == 0){
            cout << "Threads: " << NUM_THREADS << endl;
            cout << "Matrix size: " << SIZE << "x" << SIZE << endl;
            cout << "Output file name: " << FILE_NAME << endl;
        }
    }
    
    // Determine the number of workers available
    num_workers = num_tasks-1;

    // Distribute excess work among remaining workers
    num_rows = new int[num_workers];

    for (int i = 0; i < num_workers; i++){
        num_rows[i] = SIZE / num_workers;

        // Allocate excess work across initial workers
        if ((i+1) <= (SIZE % num_workers)){
            num_rows[i]++;
        }
    }

    #ifdef DEBUG_MODE
        if (rank == 0){
            for (int i = 0; i < num_workers; i++){
                cout << num_rows[i] << endl;
            }
        }
    #endif

    // Initialize pointers to matrices
    double **A, **B, **C, **buffer, *B_flat;

    // Allocate matrix B, which is stored by all ranks
    B = allocate_matrix(SIZE);
    B_flat = new double[SIZE * SIZE];
    MPI_Request request;

    if (rank == 0){
        int offset = 0;
        int size_curr;

        // Allocate the entirety of matrix A in orchestrator only
        A = allocate_matrix(SIZE);
        C = allocate_matrix(SIZE);

        // Fill matrices
        fill_matrix(A,SIZE);
        fill_matrix(B,SIZE);

        // Stored flattened matrix B
        flatten(B,B_flat,SIZE);

        // Prepare matrix A for send. The number of flattened arrays it will
        // contain is the same as the total number of workers
        buffer = new double*[num_workers];
        for (int i = 0; i < num_workers; i++){
            // Determine the current size
            size_curr = num_rows[i] * SIZE;

            // Allocate submatrix to be sent
            buffer[i] = new double[size_curr];
            flatten(A + offset, buffer[i], num_rows[i], SIZE);

            offset += num_rows[i];

            // Send matrix
            MPI_Isend(buffer[i], size_curr, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD, &request);
            MPI_Request_free(&request);

            #ifdef DEBUG_MODE
                print_matrix(&buffer[i],1,size_curr);
            #endif
        }
    } else{
        // Allocate memory for buffer and receive
        buffer = new double*[num_rows[rank-1] * SIZE];
        MPI_Irecv(buffer, num_rows[rank+1] * SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Request_free(&request);

        A = allocate_matrix(num_rows[rank-1], SIZE);
        C = allocate_matrix(num_rows[rank-1], SIZE);
    }

    // Broadcast matrix B
    MPI_Bcast(B_flat, SIZE*SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    #ifdef DEBUG_MODE
        if(rank == 1){
            print_matrix(&B_flat,1,SIZE * SIZE);
        }
    #endif

    // Perform matrix multiplication
    if (rank !=0){
        // Unflatten matrices
        unflatten(B_flat, B, SIZE, SIZE);
        //unflatten(buffer[rank+1], A, num_rows[rank+1], SIZE);
    }

    // Finalize communication across workers
    MPI_Finalize();
    /*
    

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
    */ 

    return 0;
}
