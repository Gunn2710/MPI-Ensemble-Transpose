// Toolchain check: make build/hello && mpirun -np 4 build/hello

#include <mpi.h>

#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank = 0;
    int nranks = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nranks);

    std::cout << "hello from rank " << rank << " of " << nranks << "\n";

    MPI_Finalize();
    return 0;
}
