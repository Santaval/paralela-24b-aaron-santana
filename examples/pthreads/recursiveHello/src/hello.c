// Copyright [2025] <Jeisson Hidalgo adaptado por Aaron Santana>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// procedure greet do
/**
 * @brief Function that represents secondary thread work
 *
 * This function is called by the secondary thread to perform its work, just print a message in the console.
 *
 * @param data A pointer to the data passed to the thread function.
 * @return A pointer to the data returned by the thread function.
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

    /* create a thread and check if it was created successfully, if not, keep an EXIT_FAILURE in
    the error variable.Thread creation function takes 4 parameters: the thread, the attributes, the
    function to run and the data to pass to the function. */
    int error = pthread_create(&thread, NULL, greet, NULL);

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
    // print("Hello from secondary thread\n");
    printf("Hello from secondary thread\n");
    // end
    return NULL;
}
