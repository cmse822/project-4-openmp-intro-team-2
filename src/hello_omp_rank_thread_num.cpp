#include <iostream>
#include "mpi.h"
#include <omp.h>
#include <stdio.h>
using namespace std;

int main(int argc, char *argv[])
{
    int numtasks, rank, provided;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // implementing the OpenMP threading
#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        printf("Hello World!  (rank = %d, Thread %d)\n", rank, thread_id);
        
    }
    MPI_Finalize();
    return 0;

}
