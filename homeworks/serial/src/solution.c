// Copyright <year> <You>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "solution.h"
#include <time.h>


/**
 * @brief Start program execution.
 *
 * @return Status code to the operating system, 0 means success.
 */
int main(int argc, char** argv) {
  Arguments args = processArguments(argc, argv);
  JobData* jobData = readJobData(args.jobFile);
  size_t jobsCount = calcFileLinesCount(args.jobFile);
  SimulationResult* results = malloc(jobsCount * sizeof(SimulationResult));
  assert(results != NULL);

  for (size_t i = 0; i < jobsCount; i++) {
    results[i] = processJob(jobData[i]);
  }

  for (size_t i = 0; i < jobsCount; i++) {
    printPlate(results[i].plate);
    printf("Iterations: %zu\n", results[i].iterations);
    const time_t seconds = results[i].iterations * jobData[i].duration;
    char formatted_time[48];
    format_time(seconds, formatted_time, 48);
    printf("Duration: %lf\n", jobData[i].duration);
    printf("Time: %ld\n", seconds);
    printf("Time: %s\n", formatted_time);
    
  }

  free(jobData);
  return EXIT_SUCCESS;
}


Arguments processArguments(int argc, char** argv) {
  const unsigned AGUMENTS_COUNT = 3;  // 3 arguments are expected
  Arguments args;
  // Check if the number of arguments is correct
  if (argc != AGUMENTS_COUNT) {
    fprintf(stderr, "Usage: %s <jobFile> <threadsCount>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  args.jobFile = argv[1];
  args.threadsCount = atoi(argv[2]);
  return args;
}

JobData* readJobData(const char* jobFile) {
  FILE* file = fopen(jobFile, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", jobFile);
    exit(EXIT_FAILURE);
  }
  size_t jobs = calcFileLinesCount(jobFile);
  JobData* jobData = malloc(jobs * sizeof(JobData));

  assert(jobData != NULL);
  for (size_t i = 0; i < jobs; i++) {
    jobData[i].plateFile = malloc(100 * sizeof(char));
    fscanf(file, "%s", jobData[i].plateFile);
    fscanf(file, "%lf", &jobData[i].duration);
    fscanf(file, "%lf", &jobData[i].thermalDiffusivity);
    fscanf(file, "%lf", &jobData[i].plateCellDimmensions);
    fscanf(file, "%lf", &jobData[i].balancePoint);
  }
  fclose(file);
  return jobData;
}

size_t calcFileLinesCount(const char* filePath) {
  FILE* file = fopen(filePath, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filePath);
    exit(EXIT_FAILURE);
  }
  size_t linesCount = 0;
  char c;
  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      linesCount++;
    }
  }
  fclose(file);
  return linesCount;
}

SimulationResult processJob(JobData jobData) {
  Plate plate = readPlate(jobData.plateFile);
  SimulationResult result = simulate(jobData, plate);
  return result;
}



// Code adapted from <https://es.stackoverflow.com/questions/409312/como-leer-un-binario-en-c>
Plate readPlate(const char* binaryFilepath) {
   Plate plate;
   FILE *binaryFile;
   size_t rows, cols;
   double** matrix;
  binaryFile = fopen(binaryFilepath, "rb");

    if(!binaryFile){
        printf("Error opening file %s\n",binaryFilepath);
        exit(EXIT_FAILURE);
    }

    fread(&rows,sizeof(size_t),1,binaryFile);
    fread(&cols,sizeof(size_t),1,binaryFile);

    matrix = (double**)malloc(rows*sizeof(double*));
    for(size_t i=0;i<rows;i++){
        matrix[i] = (double*)malloc(cols*sizeof(double));
        fread(matrix[i],sizeof(double),cols,binaryFile);
    }

    fclose(binaryFile);
    plate.data = matrix;
    plate.rows = rows;
    plate.cols = cols;
    return plate;
}

SimulationResult simulate(JobData jobData, Plate plate) {
  Plate previousPlate = copyPlate(plate);
  Plate currentPlate = copyPlate(plate);
  size_t iterationsCount = 0;

  do
  {
    previousPlate = copyPlate(currentPlate);
    currentPlate = simulationIteration(jobData, previousPlate);
    iterationsCount++;
  } while (!isPlateBalanced(currentPlate, previousPlate, jobData.balancePoint));

  SimulationResult result;
  result.plate = currentPlate;
  result.iterations = iterationsCount;
  return result;
  
}

Plate copyPlate(Plate plate) {
  Plate newPlate;
  newPlate.rows = plate.rows;
  newPlate.cols = plate.cols;
  newPlate.data = malloc(plate.rows * sizeof(double*));
  for (size_t i = 0; i < plate.rows; i++) {
    newPlate.data[i] = malloc(plate.cols * sizeof(double));
    memcpy(newPlate.data[i], plate.data[i], plate.cols * sizeof(double));
  }
  return newPlate;
}

Plate simulationIteration(JobData jobData, Plate plate) {
  Plate newPlate = copyPlate(plate);
  for (size_t i = 1; i < plate.rows - 1; i++) {
    for (size_t j = 1; j < plate.cols - 1; j++) {
      double left = plate.data[i][j - 1];
      double right = plate.data[i][j + 1];
      double up = plate.data[i - 1][j];
      double down = plate.data[i + 1][j];
      double cell = plate.data[i][j];
      // cell + ((duration * thermalDiffusivity) / (plateDimmensions * plateDimmensions)) * (left + right + up + down - 4 * cell);
      newPlate.data[i][j] = cell + ((jobData.duration * jobData.thermalDiffusivity) / (jobData.plateCellDimmensions * jobData.plateCellDimmensions)) * (left + right + up + down - 4 * cell);
    }
  }
  return newPlate;
}

void printPlate(Plate plate) {
  for (size_t i = 0; i < plate.rows; i++) {
    for (size_t j = 0; j < plate.cols; j++) {
      printf("%lf ", plate.data[i][j]);
    }
    printf("\n");
  }

  printf("\n");
}

bool isPlateBalanced(Plate currentPlate, Plate previousPlate, double balancePoint) {
  for (size_t i = 0; i < currentPlate.rows; i++) {
    for (size_t j = 0; j < currentPlate.cols; j++) {
      if (currentPlate.data[i][j] - previousPlate.data[i][j] > balancePoint) {
        return false;
      }
    }
  }
  return true;
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
    snprintf(buffer, buffer_size, "%02d/%02d/%02d %02d:%02d:%02d", years, months, days, hours, minutes, secs);
}