// Copyright 2024 ECCI-UCR CC-BY-4
#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]) {
  if (MPI_Init(&argc, &argv) == MPI_SUCCESS) {
    int process_number = -1;
    int process_count = -1;
    char process_hostname[MPI_MAX_PROCESSOR_NAME] = { '\0' };
    int process_hostname_length = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_number);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    MPI_Get_processor_name(process_hostname, &process_hostname_length);

    std::cout << "Hello from main thread of process " << process_number
        << " of " << process_count << " on " << process_hostname << std::endl;

    MPI_Finalize();
  } else {
    std::cerr << "Error initializing MPI" << std::endl;
  }

}
