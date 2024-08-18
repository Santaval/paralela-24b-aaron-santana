// Copyright [2025] <Jeisson Hidalgo adaptado por Aaron Santana>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

long readThreadCountArgument(int argc, char** argv);

// procedure main(argc, argv[]) do
int main(int argc, char** argv) {
//     integer thread_count := 0;
long threadCount = readThreadCountArgument(argc, argv);
//     if argc > 1 do
//         thread_count := integer(argv[1]);
//     end
//     else do
//         thread_count := system_cores();
//     end

//     for i := 0 to thread_count do
//         create_thread(greet, i);
//     end
//     print("Hello from main thread\n");
// end
}


// procedure greet(threadNumber) do
//     print("Hello from thread %d\n", threadNumber);
// end

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