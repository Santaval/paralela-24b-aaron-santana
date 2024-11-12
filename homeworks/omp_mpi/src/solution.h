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
 * @param args The arguments for the simulation.
 * @return The result of the job processing.
 */
SimulationResult processJob(JobData jobData);

/**
 * Simulates the given job data on the specified plate.
 *
 * @param jobData The job data to be simulated.
 * @param plate The plate on which the simulation will be performed.
 * @param args The arguments for the simulation.
 * @return The result of the simulation.
 */
SimulationResult simulate(JobData jobData, Plate* plate);

/**
 * @brief Creates a copy of a Plate object.
 *
 * This function creates a deep copy of the given Plate object and returns a pointer to the newly created copy.
 *
 * @param plate The Plate object to be copied.
 * @return A pointer to the newly created copy of the Plate object.
 */
Plate* copyPlate(Plate* plate);

/**
 * @brief Copies the borders of the original plate to the copy plate.
 *
 * This function copies the borders of the original plate to the copy plate.
 * The original plate and the copy plate must have the same dimensions.
 *
 * @param original The original plate.
 * @param copy The copy plate.
 */
void copyPlateBorders(Plate original, Plate copy);



/**
 * Calculates the new temperature based on the given data.
 *
 * @param data The data needed to calculate the new temperature.
 * @return A pointer to the calculated new temperature.
 */
void calcNewTemperature(SharedData* sharedData);


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
void destroyPlate(Plate* plate);
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


/**
 * @brief Creates multiple threads and assigns a routine to each thread.
 *
 * This function creates a specified number of threads and assigns a routine to each thread.
 * The routine is responsible for executing a specific task using the provided data.
 *
 * @param thread_count The number of threads to create.
 * @param routine The routine function to be executed by each thread.
 * @param data The data to be passed to each thread's routine function.
 * @return A pointer to the private data structure containing information about the created threads.
 */
struct private_data* create_threads(size_t thread_count,
    void* (*routine)(void* data), void* data);
/**
 * @brief Joins a specified number of threads and releases the resources associated with them.
 *
 * This function joins a specified number of threads and releases the resources associated with them.
 *
 * @param thread_count The number of threads to join.
 * @param team The private data structure for the team of threads.
 * @return Returns an integer indicating the success or failure of the operation.
 */
int join_threads(const size_t thread_count, struct private_data* team);


void sendJobData(JobData* jobData, int dest);
void receiveJobData(JobData* jobData, int source);

void sendJobResult(SimulationResult* result, int dest);
void receiveJobResult(SimulationResult* result, int source, int* sourceCb);