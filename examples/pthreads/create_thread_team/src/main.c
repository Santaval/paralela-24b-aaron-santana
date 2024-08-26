// Copyright 2024 Jeisson Hidalgo <jeisson.hidalgo@ucr.ac.cr> CC-BY 4.0
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// thread_private_data_t
struct private_data {
  size_t thread_number;  // rank
  size_t thread_count;
};

void* greet(void* data);
int create_threads(size_t thread_count);

// procedure main(argc, argv[])
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  size_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  if (argc == 2) {
    if (sscanf(argv[1], "%zu", &thread_count) != 1) {
      fprintf(stderr, "Error: invalid thread count\n");
      return 11;
    }
  }

  error = create_threads(thread_count);
  return error;
}  // end procedure


int create_threads(size_t thread_count) {
  int error = EXIT_SUCCESS;
  // for thread_number := 0 to thread_count do
  pthread_t* threads = (pthread_t*) malloc(thread_count * sizeof(pthread_t));
  struct private_data* private_data = (struct private_data*)
    calloc(thread_count, sizeof(struct private_data));
  if (threads && private_data) {
    for (size_t thread_number = 0; thread_number < thread_count
        ; ++thread_number) {
      private_data[thread_number].thread_number = thread_number;
      private_data[thread_number].thread_count = thread_count;
      // create_thread(greet, thread_number)
      if (pthread_create(&threads[thread_number], NULL, greet
        , &private_data[thread_number]) != EXIT_SUCCESS) {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = EXIT_FAILURE;
        break;
      }
    }

    // print "Hello from main thread"
    printf("Hello from main thread\n");

    for (size_t thread_number = 0; thread_number < thread_count
        ; ++thread_number) {
      pthread_join(threads[thread_number], NULL);
    }

    free(private_data);
    free(threads);
  } else {
    fprintf(stderr, "Error: could not allocate %zu threads\n", thread_count);
    error = EXIT_FAILURE;
  }

  return error;
}

// procedure greet:
void* greet(void* data) {
  struct private_data* private_data = (struct private_data*) data;
  printf("Hello from secondary thread %zu of %zu\n"
    , (*private_data).thread_number, private_data->thread_count);
  return NULL;
}  // end procedure
