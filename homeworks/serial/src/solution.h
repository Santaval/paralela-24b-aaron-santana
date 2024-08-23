// Copyright [2024] <Aaron Santana Valdelomar>

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/**
 * @brief Structure representing a plate with data, number of rows, and number of columns.
 */

typedef struct  {
    double** data;
    size_t rows;
    size_t cols;
} Plate;

/**
 * @struct Arguments
 * @brief Represents the arguments of the program such as the job file path and the number of threads.
 * 
 * This struct contains the job file path and the number of threads to be used.
 */
typedef struct {
    char* jobFile;
    size_t threadsCount;
} Arguments;

/**
 * @struct JobData
 * @brief Represents the data for a job.
 * 
 * This struct contains the necessary data for a job, including the file path of the plate, 
 * the duration of the simulation, the thermal diffusivity, the dimensions of the plate cells, 
 * and the balance point.
 */
typedef struct {
    char* plateFile;
    double duration;
    double thermalDiffusivity;
    double plateCellDimmensions;
    double balancePoint;
} JobData;

/**
 * @brief Structure representing the result of a simulation.
 * 
 * This structure contains a 2D array representing a plate and the number of iterations performed in the simulation.
 */
typedef struct {
    Plate plate;
    size_t iterations;
} SimulationResult;


/**
 * @brief Process the command line arguments.
 *
 * This function takes the command line arguments and processes them to extract the necessary information.
 *
 * @param argc The number of command line arguments.
 * @param argv An array of strings containing the command line arguments.
 * @return The processed arguments.
 */
Arguments processArguments(int argc, char** argv);



/**
 * Reads job data from a specified job file.
 *
 * @param jobFile The path to the job file.
 * @return A pointer to the JobData structure containing the read job data.
 */
JobData* readJobData(const char* jobFile);
/**
 * Calculates the number of lines in a file.
 *
 * @param filePath The path to the file.
 * @return The number of lines in the file.
 */
size_t calcFileLinesCount(const char* filePath);


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