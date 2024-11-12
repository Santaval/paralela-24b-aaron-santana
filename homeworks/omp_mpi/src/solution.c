// Copyright <2024> <Aaron Santana Valdelomar - UCR>
#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <mpi.h>

#include "input.h"
#include "solution.h"
#include "output.h"

/**
 * @brief Start program execution.
 *
 * @return Status code to the operating system, 0 means success.
 */
int main(int argc, char** argv) {
    struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  Arguments args = processArguments(argc, argv);
  JobData* jobsData = readJobData(args.jobFile);
  size_t jobsCount = calcFileLinesCount(args.jobFile);
  SimulationResult* results = malloc(jobsCount * sizeof(SimulationResult));
  assert(results != NULL);
  for (size_t i = 0; i < jobsCount; i++) {
    results[i] = processJob(jobsData[i], args);
  }

  writeJobsResult(jobsData, results, jobsCount, "output.txt");

  // free memory
  destroyJobsData(jobsData, jobsCount);
  destroySimulationResult(results, jobsCount);

  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsedTime = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec -
    start.tv_nsec) / 1e6;
  printf("Elapsed time: %.2f ms\n", elapsedTime);

  return EXIT_SUCCESS;
}

SimulationResult processJob(JobData jobData, Arguments args) {
  Plate* plate = readPlate(jobData.plateFile, jobData.directory);
  SimulationResult result = simulate(jobData, plate, args);
  return result;
}

SimulationResult simulate(JobData jobData, Plate* plate, Arguments args) {
  Plate* readPlate = copyPlate(plate);
  Plate* writePlate = plate;
  const size_t totalCells = readPlate->rows * readPlate->cols;
  SharedData* sharedData = malloc(sizeof(SharedData));
  sharedData->readPlate = readPlate;
  sharedData->writePlate = writePlate;
  sharedData->threadCount = args.threadsCount > totalCells ? totalCells
    : args.threadsCount;
  sharedData->jobData = jobData;
  sharedData->totalIterations = 0;
  sharedData->currentCell = 0;

  calcNewTemperature(sharedData, args);



  SimulationResult result;
  result.plate = writePlate;
  result.iterations = sharedData->totalIterations + 1;

  // free memory
  destroyPlate(readPlate);
  free(sharedData);
  return result;
}

void calcNewTemperature(SharedData* sharedData, Arguments args) {

    // MPI_Init(&args.argc, &args.argv);

    const JobData jobData = sharedData->jobData;
    const double factor = (jobData.duration * jobData.thermalDiffusivity) /
                    (jobData.plateCellDimmensions * jobData.plateCellDimmensions);

    while (1) {
        double* currentPlateData = sharedData->readPlate->data;
        double* newPlateData = sharedData->writePlate->data;
        int isBalanced = 1;

        if (sharedData->writePlate->isBalanced == 2) {
            break;
        }

        const size_t rows = sharedData->readPlate->rows;
        const size_t cols = sharedData->readPlate->cols;

        #pragma omp parallel for reduction(&:isBalanced)
        for (size_t row = cols; row < cols * (rows - 1); row += cols) {
            int localIsBalanced = 1;
            for (size_t col = row + 1; col < (cols - 2); ++col) {
                double left = currentPlateData[(row * col) - 1];
                double right = currentPlateData[(row * col) + 1];
                double up = currentPlateData[(row - 1) * col];
                double down = currentPlateData[(row + 1) * col];
                double cell = currentPlateData[row * col];

                double newTemperature = cell + factor * (left + right + up + down - 4 * cell);
                newPlateData[row * col] = newTemperature;

                if (fabs(newTemperature - cell) > jobData.balancePoint) {
                    localIsBalanced = 0;  // No está balanceado
                }
            }
            // Actualiza la variable isBalanced de manera segura
            if (localIsBalanced == 0) {
                isBalanced = 0;
            }
        }

        // Actualizar el estado de balance de la placa
        #pragma omp single
        {
            sharedData->writePlate->isBalanced = isBalanced ? 2 : 1;
        }

        // Intercambiar placas y actualizar iteraciones
        #pragma omp single
        {
            if (!isBalanced) {
                Plate* temp = sharedData->readPlate;
                sharedData->readPlate = sharedData->writePlate;
                sharedData->writePlate = temp;
                sharedData->totalIterations++;
            }
        }

        #pragma omp barrier // Sincronizar los hilos antes de la siguiente iteración
    }

    //MPI_Finalize();

    return;
}


Plate* copyPlate(Plate* plate) {
  Plate* newPlate = malloc(sizeof(Plate));
  newPlate->rows = plate->rows;
  newPlate->cols = plate->cols;
  newPlate->size = plate->size;
  newPlate->isBalanced = plate->isBalanced;
  newPlate->data = malloc(newPlate->size * sizeof(double));
  memcpy(newPlate->data, plate->data, newPlate->size * sizeof(double));
  
  return newPlate;
}


void format_time(time_t seconds, char *buffer, size_t buffer_size) {
    int years, months, days, hours, minutes, secs;

    years = seconds / 31536000;
    seconds -= years * 31536000;
    months = seconds / 2592000;
    seconds -= months * 2592000;
    days = seconds / 86400;
    seconds -= days * 86400;
    hours = seconds / 3600;
    seconds -= hours * 3600;
    minutes = seconds / 60;
    seconds -= minutes * 60;
    secs = seconds;

    // Formatear la cadena en el buffer
    snprintf(buffer, buffer_size, "%02d/%02d/%02d %02d:%02d:%02d", years,
      months, days, hours, minutes, secs);
}

void destroyJobsData(JobData *jobsData, size_t jobsCount) {
    for (size_t i = 0; i < jobsCount; i++) {
        free(jobsData[i].plateFile);
        free(jobsData[i].directory);
    }
    free(jobsData);
}

void destroyPlate(Plate* plate) {
  free(plate->data);
  free(plate);
}

void destroySimulationResult(SimulationResult* results, size_t resultsCount) {
    for (size_t i = 0; i < resultsCount; i++) {
        destroyPlate(results[i].plate);
    }
    free(results);
}

