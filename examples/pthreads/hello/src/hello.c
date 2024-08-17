// Copyright [2025] <Jeisson Hidalgo adaptado por Aaron Santana>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// procedure greet do
void* greet(void* data);

// procedure main do
int main(void) {
    // create_thread(greet);
    pthread_t thread;

    int error = pthread_create(&thread, NULL,
    greet, NULL);

    if (error == EXIT_SUCCESS) {
        // print("Hello from main thread\n");
        printf("Hello from main thread\n");
    } else {
        fprintf(stderr, "Error: Couldn't create secondary thread");
        return EXIT_FAILURE;
    }


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