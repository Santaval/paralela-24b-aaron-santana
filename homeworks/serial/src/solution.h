// Copyright [2024] <Aaron Santana Valdelomar>

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "types.h"

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
 * Simulates the given job data on the specified plate.
 *
 * @param jobData The job data to be simulated.
 * @param plate The plate on which the simulation will be performed.
 * @return The result of the simulation.
 */
SimulationResult simulate(JobData jobData, Plate plate);

/**
 * Creates a copy of the given Plate object.
 *
 * @param plate The Plate object to be copied.
 * @return A new Plate object that is a copy of the original plate.
 */
Plate copyPlate(Plate plate);

/**
 * Simulates a single iteration of the plate simulation.
 *
 * @param JobData The job data containing the necessary information for the simulation.
 * @param plate The plate object representing the current state of the simulation.
 * @return The updated plate object after the simulation iteration.
 */
Plate simulationIteration(JobData JobData, Plate plate);

/**
 * Checks if a plate is balanced based on the current plate, previous plate, and balance point.
 *
 * @param currentPlate The current plate to check.
 * @param previousPlate The previous plate to compare with.
 * @param balancePoint The balance point to determine if the plate is balanced.
 * @return True if the plate is balanced, false otherwise.
 */
bool isPlateBalanced(Plate currentPlate, Plate previousPlate,
    double balancePoint);

/**
 * @brief Destroys the JobData array and frees the memory.
 *
 * This function is responsible for deallocating the memory used by the JobData array.
 * It takes a pointer to the JobData array and the number of jobs as parameters.
 * The function frees the memory allocated for each JobData element and then frees the memory
 * allocated for the JobData array itself.
 *
 * @param jobsData Pointer to the JobData array.
 * @param jobsCount Number of jobs in the array.
 */
void destroyJobsData(JobData *jobsData, size_t jobsCount);
/**
 * @brief Destroys a Plate object.
 *
 * This function is responsible for deallocating the memory used by a Plate object.
 *
 * @param plate The Plate object to be destroyed.
 */
void destroyPlate(Plate plate);
/**
 * @brief Destroys the SimulationResult objects and frees the memory allocated for them.
 *
 * This function takes an array of SimulationResult objects and the number of results in the array.
 * It deallocates the memory used by each SimulationResult object and frees the memory for the array itself.
 *
 * @param results The array of SimulationResult objects to be destroyed.
 * @param resultsCount The number of SimulationResult objects in the array.
 */
void destroySimulationResult(SimulationResult* results, size_t resultsCount);
