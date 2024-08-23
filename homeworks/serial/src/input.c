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
