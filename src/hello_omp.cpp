#include <iostream>
#include "mpi.h"
#include <omp.h>
using namespace std;

int main(int argc, char *argv[])
{
    int numtasks, rank, provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // implementing the OpenMP threading
#pragma omp parallel
    {
         int thread_id = omp_get_thread_num();

        if (rank == numtasks -1)
        {
            cout << "Hello World: " << thread_id << endl;
        }
    }
    MPI_Finalize();
    return 0;

}
