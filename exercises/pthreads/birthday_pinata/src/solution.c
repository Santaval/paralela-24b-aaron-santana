// Copyright <2024> <Aaron Santana Valdelomar>
#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/**
 * @brief Structure to hold shared data for the birthday pinata program.
 * 
 * This structure contains the position of the thread and the number of threads that will be created.
 * It also includes a mutex to synchronize access to the pinata.
 */
typedef struct shared_data {
    uint64_t pinata_hits_remaining; /**< Pinata hits after brake */
    pthread_mutex_t can_hit_pinata; /**< If thread can hit pinata */
    uint64_t thread_count; /**< Threads total */
} shared_data_t;


/**
 * @brief Structure representing private data for each thread.
 * 
 * This structure contains the thread number (rank) and a pointer to the shared data.
 * The thread number is used to store the rank of the thread, while the shared data
 * pointer is used to store the shared data between the threads.
 */
typedef struct private_data {
    uint64_t thread_number;  /**< The rank of the thread. */
    shared_data_t* shared_data; /**< Pointer to the shared data. */
} private_data_t;

/**
 * @brief ...
 */
void* partyGuest(void* data);
int create_threads(shared_data_t* shared_data);

// procedure main(argc, argv[])
int main(int argc, char* argv[]) {
  int error = EXIT_SUCCESS;
  // create thread_count as result of converting argv[1] to integer
  // thread_count := integer(argv[1])
  uint64_t thread_count = sysconf(_SC_NPROCESSORS_ONLN);
  uint64_t pinata_hardness = 0;
  if (argc == 3) {
    if (sscanf(argv[1], "%" SCNu64, &thread_count) == 1) {
    } else {
      fprintf(stderr, "Error: invalid thread count\n");
      return EXIT_FAILURE;
    }

    if (sscanf(argv[2], "%" SCNu64, &pinata_hardness) == 1) {
    } else {
      fprintf(stderr, "Error: invalid pinata hardness\n");
      return EXIT_FAILURE;
    }
  } else {
    fprintf(stderr, "Usage: %s <thread_count> <pinata_hardness>\n", argv[0]);
    return EXIT_FAILURE;
  }
    // init the shared data
  shared_data_t* shared_data = (shared_data_t*)calloc(1, sizeof(shared_data_t));
  if (shared_data) {
    shared_data->pinata_hits_remaining = pinata_hardness;

    error = pthread_mutex_init(&shared_data->can_hit_pinata, /*attr*/NULL);
    if (error == EXIT_SUCCESS) {
      shared_data->thread_count = thread_count;

      error = create_threads(shared_data);
      // finally, destroy the mutex and free the shared data
      pthread_mutex_destroy(&shared_data->can_hit_pinata);
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
      error = pthread_create(&threads[thread_number], /*attr*/ NULL, partyGuest
        , /*arg*/ &private_data[thread_number]);
      if (error == EXIT_SUCCESS) {
      } else {
        fprintf(stderr, "Error: could not create secondary thread\n");
        error = 21;
        break;
      }
    }

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

// procedure partyGuest(thread_number) do
void* partyGuest(void* data) {
    private_data_t* private_data = (private_data_t*) data;
    shared_data_t* shared_data = private_data->shared_data;
    uint64_t my_hits = 0;
    short did_I_broke_pinata = 0;

    while (1) {
        pthread_mutex_lock(&shared_data->can_hit_pinata);
        if (shared_data->pinata_hits_remaining > 0) {
            --shared_data->pinata_hits_remaining;
            ++my_hits;
            if (shared_data->pinata_hits_remaining == 0) {
                did_I_broke_pinata = 1;
            }
        } else {
            pthread_mutex_unlock(&shared_data->can_hit_pinata);
            break;
        }

        pthread_mutex_unlock(&shared_data->can_hit_pinata);
    }

    if (did_I_broke_pinata) {
        printf("I'm thread %" PRIu64 "/%" PRIu64 " I hit %" PRIu64
            " times and I broke the pinata!\n", private_data->
            thread_number, shared_data->thread_count, my_hits);
    } else {
        printf("I'm thread %" PRIu64 "/%" PRIu64 " I hit %"
        PRIu64 " times\n", private_data->thread_number, shared_data->
        thread_count, my_hits);
    }
    return NULL;
}  // end procedure
