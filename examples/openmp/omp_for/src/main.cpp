// Copyright 2024 ECCI-UCR CC-BY 4.0
#include <omp.h>
#include <iostream>

int main(int argc, char* argv[]) {
  const int thread_count = argc >= 2 ? ::atoi(argv[1]) : omp_get_num_procs();
  const int iteration_count = argc >= 3 ? ::atoi(argv[2]) : thread_count;

  #pragma omp parallel for num_threads(thread_count) schedule(dynamic, 2) \
      default(none) shared(std::cout, iteration_count)
  for (int iteration = 0; iteration < iteration_count; ++iteration) {
     #pragma omp critical(can_print)
    std::cout << omp_get_thread_num() << '/' << omp_get_num_threads()
      << ": iteration " << iteration << '/' << iteration_count << std::endl;
  }
}
