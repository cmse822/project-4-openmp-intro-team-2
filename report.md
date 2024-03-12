# Project 4 (OpenMP and Hybrid Parallelism) Team 2 Report

## Part 1: OpenMP Matrix-Matrix Multiplication


## Part 2: Adding OpenMP threading to a simple MPI application
- The print statement was wrapped in the `omp parallel` region after the MPI initialization environment.
- The `MPI_Init` was modified to `MPI_Init_thread` to allow for thread support. The level of thread support needed is the `MPI_THREAD_FUNNELED`. It's good to point out that using just `MPI_Init` performs similarly as the `MPI_Init_thread`.
- `<omp.h>` was used to extract the unique thread number using `omp_get_thread_num()` in printing the `Hello World!`.
- The image below shows the differences between setting the threads environment variable number and not.
![Alt text](<omp simple mpi app-1.jpg>)

- Within the code, we set the print statement to only print when we have `rank == 1`. Therefore, we only have one print statement per thread. If we remove this constraints on the rank, we will have two print statements per thread. 
On the left side of the image, we see that the mpi utilized all the available threads, but when we specified the `number of thread = 4`, the mpi was limited to print using `4` threads.
- The image below shows the results obtained when the constraints on the rank is removed, and the rank number is printed together with the thread number used.
![Alt text](<omp simple mpi app with rank.jpg>)
- From both figures, we see clearly that the print statement did not follow any particular order, and this is because of the non-deterministic scheduling of threads and processes.