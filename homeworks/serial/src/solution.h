// Copyright [2024] <Aaron Santana Valdelomar>

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include <time.h>

/**
 * @brief Processes a job using the provided job data.
 *
 * This function takes a `JobData` object as input and processes the job accordingly.
 * It returns a `SimulationResult` object that represents the result of the job processing.
 *
 * @param jobData The job data to be processed.
 * @return The result of the job processing.
 */
SimulationResult processJob(JobData jobData);

/**
 * Reads a matrix from a binary file that represents a plate.
 *
 * @param binaryFilpath The filepath of the binary file.
 * @return A double pointer to the matrix read from the file.
 */
Plate readPlate(const char* binaryFilpath);

/**
 * Writes a matrix to a binary file.
 *
 * @param plate The matrix to be written to the file.
 * @param binaryFilpath The filepath of the binary file.
 */
void writePlate(Plate plate, const char* binaryFilpath);

/**
 * Simulates the given job data on the specified plate.
 *
 * @param jobData The job data to be simulated.
 * @param plate The plate on which the simulation will be performed.
 * @return The result of the simulation.
 */
SimulationResult simulate(JobData jobData, Plate plate);

Plate copyPlate(Plate plate);

Plate simulationIteration(JobData JobData, Plate plate);

bool isPlateBalanced(Plate currentPlate, Plate previousPlate, double balancePoint);

void printPlate(Plate plate);

void format_time(time_t seconds, char *buffer, size_t buffer_size);