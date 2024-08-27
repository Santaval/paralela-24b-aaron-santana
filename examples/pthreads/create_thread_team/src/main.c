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
  void* data;
 pthread_t thread_id;
};

void* greet(void* data);
struct private_data*  create_threads(size_t thread_count, void* (*routine)(void* data), void* data);
int join_threads(size_t thread_count, struct private_data* team);

// procedure main(argc, argv[])
int main(int argc, char* argv[]) {
  struct private_data* team = NULL;
  size_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  if (argc == 2) {
    if (sscanf(argv[1], "%zu", &thread_count) != 1) {
      fprintf(stderr, "Error: invalid thread count\n");
      return EXIT_FAILURE;
    }
  }

  team = create_threads(thread_count, greet, NULL);
  if (team) {
  // print "Hello from main thread"
  printf("Hello from main thread\n");
  join_threads(thread_count, team);

  } else {
    fprintf(stderr, "Error: creating threads team\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}  // end procedure


struct private_data* create_threads(size_t thread_count, void* (*routine)(void* data), void* data) {
  // for thread_number := 0 to thread_count do
  struct private_data* team = (struct private_data*)
    calloc(thread_count, sizeof(struct private_data));
  if (team) {
    for (size_t thread_number = 0; thread_number < thread_count
        ; ++thread_number) {
      team[thread_number].thread_number = thread_number;
      team[thread_number].thread_count = thread_count;
      team[thread_number].data = data;
      // create_thread(routine, thread_number)
      if (pthread_create(&team[thread_number].thread_id, NULL, routine
        , &team[thread_number]) != EXIT_SUCCESS) {
        fprintf(stderr, "Error: could not create secondary thread\n");
        join_threads(thread_number, team);
        return NULL;
      }
    }
  } else {
    fprintf(stderr, "Error: could not allocate %zu threads\n", thread_count);
    return NULL;
  }

  return team;
}

int join_threads(const size_t thread_count, struct private_data* team) {
  int result = EXIT_SUCCESS;
  for (size_t thread_number = 0; thread_number < thread_count
        ; ++thread_number) {
      int error = pthread_join(team[thread_number].thread_id, NULL);
      if (result == EXIT_SUCCESS) {
       result = error;
      }
    }
    free(team);
    return result;
}

// procedure greet:
void* greet(void* data) {
  struct private_data* private_data = (struct private_data*) data;
  printf("Hello from secondary thread %zu of %zu\n"
    , (*private_data).thread_number, private_data->thread_count);
  return NULL;
}  // end procedure
