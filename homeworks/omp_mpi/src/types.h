// Copyright <2024> <Aaron Santana Valdelomar - UCR>
#pragma once
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/**
 * @brief Structure representing a plate with data, number of rows, and number of columns.
 */

typedef struct  {
    double* data;  /// < matrix of doubles representing the plate
    short isBalanced;  /// < indicates if the plate is balanced
    size_t rows;  /// < number of rows in the plate
    size_t cols;  /// < number of columns in the plate
    size_t size; /// < number of cells (elements) in the plate
} Plate;

/**
 * @struct Arguments
 * @brief Represents the arguments of the program such as the job file path and the number of threads.
 * 
 * This struct contains the job file path and the number of threads to be used.
 */
typedef struct {
    char* jobFile;  /// < path to the job file
    size_t threadsCount;  /// < number of threads to be used
    short isVerbose;  /// < indicates if the program should print verbose output
    short shloudPrintIterations;  /// < indicates if the program
    int argc;  /// < number of arguments
    char** argv;  /// < arguments
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
    char* plateFile;  /// < path to the file containing the plate data
    double duration;  /// < duration of each iteration in the simulation
    double thermalDiffusivity;  /// < thermal diffusivity of the plate
    double plateCellDimmensions;  /// < dimensions of the plate cells
    double balancePoint;  /// < balance point of the plate
    char* directory;  /// < directory where the results will be written
} JobData;

/**
 * @brief Structure representing the result of a simulation.
 * 
 * This structure contains a 2D array representing a plate and the number of iterations performed in the simulation.
 */
typedef struct {
    Plate* plate;  /// < plate resulting from the simulation
    size_t iterations;  /// < number of iterations performed in the simulation
} SimulationResult;

/**
 * @struct SharedDate
 * @brief Structure representing shared data for threads.
 *
 * This structure holds information that is shared among multiple threads.
 * It contains the number of threads, the current plate, and the new plate.
 */
typedef struct {
    size_t threadCount;  /// < number of threads
    Plate* readPlate;  /// < current plate
    Plate* writePlate;  /// < new plate
    JobData jobData;  /// < job data
    size_t totalIterations;  /// < total number of iterations
    size_t currentCell;  /// < current cell being processed
} SharedData;

