// Copyright <2024> <Aaron Santana Valdelomar - UCR>
#pragma once
#include "types.h"

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
 * Reads a matrix from a binary file that represents a plate.
 *
 * @param binaryFilpath The filepath of the binary file.
 * @return A double pointer to the matrix read from the file.
 */
Plate readPlate(const char* binaryFilpath);
