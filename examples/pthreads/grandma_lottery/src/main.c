// Copyright 2024 <You>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void* buyLottery(void* data);

// procedure main:
int main(void) {
  // create thwo grandchild threads who buy lottery tickets
  pthread_t grandchild1, grandchild2;
  int error = pthread_create(&grandchild1, /*attr*/ NULL,
      buyLottery, /*arg*/ NULL);
  int error2 = pthread_create(&grandchild2, /*attr*/ NULL,
      buyLottery, /*arg*/ NULL);
  if (error == EXIT_FAILURE || error2 == EXIT_FAILURE) {
    printf("Error creating thread\n");
    return EXIT_FAILURE;
  }

  // Wait for the threads to finish
  void* number1 = NULL;
  void* number2 = NULL;
  pthread_join(grandchild1, /*value_ptr*/ &number1);
  pthread_join(grandchild2, /*value_ptr*/ &number2);

  // report the numbers that the grandchild threads got
  printf("Grandchild 1 got number %zu\n", (size_t) number1);
  printf("Grandchild 2 got number %zu\n", (size_t) number2);

  return EXIT_SUCCESS;
}  // end procedure

// procedure buyLottery:
void* buyLottery(void* data) {
  (void) data;
  unsigned int mySeed = time(NULL) + clock();
  size_t number = rand_r(&mySeed) % 100;
  //  return &number;
  return (void*)number;
}  // end procedure
