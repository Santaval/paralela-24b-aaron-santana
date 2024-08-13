// Copyright <2024> <Aaron Santana Valdelomar>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Start program execution.
 *
 * @return Status code to the operating system, 0 means success.
 */
int main(void) {
  double value = 0.0;
  while (scanf("%lf", &value) == 1) {
    // Read the number from the user
    double square = value * value;      // Calculate the square of the number
    printf("%lg:%lg \n", value, square);  // Print the result
  }

  return EXIT_SUCCESS;
}
