#pragma once
#include "types.h"
#include <time.h>

/**
 * Writes a matrix to a binary file.
 *
 * @param plate The matrix to be written to the file.
 * @param binaryFilpath The filepath of the binary file.
 */
void writePlate(Plate plate, const char* binaryFilpath);

void writeJobData(JobData jobData, SimulationResult result, const char* filepath);

/**
 * Prints the palte matrix to the console.
 *
 * @param plate The matrix to be printed.
 */
void printPlate(Plate plate);

void format_time(time_t seconds, char *buffer, size_t buffer_size);
