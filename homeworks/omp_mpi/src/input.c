// Copyright <2024> <Aaron Santana Valdelomar - UCR>
#include "input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "types.h"

#define MAX_PATH_SIZE 100

Arguments processArguments(int argc, char **argv) {
  const int MIN_ARGUMENTS_COUNT = 3;  // 3 arguments are expected

  Arguments args;
  args.isVerbose = 0;
  args.shloudPrintIterations = 0;

  if (argc == 2 && (strcmp(argv[1], "-h") == 0 ||
    strcmp(argv[1], "--help") == 0)) {
      fprintf(stderr, "Usage: %s <jobFile> <threadsCount>\n", argv[0]);
      fprintf(stderr, "jobFile: path to the file containing the job data\n");
      fprintf(stderr, "threadsCount: number of threads to be used\n");
      fprintf(stderr, "FLAGS:\n");
      fprintf(stderr, "-h, --help: show this help message\n");
      fprintf(stderr, "-v, --verbose: show verbose output\n");
      fprintf(stderr,
        "-i, --iterations: show current iteration (k) number\n");

  } else if ( argc >= MIN_ARGUMENTS_COUNT ) {
     // assign the arguments to the struct
    args.jobFile = argv[1];
    if (sscanf(argv[2], "%zu", &args.threadsCount) != 1) {
      args.threadsCount = sysconf(_SC_NPROCESSORS_ONLN);
    }

    if (argc > MIN_ARGUMENTS_COUNT) {
      for (int i = MIN_ARGUMENTS_COUNT; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
          args.isVerbose = 1;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i],
          "--iterations") == 0) {
          args.shloudPrintIterations = 1;
        }
      }
      printf("Verbose: %d\n", args.isVerbose);
      printf("Print iterations: %d\n", args.shloudPrintIterations);
    }
  } else {
    fprintf(stderr, "Usage: %s <jobFile> <threadsCount>\n", argv[0]);
    fprintf(stderr, "jobFile: path to the file containing the job data\n");
    fprintf(stderr, "threadsCount: number of threads to be used\n");
    exit(EXIT_FAILURE);
  }
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
    jobData[i].jobIndex = i;
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
  snprintf(path, MAX_PATH_SIZE, "%s/%s", directory, binaryFilepath);
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