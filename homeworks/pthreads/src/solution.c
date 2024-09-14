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

#include "input.h"
#include "solution.h"
#include "output.h"

/**
 * @brief Start program execution.
 *
 * @return Status code to the operating system, 0 means success.
 */
int main(int argc, char** argv) {
  Arguments args = processArguments(argc, argv);
  JobData* jobsData = readJobData(args.jobFile);
  size_t jobsCount = calcFileLinesCount(args.jobFile);
  SimulationResult* results = malloc(jobsCount * sizeof(SimulationResult));
  assert(results != NULL);
  for (size_t i = 0; i < jobsCount; i++) {
    results[i] = processJob(jobsData[i]);
  }

  writeJobsResult(jobsData, results, jobsCount, "output.txt");

  // free memory
  destroyJobsData(jobsData, jobsCount);
  destroySimulationResult(results, jobsCount);

  return EXIT_SUCCESS;
}

SimulationResult processJob(JobData jobData) {
  Plate plate = readPlate(jobData.plateFile, jobData.directory);
  SimulationResult result = simulate(jobData, plate);
  return result;
}

SimulationResult simulate(JobData jobData, Plate plate) {
  Plate previousPlate = plate;
  printPlate(previousPlate);
  Plate currentPlate;
  currentPlate.rows = previousPlate.rows;
  currentPlate.cols = previousPlate.cols;
  currentPlate.isBalanced = 0;
  currentPlate.data = malloc(currentPlate.rows * sizeof(double*));
  for (size_t i = 0; i < currentPlate.rows; i++) {
    currentPlate.data[i] = malloc(currentPlate.cols * sizeof(double));
  }


  copyPlateBorders(previousPlate, currentPlate);
  printPlate(currentPlate);

  size_t iterationsCount = 0;
  do {
    printf("Iteration %zu\n", iterationsCount);
    // if (iterationsCount > 0) {
    //   destroyPlate(previousPlate);
    // }
    previousPlate = currentPlate;
    currentPlate = simulationIteration(jobData, previousPlate);
    
    iterationsCount++;
  } while (currentPlate.isBalanced);

  // free memory
  destroyPlate(previousPlate);
  SimulationResult result;
  result.plate = currentPlate;
  result.iterations = iterationsCount;
  return result;
}

Plate simulationIteration(JobData jobData, Plate plate) {

  Plate newPlate;
  newPlate.rows = plate.rows;
  newPlate.cols = plate.cols;
  newPlate.isBalanced = 1;
  newPlate.data = malloc(newPlate.rows * sizeof(double*));
  for (size_t i = 0; i < newPlate.rows; i++) {
    newPlate.data[i] = malloc(newPlate.cols * sizeof(double));
  }

  copyPlateBorders(plate, newPlate);

  const size_t STATIC_THREAD_COUNT = 10;

  SharedData* sharedData = malloc(sizeof(SharedData));
  sharedData->currentPlate = plate;
  sharedData->newPlate = newPlate;
  sharedData->threadCount = STATIC_THREAD_COUNT > plate.rows * plate.cols ? plate.rows * plate.cols : STATIC_THREAD_COUNT;
  sharedData->jobData = jobData;

  pthread_mutex_init(&sharedData->can_accsess_isBalanced, NULL);
  // start time 
  struct private_data* team = create_threads(sharedData->threadCount, calcNewTemperature, sharedData);


  join_threads(sharedData->threadCount, team);

  
  pthread_mutex_destroy(&sharedData->can_accsess_isBalanced);
  Plate resultPlate = sharedData->newPlate;
  free(sharedData);



  return resultPlate;
}

void* calcNewTemperature(void* data) {
    struct private_data* privateData = (struct private_data*)data;
    size_t cellNumber = privateData->thread_number;
    SharedData* sharedData = (SharedData*) privateData->data;

    size_t totalCells = sharedData->currentPlate.rows * sharedData->currentPlate.cols;
    size_t threadCount = privateData->thread_count;
    size_t cellsProcessed = 0;

    JobData jobData = sharedData->jobData;
    double factor = (jobData.duration * jobData.thermalDiffusivity) / 
                    (jobData.plateCellDimmensions * jobData.plateCellDimmensions);
    
    int localIsBalanced = 1;
    
    double** currentPlateData = sharedData->currentPlate.data;
    double** newPlateData = sharedData->newPlate.data;
    size_t rows = sharedData->currentPlate.rows;
    size_t cols = sharedData->currentPlate.cols;

    while (cellNumber < totalCells) {
        size_t row = cellNumber / cols;
        size_t col = cellNumber % cols;
        if (row > 0 && row < rows - 1 && col > 0 && col < cols - 1) {
            double left = currentPlateData[row][col - 1];
            double right = currentPlateData[row][col + 1];
            double up = currentPlateData[row - 1][col];
            double down = currentPlateData[row + 1][col];
            double cell = currentPlateData[row][col];

            double newTemperature = cell + factor * (left + right + up + down - 4 * cell);
            newPlateData[row][col] = newTemperature;

            if (fabs(newTemperature - cell) > jobData.balancePoint) {
                localIsBalanced = 0;  // Se detecta que no está balanceado
            }
        }

        cellNumber += threadCount;
        cellsProcessed++;
    }

    if (!localIsBalanced) {
        pthread_mutex_lock(&sharedData->can_accsess_isBalanced);
        sharedData->newPlate.isBalanced = 0;
        pthread_mutex_unlock(&sharedData->can_accsess_isBalanced);
    }
    return NULL;
}



void copyPlateBorders(Plate original, Plate copy) {
  // top
  for (size_t colIndex = 0; colIndex < original.cols; colIndex++) {
    copy.data[0][colIndex] = original.data[0][colIndex];
  }

  // left
  for (size_t rowIndex = 0; rowIndex < original.rows; rowIndex++) {
    copy.data[rowIndex][0] = original.data[rowIndex][0];
  }

  // right
  for (size_t rowIndex = 0; rowIndex < original.rows; rowIndex++) {
    copy.data[rowIndex][original.cols - 1]
      = original.data[rowIndex][original.cols - 1];
  }

  // bottom
  for (size_t colIndex = 0; colIndex < original.cols; colIndex++) {
    copy.data[original.rows - 1][colIndex]
      = original.data[original.rows - 1][colIndex];
  }
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

void destroyPlate(Plate plate) {
  for (size_t i = 0; i < plate.rows; i++) {
    free(plate.data[i]);
  }
  free(plate.data);
}

void destroySimulationResult(SimulationResult* results, size_t resultsCount) {
    for (size_t i = 0; i < resultsCount; i++) {
        destroyPlate(results[i].plate);
    }
    free(results);
}

struct private_data* create_threads(size_t thread_count, void* (*routine)(void* data), void* data) {
  // for thread_number := 0 to thread_count do
  struct private_data* team = (struct private_data*)
    calloc(thread_count, sizeof(struct private_data));
  if (team) {
    for (size_t thread_number = 0; thread_number < thread_count
        ; ++thread_number) {
      team[thread_number].thread_number = thread_number;
      team[thread_number].thread_count = thread_count;
      team[thread_number].data = data;
      // create_thread(routine, thread_number)
      if (pthread_create(&team[thread_number].thread_id, NULL, routine
        , &team[thread_number]) != EXIT_SUCCESS) {
        fprintf(stderr, "Error: could not create secondary thread\n");
        join_threads(thread_number, team);
        return NULL;
      }
    }
  } else {
    fprintf(stderr, "Error: could not allocate %zu threads\n", thread_count);
    return NULL;
  }

  return team;
}

int join_threads(const size_t thread_count, struct private_data* team) {
  int result = EXIT_SUCCESS;
for (size_t thread_number = 0; thread_number < thread_count; ++thread_number) {
    int error = pthread_join(team[thread_number].thread_id, NULL);
    if (result == EXIT_SUCCESS) {
        result = error;
    }
}

    free(team);
    return result;
}
