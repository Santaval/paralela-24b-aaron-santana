// Copyright <2024> <Aaron Santana Valdelomar - UCR>
#pragma once
#include <stdio.h>
#include <stdlib.h>

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
