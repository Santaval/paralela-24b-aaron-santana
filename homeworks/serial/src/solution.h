// Copyright [2024] <Aaron Santana Valdelomar>

#pragma once
#include <stdio.h>
#include <stdlib.h>

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
    double** plate;
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
 * Reads a matrix from a binary file.
 *
 * @param binaryFilpath The filepath of the binary file.
 * @return A double pointer to the matrix read from the file.
 */
double** readMatrix(const char* binaryFilpath);