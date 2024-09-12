// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4
// This program simulates a producer-consumer system using an unbounded buffer.

#include <stdlib.h>
#include <stdio.h>

#include "simulation.h"  // Header file that contains declarations related to simulation management

int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;

  // Create a new simulation instance
  simulation_t* simulation = simulation_create();

  // Check if simulation instance was created successfully
  if (simulation) {
    // Run the simulation with command-line arguments
    error = simulation_run(simulation, argc, argv);

    // Clean up and destroy the simulation instance
    simulation_destroy(simulation);
  } else {
    // Print an error message if the simulation instance could not be allocated
    fprintf(stderr, "error: could not allocate simulation\n");
    error = EXIT_FAILURE;
  }

  // Return the exit status
  return error;
}
