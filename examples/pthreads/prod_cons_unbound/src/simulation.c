// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/random.h>
#include <stdio.h>

#include "common.h"
#include "consumer.h"
#include "producer.h"
#include "simulation.h"

// Function prototypes
int analyze_arguments(simulation_t* simulation, int argc, char* argv[]);
int create_consumers_producers(simulation_t* simulation);
int join_threads(size_t count, pthread_t* threads);

// Create and initialize a new simulation structure
simulation_t* simulation_create() {
  simulation_t* simulation = (simulation_t*) calloc(1, sizeof(simulation_t));
  if (simulation) {
    simulation->unit_count = 0;
    simulation->producer_count = 0;
    simulation->consumer_count = 0;
    simulation->producer_min_delay = 0;
    simulation->producer_max_delay = 0;
    simulation->consumer_min_delay = 0;
    simulation->consumer_max_delay = 0;
    queue_init(&simulation->queue);  // Initialize the queue
    simulation->next_unit = 0;
    simulation->consumed_count = 0;
  }
  return simulation;
}

// Free the memory allocated for the simulation structure and its queue
void simulation_destroy(simulation_t* simulation) {
  assert(simulation);  // Ensure the simulation pointer is not NULL
  queue_destroy(&simulation->queue);  // Destroy the queue
  free(simulation);  // Free the simulation structure
}

// Run the simulation by analyzing arguments and creating threads
int simulation_run(simulation_t* simulation, int argc, char* argv[]) {
  int error = analyze_arguments(simulation, argc, argv);
  if (error == EXIT_SUCCESS) {
    unsigned int seed = 0;
    getrandom(&seed, sizeof(seed), GRND_NONBLOCK);  // Seed for random number generation
    srandom(seed);

    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);  // Record the start time

    error = create_consumers_producers(simulation);  // Create and manage producer/consumer threads

    struct timespec finish_time;
    clock_gettime(CLOCK_MONOTONIC, &finish_time);  // Record the finish time

    // Calculate and print the elapsed time
    double elapsed = (finish_time.tv_sec - start_time.tv_sec) +
      (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;
    printf("execution time: %.9lfs\n", elapsed);
  }
  return error;
}

// Analyze command-line arguments and initialize the simulation parameters
int analyze_arguments(simulation_t* simulation, int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  if (argc == 8) {
    // Parse and validate command-line arguments
    if (sscanf(argv[1], "%zu", &simulation->unit_count) != 1 || simulation->unit_count == 0) {
        fprintf(stderr, "error: invalid unit count\n");
        error = ERR_UNIT_COUNT;
    } else if (sscanf(argv[2], "%zu", &simulation->producer_count) != 1 || simulation->producer_count == 0) {
        fprintf(stderr, "error: invalid producer count\n");
        error = ERR_PRODUCER_COUNT;
    } else if (sscanf(argv[3], "%zu", &simulation->consumer_count) != 1 || simulation->consumer_count == 0) {
        fprintf(stderr, "error: invalid consumer count\n");
        error = ERR_CONSUMER_COUNT;
    } else if (sscanf(argv[4], "%u", &simulation->producer_min_delay) != 1) {
        fprintf(stderr, "error: invalid min producer delay\n");
        error = ERR_MIN_PROD_DELAY;
    } else if (sscanf(argv[5], "%u", &simulation->producer_max_delay) != 1) {
        fprintf(stderr, "error: invalid max producer delay\n");
        error = ERR_MAX_PROD_DELAY;
    } else if (sscanf(argv[6], "%u", &simulation->consumer_min_delay) != 1) {
        fprintf(stderr, "error: invalid min consumer delay\n");
        error = ERR_MIN_CONS_DELAY;
    } else if (sscanf(argv[7], "%u", &simulation->consumer_max_delay) != 1) {
        fprintf(stderr, "error: invalid max consumer delay\n");
        error = ERR_MAX_CONS_DELAY;
    }
  } else {
    fprintf(stderr, "usage: producer_consumer unit_count producer_count consumer_count"
      " producer_min_delay producer_max_delay consumer_min_delay consumer_max_delay\n");
    error = ERR_NO_ARGS;
  }
  return error;
}

// Create an array of threads for the specified number of threads, using the provided subroutine
pthread_t* create_threads(size_t count, void*(*subroutine)(void*), void* data) {
  pthread_t* threads = (pthread_t*) calloc(count, sizeof(pthread_t));
  if (threads) {
    for (size_t index = 0; index < count; ++index) {
      if (pthread_create(&threads[index], NULL, subroutine, data) != EXIT_SUCCESS) {
        fprintf(stderr, "error: could not create thread %zu\n", index);
        join_threads(index, threads);  // Clean up already created threads
        return NULL;
      }
    }
  }
  return threads;
}

// Wait for all threads to complete and free their resources
int join_threads(size_t count, pthread_t* threads) {
  int error = EXIT_SUCCESS;
  for (size_t index = 0; index < count; ++index) {
    // Wait for each thread to complete and aggregate any errors
    error += pthread_join(threads[index], NULL);
  }
  free(threads);  // Free the array of thread identifiers
  return error;
}

// Create and manage the producer and consumer threads
int create_consumers_producers(simulation_t* simulation) {
  assert(simulation);  // Ensure the simulation pointer is not NULL
  int error = EXIT_SUCCESS;

  pthread_t* producers = create_threads(simulation->producer_count, produce, simulation);
  pthread_t* consumers = create_threads(simulation->consumer_count, consume, simulation);

  if (producers && consumers) {
    join_threads(simulation->producer_count, producers);  // Wait for producers to finish
    join_threads(simulation->consumer_count, consumers);  // Wait for consumers to finish
  } else {
    fprintf(stderr, "error: could not create threads\n");
    error = ERR_CREATE_THREAD;
  }

  return error;
}
