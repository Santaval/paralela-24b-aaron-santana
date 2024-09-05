#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// thread_shared_data_t

// we define a struct that will be used to store the shared data between the threads

typedef struct shared_data {
    // this is the position, this will be used to store the position of the thread
  uint64_t position;
  // this is the mutex, this will be used to check if a thread can access the position and modify it 
  pthread_mutex_t can_access_position;
    // this is the thread count, this will be used to store the number of threads that will be created
  uint64_t thread_count;
} shared_data_t;

// thread_private_data_t
// also, we define a struct that will be used to store the private data of each thread
typedef struct private_data {
    // this is the thread number, this will be used to store the rank of the thread
  uint64_t thread_number;  // rank
  // this is the shared data, this will be used to store the shared data between the threads
  shared_data_t* shared_data;
} private_data_t;

/**
 * @brief ...
 */
void* race(void* data);
int create_threads(shared_data_t* shared_data);

// procedure main(argc, argv[])
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  // create thread_count as result of converting argv[1] to integer
  // thread_count := integer(argv[1])
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  if (argc == 2) {
    if (sscanf(argv[1], "%" SCNu64, &thread_count) == 1) {
    } else {
      fprintf(stderr, "Error: invalid thread count\n");
      return 11;
    }
  }
    // init the shared data
  shared_data_t* shared_data = (shared_data_t*)calloc(1, sizeof(shared_data_t));
  if (shared_data) {
    shared_data->position = 0;

    // init mutex that receives shared_data->can_access_position as parameter, 
    // this will be used to check if a thread can access the position and modify it
    error = pthread_mutex_init(&shared_data->can_access_position, /*attr*/NULL);
    if (error == EXIT_SUCCESS) {
      shared_data->thread_count = thread_count;

      struct timespec start_time, finish_time;
      clock_gettime(CLOCK_MONOTONIC, &start_time);

      error = create_threads(shared_data);

      clock_gettime(CLOCK_MONOTONIC, &finish_time);
      double elapsed_time = finish_time.tv_sec - start_time.tv_sec +
        (finish_time.tv_nsec - start_time.tv_nsec) * 1e-9;

      printf("Execution time: %.9lfs\n", elapsed_time);

      // finally, destroy the mutex and free the shared data
      pthread_mutex_destroy(&shared_data->can_access_position);
      free(shared_data);
    } else {
      fprintf(stderr, "Error: could not init mutex\n");
      return 13;
    }
  } else {
    fprintf(stderr, "Error: could not allocate shared data\n");
    return 12;
  }
  return error;
}  // end procedure


int create_threads(shared_data_t* shared_data) {
  int error = EXIT_SUCCESS;
  // for thread_number := 0 to thread_count do
  pthread_t* threads = (pthread_t*)
    malloc(shared_data->thread_count * sizeof(pthread_t));
  private_data_t* private_data = (private_data_t*)
    calloc(shared_data->thread_count, sizeof(private_data_t));
  if (threads && private_data) {
    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
        ; ++thread_number) {
      private_data[thread_number].thread_number = thread_number;
      private_data[thread_number].shared_data = shared_data;
      // create_thread(greet, thread_number)
      error = pthread_create(&threads[thread_number], /*attr*/ NULL, race
        , /*arg*/ &private_data[thread_number]);
      if (error == EXIT_SUCCESS) {
      } else {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = 21;
        break;
      }
    }

    // print "Hello from main thread"
    printf("Hello from main thread\n");

    for (uint64_t thread_number = 0; thread_number < shared_data->thread_count
        ; ++thread_number) {
      pthread_join(threads[thread_number], /*value_ptr*/ NULL);
    }

    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %" PRIu64 " threads\n"
      , shared_data->thread_count);
    error = 22;
  }

  return error;
}

// procedure greet:
void* race(void* data) {
  assert(data);
  private_data_t* private_data = (private_data_t*) data;
  shared_data_t* shared_data = private_data->shared_data;

  // lock(can_access_position)
  // lock the mutex, that indicates that the thread is going to access the shared data and other threads can't access it
  pthread_mutex_lock(&shared_data->can_access_position);
  // race condition/data race/condición de carrera:
  // modificación concurrente de memoria compartida
  // position := position + 1
  ++shared_data->position;
  // my_position := position
  uint64_t my_position = shared_data->position;
  // print "Hello from secondary thread"
  printf("Thread %" PRIu64 "/%" PRIu64 ": I arrived at position %" PRIu64 "\n"
    , private_data->thread_number, shared_data->thread_count, my_position);

  // unlock(can_access_position)
  // after the thread has accessed the shared data, it unlocks the mutex, so other threads can access it
  pthread_mutex_unlock(&shared_data->can_access_position);
  return NULL;
}  // end procedure