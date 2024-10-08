// Copyright 2024 ECCI-UCR CC-BY 4.0
#include <omp.h>
#include <iostream>

int main(int argc, char* argv[]) {
  const int thread_count = argc == 2 ? ::atoi(argv[1]) : omp_get_max_threads();

  #pragma omp parallel num_threads(thread_count) \
      default(none) shared(std::cout, thread_count)
  { // firstprivate(thread_count)
    #pragma omp critical(can_greet)
    std::cout << "Hello from secondary thread " << omp_get_num_procs()
    << " of " << thread_count << std::endl;
  }

}
