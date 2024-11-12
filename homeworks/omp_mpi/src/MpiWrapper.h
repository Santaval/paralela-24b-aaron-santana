#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int rank;
    int size;
    char hostname[MPI_MAX_PROCESSOR_NAME];
} Mpi;

int mpi_init(Mpi* mpi, int* argc, char*** argv) {
    int hostname_len;
    if (MPI_Init(argc, &argv) != MPI_SUCCESS) {
        fprintf(stderr, "Error: could not init mpi\n");
        return -1;
    }
    if (MPI_Comm_rank(MPI_COMM_WORLD, &mpi->rank) != MPI_SUCCESS) {
        fprintf(stderr, "Error: could not get MPI rank\n");
        return -1;
    }

    if(MPI_Comm_size(MPI_COMM_WORLD, &mpi->size) != MPI_SUCCESS) {
        fprintf(stderr, "Error: could not get MPI rank\n");
        return -1;
    }
    if(MPI_Get_processor_name(mpi->hostname, &hostname_len) != MPI_SUCCESS) {
        fprintf(stderr, "Error: could not get MPI rank\n");
        return -1;
    }
    return 0;
}

void mpi_finalize() {
    MPI_Finalize();
}

int mpi_send(const void* data, int count,MPI_Datatype dataType,
int toProcess, int tag) {

    if (MPI_Send(data, count, dataType, toProcess, tag, MPI_COMM_WORLD)
        != MPI_SUCCESS) {
            fprintf(stderr, "Error: could not send data \n");
            return -1;
        }
    return 0;

}

int mpi_receive(void* data, int count, MPI_Datatype dataType,
    int fromProcess, int tag, int* source) {
    MPI_Status status;
    if (MPI_Recv(data, count, dataType, fromProcess, tag, MPI_COMM_WORLD,
        &status) != MPI_SUCCESS) {
            fprintf(stderr, "Error: could not receive data \n");
            return -1;
    }

    if (source) {
        *source = status.MPI_SOURCE;
    }

    int receivedCount;
    if (MPI_Get_count(&status, dataType, &receivedCount) != MPI_SUCCESS) {
        fprintf(stderr, "Error: could not get count from status \n");
        return -1;
    }

    return receivedCount;
}