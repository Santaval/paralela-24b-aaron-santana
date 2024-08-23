#pragma once
#include "input.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"


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
