// Copyright 2021 Jeisson Hidalgo-Cespedes <jeisson.hidalgo@ucr.ac.cr> CC-BY-4
// This program simulates a producer-consumer scenario using a bounded buffer.

#define _DEFAULT_SOURCE  // Ensures that the program includes the default feature set

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <unistd.h>

// Error codes for handling specific failures
enum {
  ERR_NOMEM_SHARED = EXIT_FAILURE + 1,  // Error code for memory allocation failure of shared data
  ERR_NOMEM_BUFFER,                     // Error code for memory allocation failure of the buffer
  ERR_NO_ARGS,                          // Error code for missing command-line arguments
  ERR_BUFFER_CAPACITY,                  // Error code for invalid buffer capacity
  ERR_ROUND_COUNT,                      // Error code for invalid round count
  ERR_MIN_PROD_DELAY,                   // Error code for invalid minimum producer delay
  ERR_MAX_PROD_DELAY,                   // Error code for invalid maximum producer delay
  ERR_MIN_CONS_DELAY,                   // Error code for invalid minimum consumer delay
  ERR_MAX_CONS_DELAY,                   // Error code for invalid maximum consumer delay
  ERR_CREATE_THREAD,                    // Error code for thread creation failure
};

// Structure for shared data between producer and consumer
typedef struct {
  size_t thread_count;                 // Number of threads (not used in this implementation)
  size_t buffer_capacity;              // Maximum capacity of the buffer
  double* buffer;                      // Pointer to the buffer storing produced data
  size_t rounds;                       // Number of rounds each thread will perform
  useconds_t producer_min_delay;       // Minimum delay time for producer (in microseconds)
  useconds_t producer_max_delay;       // Maximum delay time for producer (in microseconds)
  useconds_t consumer_min_delay;       // Minimum delay time for consumer (in microseconds)
  useconds_t consumer_max_delay;       // Maximum delay time for consumer (in microseconds)

  sem_t can_produce;                   // Semaphore to control production access
  sem_t can_consume;                   // Semaphore to control consumption access
} shared_data_t;

// Structure for private thread data (not used in this implementation)
typedef struct {
  size_t thread_number;                // Identifier for the thread
  shared_data_t* shared_data;         // Pointer to the shared data structure
} private_data_t;

// Function prototypes
int analyze_arguments(int argc, char* argv[], shared_data_t* shared_data);
int create_threads(shared_data_t* shared_data);
void* produce(void* data);
void* consume(void* data);
useconds_t random_between(useconds_t min, useconds_t max);

int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;

  // Allocate memory for shared data structure
  shared_data_t* shared_data = (shared_data_t*)
    calloc(1, sizeof(shared_data_t));

  if (shared_data) {
    // Analyze command-line arguments and initialize shared data
    error = analyze_arguments(argc, argv, shared_data);
    if (error == EXIT_SUCCESS) {
      // Allocate memory for the buffer
      shared_data->buffer = (double*)
        calloc(shared_data->buffer_capacity, sizeof(double));
      if (shared_data->buffer) {
        // Initialize semaphores
        sem_init(&shared_data->can_produce, /*pshared*/ 0,
          shared_data->buffer_capacity);
        sem_init(&shared_data->can_consume, /*pshared*/ 0, /*value*/ 0);

        // Seed the random number generator
        unsigned int seed = 0u;
        getrandom(&seed, sizeof(seed), GRND_NONBLOCK);
        srandom(seed);

        // Measure the execution time
        struct timespec start_time;
        clock_gettime(/*clk_id*/CLOCK_MONOTONIC, &start_time);

        // Create producer and consumer threads
        error = create_threads(shared_data);

        struct timespec finish_time;
        clock_gettime(/*clk_id*/CLOCK_MONOTONIC, &finish_time);

        // Calculate and print the elapsed time
        double elapsed = (finish_time.tv_sec - start_time.tv_sec) +
          (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;
        printf("execution time: %.9lfs\n", elapsed);

        // Clean up
        sem_destroy(&shared_data->can_consume);
        sem_destroy(&shared_data->can_produce);
        free(shared_data->buffer);
      } else {
        fprintf(stderr, "error: could not create buffer\n");
        error = ERR_NOMEM_BUFFER;
      }
    }

    free(shared_data);
  } else {
    fprintf(stderr, "Error: could not allocate shared data\n");
    error = ERR_NOMEM_SHARED;
  }

  return error;
}

// Analyze command-line arguments and initialize shared data structure
int analyze_arguments(int argc, char* argv[], shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;
  if (argc == 7) {
    if (sscanf(argv[1], "%zu", &shared_data->buffer_capacity) != 1
      || shared_data->buffer_capacity == 0) {
        fprintf(stderr, "error: invalid buffer capacity\n");
        error = ERR_BUFFER_CAPACITY;
    } else if (sscanf(argv[2], "%zu", &shared_data->rounds) != 1
      || shared_data->rounds == 0) {
        fprintf(stderr, "error: invalid round count\n");
        error = ERR_ROUND_COUNT;
    } else if (sscanf(argv[3], "%u", &shared_data->producer_min_delay) != 1) {
        fprintf(stderr, "error: invalid min producer delay\n");
        error = ERR_MIN_PROD_DELAY;
    } else if (sscanf(argv[4], "%u", &shared_data->producer_max_delay) != 1) {
        fprintf(stderr, "error: invalid max producer delay\n");
        error = ERR_MAX_PROD_DELAY;
    } else if (sscanf(argv[5], "%u", &shared_data->consumer_min_delay) != 1) {
        fprintf(stderr, "error: invalid min consumer delay\n");
        error = ERR_MIN_CONS_DELAY;
    } else if (sscanf(argv[6], "%u", &shared_data->consumer_max_delay) != 1) {
        fprintf(stderr, "error: invalid max consumer delay\n");
        error = ERR_MAX_CONS_DELAY;
    }
  } else {
    fprintf(stderr, "usage: prod_cons_bound buffer_capacity rounds"
      " producer_min_delay producer_max_delay"
      " consumer_min_delay consumer_max_delay\n");
      error = ERR_NO_ARGS;
  }
  return error;
}

// Create producer and consumer threads
int create_threads(shared_data_t* shared_data) {
  assert(shared_data);
  int error = EXIT_SUCCESS;

  pthread_t producer, consumer;
  error = pthread_create(&producer, /*attr*/ NULL, produce, shared_data);
  if (error == EXIT_SUCCESS) {
    error = pthread_create(&consumer, /*attr*/ NULL, consume, shared_data);
    if (error != EXIT_SUCCESS) {
      fprintf(stderr, "error: could not create consumer\n");
      error = ERR_CREATE_THREAD;
    }
  } else {
    fprintf(stderr, "error: could not create producer\n");
    error = ERR_CREATE_THREAD;
  }

  // Wait for both threads to finish
  if (error == EXIT_SUCCESS) {
    pthread_join(producer, /*value_ptr*/ NULL);
    pthread_join(consumer, /*value_ptr*/ NULL);
  }

  return error;
}

// Producer thread function
void* produce(void* data) {
  shared_data_t* shared_data = (shared_data_t*)data;
  size_t count = 0;
  for (size_t round = 0; round < shared_data->rounds; ++round) {
    for (size_t index = 0; index < shared_data->buffer_capacity; ++index) {
      // Wait until a slot is available for producing
      sem_wait(&shared_data->can_produce);

      // Sleep for a random delay within the specified range
      usleep(1000 * random_between(shared_data->producer_min_delay
        , shared_data->producer_max_delay));
      
      // Produce an item and store it in the buffer
      shared_data->buffer[index] = ++count;
      printf("Produced %lg\n", shared_data->buffer[index]);

      // Signal that an item is available for consumption
      sem_post(&shared_data->can_consume);
    }
  }

  return NULL;
}

// Consumer thread function
void* consume(void* data) {
  shared_data_t* shared_data = (shared_data_t*)data;
  for (size_t round = 0; round < shared_data->rounds; ++round) {
    for (size_t index = 0; index < shared_data->buffer_capacity; ++index) {
      // Wait until an item is available for consuming
      sem_wait(&shared_data->can_consume);

      // Consume the item from the buffer
      double value = shared_data->buffer[index];
      usleep(1000 * random_between(shared_data->consumer_min_delay
        , shared_data->consumer_max_delay));
      printf("\tConsumed %lg\n", value);

      // Signal that a slot is available for producing
      sem_post(&shared_data->can_produce);
    }
  }

  return NULL;
}

// Generate a random number between min and max (inclusive)
useconds_t random_between(useconds_t min, useconds_t max) {
  return min + (max > min ? (random() % (max - min)) : 0);
}