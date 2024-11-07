// Copyright 2024 ECCI-UCR CC-BY-4
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

#include "Mpi.hpp"

int main(int argc, char* argv[]) {
  try {
    Mpi mpi(argc, argv);
    usleep(100000 * (mpi.size() - mpi.rank()));
    std::stringstream message;
    message << "Hello from main thread of process " << mpi.getProcessNumber()
        << " of " << mpi.getProcessCount() << std::endl; // << " on " << mpi.getHostname()
        
    if (mpi.rank() == 0) {
      std::vector<std::string> greetings(mpi.size());
      greetings[mpi.rank()] = message.str();
      for (int counter = 1; counter < mpi.size(); ++counter) {
        std::string greet;
        int source = -1;
        mpi.receive(greet, MPI_ANY_SOURCE, 128, 0, &source);
        assert(0 <= source && source < mpi.size());
        greetings[source] = greet;
      }
      for (const std::string& greet : greetings) {
        std::cout << greet;
      }
    } else {
      mpi.send(message.str(), 0);
    }
  } catch (const std::exception& error) {
    std::cerr << "error: " << error.what() << std::endl;
  }
}
