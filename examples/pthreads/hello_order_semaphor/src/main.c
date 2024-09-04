// Copyright 2021 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0

#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Define a structure to hold shared data for threads
typedef struct shared_data {
  sem_t* can_greet;       // Array of semaphores
  uint64_t thread_count;  // Number of threads
} shared_data_t;

// Define a structure to hold private data for each thread
typedef struct private_data {
  uint64_t thread_number;  // Thread's unique number (rank)
  shared_data_t* shared_data;  // Pointer to shared data
} private_data_t;

void* greet(void* data);
int create_threads(shared_data_t* shared_data);

int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;  // Initialize error code to success
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);  // Get number of processors

  // If an argument is provided, convert it to an integer
  if (argc == 2) {
    if (sscanf(argv[1], "%" SCNu64, &thread_count) != 1) {
      fprintf(stderr, "Error: invalid thread count\n");  // Print error if conversion fails
      return 11;  // Return error code
    }
  }

  // Allocate memory for shared data
  shared_data_t* shared_data = (shared_data_t*)calloc(1, sizeof(shared_data_t));
  if (shared_data) {
    // Allocate memory for semaphores
    shared_data->can_greet = (sem_t*) calloc(thread_count, sizeof(sem_t));
    shared_data->thread_count = thread_count;  // Set thread count

    // Initialize semaphores
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count; ++thread_number) {
      error = sem_init(&shared_data->can_greet[thread_number], 0, !thread_number);
    }

    if (shared_data->can_greet) {
      struct timespec start_time, finish_time;
      clock_gettime(CLOCK_MONOTONIC, &start_time);

      error = create_threads(shared_data); 

      clock_gettime(CLOCK_MONOTONIC, &finish_time);
      double elapsed_time = finish_time.tv_sec - start_time.tv_sec +
        (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;  // Calculate elapsed time

      printf("Execution time: %.9lfs\n", elapsed_time);

      free(shared_data->can_greet);  // Free semaphore memory
    } else {
      fprintf(stderr, "Error: could not allocate semaphores\n");  // Print error if semaphore allocation fails
      error = 13; 
    }
    free(shared_data);  // Free shared data memory
  } else {
    fprintf(stderr, "Error: could not allocate shared data\n");  // Print error if shared data allocation fails
    error = 12;  // Set error code
  }
  return error;
}

// Function to create threads
int create_threads(shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;  // Initialize error code to success

  // Allocate memory for thread handles and private data
  pthread_t* threads = (pthread_t*) malloc(shared_data->thread_count * sizeof(pthread_t));
  private_data_t* private_data = (private_data_t*) calloc(shared_data->thread_count, sizeof(private_data_t));
  if (threads && private_data) {
    // Create threads
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count; ++thread_number) {
      if (error == EXIT_SUCCESS) {
        private_data[thread_number].thread_number = thread_number;  // Set thread number
        private_data[thread_number].shared_data = shared_data;  // Set shared data pointer
        error = pthread_create(&threads[thread_number], NULL, greet, &private_data[thread_number]);  // Create thread
        if (error != EXIT_SUCCESS) {
          fprintf(stderr, "Error: could not create secondary thread\n");
          error = 21;
          break;
        }
      } else {
        fprintf(stderr, "Error: could not init semaphore\n");
        error = 22;
        break;
      }
    }

    printf("Hello from main thread\n");

    // Join threads and destroy semaphores
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count; ++thread_number) {
      pthread_join(threads[thread_number], NULL);  // Wait for thread to finish
      sem_destroy(&shared_data->can_greet[thread_number]);  // Destroy semaphore
    }

    free(private_data);  // Free private data memory
    free(threads);  // Free thread handles memory
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n", shared_data->thread_count);
    error = 23;
  }

  return error;
}

void* greet(void* data) {
  assert(data);  // Ensure data is not NULL
  private_data_t* private_data = (private_data_t*) data;  // Cast data to private_data_t
  shared_data_t* shared_data = private_data->shared_data;  // Get shared data pointer

  printf("  %" PRIu64 "/%" PRIu64 ": wait for semaphore\n", private_data->thread_number, shared_data->thread_count);

  int error = sem_wait(&shared_data->can_greet[private_data->thread_number]);  // Wait for semaphore
  if (error) {
    fprintf(stderr, "error: could not wait for semaphore\n");  // Print error if semaphore wait fails
  }

  printf("Hello from secondary thread %" PRIu64 " of %" PRIu64 "\n", private_data->thread_number, shared_data->thread_count);

  const uint64_t next_thread = (private_data->thread_number + 1) % shared_data->thread_count;  // Calculate next thread number

  error = sem_post(&shared_data->can_greet[next_thread]);  // Signal next thread's semaphore
  if (error) {
    fprintf(stderr, "error: could not increment semaphore\n");
  }

  return NULL;
}