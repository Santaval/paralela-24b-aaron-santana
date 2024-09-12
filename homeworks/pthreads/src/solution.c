// Copyright <2024> <Aaron Santana Valdelomar - UCR>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "input.h"
#include "solution.h"
#include "output.h"
#include "ethreads/ethread.h"


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
  Plate currentPlate = plate;
  size_t iterationsCount = 0;

  do {
    if (iterationsCount > 0) {
      destroyPlate(previousPlate);
    }
    previousPlate = currentPlate;
    currentPlate = simulationIteration(jobData, previousPlate);
    iterationsCount++;
  } while (!currentPlate.isBalanced);

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

  // Allocate memory for the new plate
  newPlate.data = (double **)malloc(plate.rows * sizeof(double *));
  for (size_t i = 0; i < plate.rows; i++) {
    newPlate.data[i] = (double *)malloc(plate.cols * sizeof(double));
  }
  copyPlateBorders(plate, newPlate);

  struct thread_team* team = create_threads(4, calcNewTemperature, NULL); 
  
  if (team) {
    printf("Threads created\n");
  }

  for (size_t i = 1; i < plate.rows - 1; i++) {
    for (size_t j = 1; j < plate.cols - 1; j++) {
      calcNewTemperature(plate, newPlate, jobData, i, j);
    }
  }
  return newPlate;
}

void calcNewTemperature(Plate currentPlate, Plate newPlate, JobData jobData, size_t currentCellRow, size_t currentCellCol) {
  double left = currentPlate.data[currentCellRow][currentCellCol - 1];
      double right = currentPlate.data[currentCellRow][currentCellCol + 1];
      double up = currentPlate.data[currentCellRow - 1][currentCellCol];
      double down = currentPlate.data[currentCellRow + 1][currentCellCol];
      double cell = currentPlate.data[currentCellRow][currentCellCol];
      double newTemperature = cell + ((jobData.duration * jobData
      .thermalDiffusivity) / (jobData.plateCellDimmensions *
        jobData.plateCellDimmensions)) * (left + right + up + down - 4 * cell);
      newPlate.data[currentCellRow][currentCellCol] = newTemperature;
      if ((newTemperature - cell) > jobData.balancePoint) {
        newPlate.isBalanced = 0;
      }
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
