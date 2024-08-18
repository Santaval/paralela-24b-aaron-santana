// Copyright [2025] <Jeisson Hidalgo adaptado por Aaron Santana>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

long readThreadCountArgument(int argc, char** argv);
void* greet(void* data);

// procedure main(argc, argv[]) do
int main(int argc, char** argv) {
    //     integer thread_count := 0;
    long threadCount = readThreadCountArgument(argc, argv); 

    //     for i := 0 to thread_count do
    for (long threadIndex = 0; threadIndex < threadCount; ++threadIndex) {
        // create_thread(greet, i);
        pthread_t thread;

        int error = pthread_create(&thread, NULL, greet, &threadIndex);

        if (error == EXIT_FAILURE) {
            fprintf(stderr, "Error: Couldn't create %ld thread \n", threadIndex);
            return EXIT_FAILURE;
        }

        pthread_join(thread, NULL);

    //     end

    }
    //     print("Hello from main thread\n");
    printf("Hello from home astronaut! \n");
    // end
}


// procedure greet(threadNumber) do
void* greet(void* data) {
    long threadNumber = *(long*) data;
    //     print("Hello from thread %d\n", threadNumber);
    printf("Hello from earth %ld astronaut! \n", threadNumber);
    // end

    return NULL;
}

/**
 * Reads the thread count argument from the command line arguments. If the argument is not present, the function
 * returns the number of threads available in the system.
 *
 * @param argc The number of command line arguments.
 * @param argv The array of command line arguments.
 * @return The thread count argument as a long integer.
 */
long readThreadCountArgument(int argc, char** argv) {
    for (int index = 0; index < argc; ++index) {
        if (strcmp(argv[index], "-t" ) == 0 ) {
            if (index == (argc -1)) {
                fprintf(stderr, "Error: -t flag must be followed by a number\n");
                exit(EXIT_FAILURE);
            }
            return (long) strtol(argv[index + 1], NULL, 10);
        }
    }
    return sysconf(_SC_NPROCESSORS_ONLN);
}