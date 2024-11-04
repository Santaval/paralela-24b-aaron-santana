// Copyright 2024 ECCI-UCR CC-BY-4
#include <iostream>
#include <unistd.h>

#include "Mpi.hpp"

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);
    int rank = mpi.rank();
    int continueSignal = 1;
    if (rank > 0) {
      mpi.receive<int>(continueSignal, rank - 1);
    }


    usleep(100000 *( mpi.size() - mpi.rank()));
    std::cout << "Hello from main thread of process " << mpi.getProcessNumber()
        << " of " << mpi.getProcessCount() << " on " << mpi.getHostname()
        << std::endl;
  
    if (rank < (mpi.size() - 1)) {
      mpi.send(continueSignal, rank + 1);
    }
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
}
