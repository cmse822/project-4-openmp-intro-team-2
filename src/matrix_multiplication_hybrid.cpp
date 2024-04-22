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
    const char* THREADS;
    int NUM_THREADS = 0;
    int num_tasks, rank, num_workers, *num_rows;
    double start, time;

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
        THREADS = getenv("OMP_NUM_THREADS");

        if (THREADS != nullptr){
            NUM_THREADS = atoi(THREADS);
        }



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
    double **A, **B, **C, **buffer, *buffer_worker, *B_flat;

    // Allocate matrix B, which is stored by all ranks
    B = allocate_matrix(SIZE);
    B_flat = new double[SIZE * SIZE];
    MPI_Request request, *requests;

    if (rank == 0){
        int offset = 0;
        int size_curr;

        // Allocate the entirety of matrix A in orchestrator only
        A = allocate_matrix(SIZE);
        C = allocate_matrix(SIZE);

        // Fill matrices
        fill_matrix(A,SIZE,true);
        fill_matrix(B,SIZE,true);

        // Start timer
        start = MPI_Wtime();

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
            MPI_Send(buffer[i], size_curr, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD);
        }

        #ifdef DEBUG_MODE 
        // Print matrices
        cout << "A:" << endl;
        print_matrix(A,SIZE);

        cout << "B:" << endl;
        print_matrix(B,SIZE);

        cout << "B_flat:" << endl;
        print_matrix(&B_flat,1,SIZE*SIZE);
        #endif
    } else{
        // Allocate memory for matrices
        A = allocate_matrix(num_rows[rank-1], SIZE);
        C = allocate_matrix(num_rows[rank-1], SIZE);

        // Allocate memory for buffer and receive
        buffer_worker = new double[num_rows[rank-1] * SIZE];
        MPI_Recv(buffer_worker, num_rows[rank-1] * SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Broadcast matrix B
    MPI_Bcast(B_flat, SIZE*SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank !=0){
        // Unflatten received data
        unflatten(B_flat, B, SIZE, SIZE);
        unflatten(buffer_worker, A, num_rows[rank-1], SIZE);
    }

    #ifdef DEBUG_MODE
    if (rank == 1){
        cout << "Rank " << rank << ":" << endl;
        print_matrix(A,num_rows[rank-1],SIZE);
        print_matrix(B,SIZE);
        print_matrix(&B_flat, 1, SIZE*SIZE);
    }
    #endif 

    // Perform matrix multiplication
    if (rank !=0){
        matrix_multiplication(A,B,C, SIZE, num_rows[rank-1]);
    }

    #ifdef DEBUG_MODE
    if (rank==1){
        print_matrix(C,num_rows[rank-1],SIZE);
    }
    #endif

    if (rank == 0){
        // Retrieve matrix
        for (int i = 0; i < num_workers; i++){
            MPI_Recv(buffer[i], num_rows[i] * SIZE, MPI_DOUBLE, i+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            #ifdef DEBUG_MODE
            cout << "Received from rank " << i+1 << ":" << endl;
            print_matrix(&buffer[i], 1, num_rows[i]*SIZE);
            #endif
        }

        // Reconstruct matrix
        int offset = 0;
        for (int i = 0; i < num_workers; i++){
            unflatten(buffer[i], C + offset, num_rows[i], SIZE);

            offset += num_rows[i];
        }

        // Stop time
        time = MPI_Wtime() - start;
        
        #ifdef DEBUG_MODE
        cout << "Result:" << endl;
        print_matrix(C, SIZE);
        #endif

        cout << "in " << time << "seconds" << endl;

        // Save to output file
        fstream output_file;
        output_file.open(FILE_NAME, ios::out | ios::app);
        output_file << SIZE << "," << num_tasks << "," << NUM_THREADS <<  "," << time << endl;
        output_file.close();
    } else{
        // Flatten matrix
        flatten(C, buffer_worker, num_rows[rank-1], SIZE);

        // Send to orchestrator
        MPI_Send(buffer_worker, num_rows[rank-1] * SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Deallocate memory
    if (rank == 0){
        // Deallocate matrices
        deallocate_matrix(A, SIZE);
        deallocate_matrix(C, SIZE);

        // Deallocate buffer
        //for (int i = 0; i < num_tasks; i++){
        //    delete[] buffer[i];
        //}

        delete[] buffer;
        delete[] B_flat;
    } else{
        // Deallocate matrices
        deallocate_matrix(A, num_rows[rank-1]);
        deallocate_matrix(C, num_rows[rank-1]);

        // Deallocate buffers
        delete[] buffer_worker;
    }

    deallocate_matrix(B,SIZE);
    delete[] num_rows;

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
