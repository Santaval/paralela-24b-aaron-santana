// Copyright <2024> <Aaron Santana Valdelomar - UCR>
#pragma once
#include <time.h>
#include "types.h"


/**
 * Writes the results of the jobs to a file.
 *
 * @param jobsData The array of JobData containing the job information.
 * @param results The array of SimulationResult containing the simulation results.
 * @param jobsCount The number of jobs.
 * @param filepath The path of the file to write the results to.
 */
void writeJobsResult(JobData* jobsData, SimulationResult* results,
  size_t jobsCount, char* filepath);
/**
 * Writes the result of a job to a file.
 *
 * @param jobData The data of the job.
 * @param result The simulation result.
 * @param file The file to write the result to.
 */
void writeJobResult(JobData jobData, SimulationResult result, FILE* file);

/**
 * Prints the palte matrix to the console.
 *
 * @param plate The matrix to be printed.
 */
void printPlate(Plate* plate);

/**
 * Writes the contents of a Plate structure to a binary file.
 *
 * @param plate The Plate structure to be written.
 * @param binaryFilepath The filepath of the binary file to write to.
 */
void writePlate(Plate* plate, const char* binaryFilepath);

/**
 * Formats the given time in seconds into a human-readable (YYYY/MM/DD HH:MM:SS) format and stores it in the provided buffer.
 *
 * @param seconds The time in seconds to be formatted.
 * @param buffer The buffer to store the formatted time.
 * @param buffer_size The size of the buffer.
 */
void format_time(time_t seconds, char *buffer, size_t buffer_size);

/**
 * @brief Removes the file extension from the given filepath.
 *
 * This function modifies the input `filepath` string by removing the file extension.
 *
 * @param filepath The filepath from which to remove the extension.
 */
void removeExtension(char *filepath);


/**
 * Extracts numbers from a file and stores them in a character array.
 *
 * @param filename The name of the file to extract numbers from.
 * @param numbers  The character array to store the extracted numbers.
 */
void extractNumbers(const char *filename, char *numbers);
