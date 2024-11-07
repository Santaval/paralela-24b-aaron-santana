// Copyright 2024 ECCI-UCR CC-BY-4
#include <omp.h>
#include <iostream>
#include <unistd.h>

#include "Mpi.hpp"

void play_ping_pong(Mpi& mpi);

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);
    if (mpi.size() != 2) {
      std::cout << "Process count must be 2" << std::endl;
      return EXIT_FAILURE;
    }
    play_ping_pong(mpi);
  } catch (const Mpi::Error& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
  return 0;
}

void play_ping_pong(Mpi& mpi) {
  int ball = 31;
  const int rank = mpi.rank();
  const int rival = rank ? 0 : 1;
  // saque
  if(!rank) {
    mpi.send(ball, 1);
  }

  while (true) {   
    mpi.receive(ball, rival);
    std::cout << rank <<" serves" << std::endl;
    usleep(500000);
    mpi.send(ball, rival);
  }
  
}
