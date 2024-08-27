// Copyright <2024> <Aaron Santana Valdelomar - UCR>
#include <string.h>
#include <ctype.h>
#include "types.h"
#include "output.h"

void printPlate(Plate plate) {
  for (size_t i = 0; i < plate.rows; i++) {
    for (size_t j = 0; j < plate.cols; j++) {
      printf("%lf ", plate.data[i][j]);
    }
    printf("\n");
  }

  printf("\n");
}


// code adapted from <https://es.stackoverflow.com/questions/358361/escribir-leer-estructuras-en-archivos-binarios>
void writePlate(Plate plate, const char* binaryFilepath) {
  FILE *binaryFile;
  binaryFile = fopen(binaryFilepath, "wb");

  if (!binaryFile) {
      printf("Error opening file %s\n", binaryFilepath);
      exit(EXIT_FAILURE);
  }

  fwrite(&plate.rows, sizeof(size_t), 1, binaryFile);
  fwrite(&plate.cols, sizeof(size_t), 1, binaryFile);

  for (size_t i=0; i< plate.rows; i++) {
      fwrite(plate.data[i], sizeof(double), plate.cols, binaryFile);
  }

  fclose(binaryFile);
}


void writeJobsResult(JobData* jobsData, SimulationResult* results,
  size_t jobsCount, char* filepath) {
  FILE *file;

  char* jobNumbers = malloc(100 * sizeof(char));
  char* path = malloc(100 * sizeof(char));
  extractNumbers(jobsData[0].plateFile, jobNumbers);
  sprintf(path, "%s/job%s.tsv", jobsData[0].directory, jobNumbers);
  
  printf("Writing results to %s\n", path);
  file = fopen(path, "w");

  if (!file) {
      printf("Error opening file %s\n", filepath);
      exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < jobsCount; i++) {
    writeJobResult(jobsData[i], results[i], file);
    // TO-DO print plate in binary file
    char* binaryFilepath = malloc(100 * sizeof(char));
    removeExtension(jobsData[i].plateFile);
    sprintf(binaryFilepath, "%s/%s-%zu.bin", jobsData[i].directory,
      jobsData[i].plateFile, results[i].iterations);

    printf("Writing plate to %s\n", binaryFilepath);
    writePlate(results[i].plate, binaryFilepath);
    free(binaryFilepath);
  }

  free(jobNumbers);
  free(path);

  fclose(file);
}


void writeJobResult(JobData jobData, SimulationResult result, FILE* file) {
  fprintf(file, "%s ", jobData.plateFile);
  fprintf(file, "%lf ", jobData.duration);
  fprintf(file, "%lf ", jobData.thermalDiffusivity);
  fprintf(file, "%lf ", jobData.plateCellDimmensions);
  fprintf(file, "%lf ", jobData.balancePoint);
  fprintf(file, "%zu ", result.iterations);
  const time_t seconds = result.iterations * jobData.duration;
    char formatted_time[48];
    format_time(seconds, formatted_time, 48);
  fprintf(file, "%s", formatted_time);
  fprintf(file, "\n");
}

 void removeExtension(char *filepath) {
    char *last_dot = strrchr(filepath, '.');

    if (last_dot != NULL) {
        *last_dot = '\0';
    }
}


void extractNumbers(const char *filename, char *numbers) {
    while (*filename) {
        if (isdigit((unsigned char)*filename)) {
            *numbers++ = *filename;
        }
        filename++;
    }
    *numbers = '\0'; // Null-terminate the result string
}