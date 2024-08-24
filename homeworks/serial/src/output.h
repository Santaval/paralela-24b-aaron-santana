#pragma once
#include "types.h"
#include <time.h>

/**
 * Writes a matrix to a binary file.
 *
 * @param plate The matrix to be written to the file.
 * @param binaryFilpath The filepath of the binary file.
 */
void writePlate(Plate plate, const char* binaryFilpath);

/**
 * Writes the results of the jobs to a file.
 *
 * @param jobsData The array of JobData containing the job information.
 * @param results The array of SimulationResult containing the simulation results.
 * @param jobsCount The number of jobs.
 * @param filepath The path of the file to write the results to.
 */
void writeJobsResult(JobData* jobsData, SimulationResult* results, size_t jobsCount, const char* filepath);
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
void printPlate(Plate plate);

/**
 * Formats the given time in seconds into a human-readable (YYYY/MM/DD HH:MM:SS) format and stores it in the provided buffer.
 *
 * @param seconds The time in seconds to be formatted.
 * @param buffer The buffer to store the formatted time.
 * @param buffer_size The size of the buffer.
 */
void format_time(time_t seconds, char *buffer, size_t buffer_size);
