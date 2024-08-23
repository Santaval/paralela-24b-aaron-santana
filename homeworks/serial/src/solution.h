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

Plate copyPlate(Plate plate);

Plate simulationIteration(JobData JobData, Plate plate);

bool isPlateBalanced(Plate currentPlate, Plate previousPlate, double balancePoint);
