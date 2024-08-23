// Copyright [2025] <Jeisson Hidalgo adaptado por Aaron Santana>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Function to greet "astronauts" :)  recursively.
 *
 * This function takes a pointer to a number and greets the astronauts recursively.
 * If the number is 0, it prints "Goodbye astronaut" followed by the number.
 * Otherwise, it prints "Hello astronaut" followed by the number and creates a secondary thread to greet the next astronaut.
 * The function waits for the secondary thread to finish using pthread_join.
 *
 * @param data A pointer to the number of the astronaut.
 * @return NULL
 */
void* greet(void* data);

// procedure main do
/**
 * @brief Function that represents the main thread work
 *
 * This function is called by the main thread to perform its work, create a secondary thread and wait for it to finish.
 *
 * @return The exit status of the program.
 */
int main(void) {
    // create_thread(greet);
    pthread_t thread;  // create another thread to run greet function

    // allocate memory for the initial number
    size_t* initial_num = malloc(sizeof(size_t));

    *initial_num = 2;  // set the initial number

    /* create a thread and check if it was created successfully, if not, keep an EXIT_FAILURE in
    the error variable.Thread creation function takes 4 parameters: the thread, the attributes, the
    function to run and the data to pass to the function. */
    int error = pthread_create(&thread, NULL, greet, initial_num);

    /* check if the thread was created successfully and print a message in
    the console */
    if (error == EXIT_SUCCESS) {
        // print("Hello from main thread\n");
        printf("Hello from main thread\n");
    } else {
        /* if the secondary thread was not created successfully, print an error message
        in the error output */
        fprintf(stderr, "Error: Couldn't create secondary thread");
        // return that the program failed
        return EXIT_FAILURE;
    }

    /* this function waits for the thread to finish its work, if it does not finish, it will wait indefinitely.
    Takes two parameters: the thread and the data returned by the thread function. It is necessary if the main
    thread finishes before the secondary thread starts. */
    pthread_join(thread, NULL);


    return EXIT_SUCCESS;
    // end
}

// procedure greet do
void* greet(void* data) {
    size_t number = *(size_t*)data;  // get the number of the astronaut

    free(data);  // free the memory allocated for the number

    // print a goodbye message if the number is 0
    if (number == 0) {
        printf("Goodbye astronaut %zu\n", number);
    } else {
        // print a hello message if the number is not 0
        printf("Hello astronaut #%zu\n", number);
        pthread_t thread;  // create another thread to run greet function

        // allocate memory for the next number
        size_t* next_number = malloc(sizeof(size_t));

        // check if the memory was allocated successfully
        if (next_number == NULL) {
            fprintf(stderr, "Error: Couldn't allocate memory for next number");
            return NULL;
        }

        *next_number = number - 1;  // set the next number

        int error = pthread_create(&thread, NULL, greet, next_number);
        // check if the thread was created successfully
        if (error == EXIT_FAILURE) {
            fprintf(stderr, "Error: Couldn't create secondary thread");
            return NULL;
        }
        // wait for the thread to finish its wor
        pthread_join(thread, NULL);
    }
    return NULL;
}
