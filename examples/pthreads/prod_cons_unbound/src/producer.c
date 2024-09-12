// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#include <stdio.h>

#include "common.h"   // Header file containing common definitions and declarations
#include "producer.h" // Header file specific to producer functionality, declaring the `produce` function

// Producer thread function
void* produce(void* data) {
  // Cast the void pointer to the `simulation_t` structure
  simulation_t* simulation = (simulation_t*)data;

  // Loop until all units are produced
  while (simulation->next_unit < simulation->unit_count) {
    // Increment and get the next unit number to produce
    size_t my_unit = ++simulation->next_unit;

    // Sleep for a random duration within the specified delay range
    usleep(1000 * random_between(simulation->producer_min_delay
      , simulation->producer_max_delay));

    // Enqueue the produced unit into the queue
    queue_enqueue(&simulation->queue, my_unit);

    // Print the produced unit number
    printf("Produced %zu\n", my_unit);
  }

  return NULL;
}
