// Copyright <2024> <Aaron Santana Valdelomar - UCR>
#include "input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"

#define MAX_PATH_SIZE 100

Arguments processArguments(int argc, char **argv) {
  const int AGUMENTS_COUNT = 3;  // 3 arguments are expected
  Arguments args;
  // Check if the number of arguments is correct
  if (argc != AGUMENTS_COUNT) {
    fprintf(stderr, "Usage: %s <jobFile> <threadsCount>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // assign the arguments to the struct
  args.jobFile = argv[1];
  args.threadsCount = atoi(argv[2]);
  return args;
}

JobData *readJobData(const char *jobFile) {
  FILE *file = fopen(jobFile, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", jobFile);
    exit(EXIT_FAILURE);
  }

  // get the directory from the job file
  char directory[MAX_PATH_SIZE];
  getDirectory(jobFile, directory, MAX_PATH_SIZE);

  // Read the number of jobs based on the number of lines in the file
  size_t jobs = calcFileLinesCount(jobFile);
  JobData *jobData = malloc(jobs * sizeof(JobData));

  assert(jobData != NULL);  // Check if memory allocation was successful

  // Read the job data from the file and store it in the array
  for (size_t i = 0; i < jobs; i++) {
    jobData[i].plateFile = malloc(MAX_PATH_SIZE * sizeof(char));
    jobData[i].directory = malloc(MAX_PATH_SIZE * sizeof(char));
    fscanf(file, "%s", jobData[i].plateFile);
    fscanf(file, "%lf", &jobData[i].duration);
    fscanf(file, "%lf", &jobData[i].thermalDiffusivity);
    fscanf(file, "%lf", &jobData[i].plateCellDimmensions);
    fscanf(file, "%lf", &jobData[i].balancePoint);
    snprintf(jobData[i].directory, MAX_PATH_SIZE,
      "%s", directory);
  }
  fclose(file);
  return jobData;
}

size_t calcFileLinesCount(const char *filePath) {
  FILE *file = fopen(filePath, "r");
  if (file == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filePath);
    exit(EXIT_FAILURE);
  }
  size_t linesCount = 0;
  char c;
  // Count the number of lines in the file
  while ((c = fgetc(file)) != EOF) {
    if (c == '\n') {
      linesCount++;
    }
  }
  fclose(file);
  return linesCount;
}

// Code adapted from <https://es.stackoverflow.com/questions/409312/como-leer-un-binario-en-c>
Plate* readPlate(const char *binaryFilepath, char *directory) {
  Plate* plate = malloc(sizeof(Plate));
  FILE *binaryFile;
  size_t rows, cols;
  double **matrix;
  char path[MAX_PATH_SIZE];
  sprintf(path, "%s/%s", directory, binaryFilepath);
  binaryFile = fopen(path, "rb");

  if (!binaryFile) {
    printf("Error opening file %s\n", path);
    exit(EXIT_FAILURE);
  }

  fread(&rows, sizeof(size_t), 1, binaryFile);
  fread(&cols, sizeof(size_t), 1, binaryFile);

  matrix = (double **)malloc(rows * sizeof(double *));
  for (size_t i = 0; i < rows; i++) {
    matrix[i] = (double *)malloc(cols * sizeof(double));
    fread(matrix[i], sizeof(double), cols, binaryFile);
  }

  fclose(binaryFile);
  plate->data = matrix;
  plate->rows = rows;
  plate->cols = cols;
  plate->isBalanced = 0;
  return plate;
}

void getDirectory(const char *path, char *directory, size_t size) {
    strncpy(directory, path, size);
    directory[size - 1] = '\0';

    char *last_slash = strrchr(directory, '/');

    if (last_slash != NULL) {
        *last_slash = '\0';
    } else {
        directory[0] = '\0';
    }
}
