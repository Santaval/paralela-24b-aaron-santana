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
// #include <mpi.h>

#include "input.h"
#include "solution.h"
#include "output.h"
#include "MpiWrapper.h"

/**
 * @brief Start program execution.
 *
 * @return Status code to the operating system, 0 means success.
 */
int main(int argc, char** argv) {
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);


  Mpi mpi;
  mpi_init(&mpi, &argc, &argv);


  int MAIN_PROCESS = 0;
  int DISCONNECT_SIGNAL = 1;
  int REQUEST_NEWJOB = 2;


  if (mpi.rank == MAIN_PROCESS) {
    Arguments args = processArguments(argc, argv);
    JobData* jobsData = readJobData(args.jobFile);
    size_t jobsCount = calcFileLinesCount(args.jobFile);
    SimulationResult* results = malloc(jobsCount * sizeof(SimulationResult));
    size_t processedCount = 0;
    int disconnectedCount = 0;

    // distribute first jobs

    for (int i = 1; i < mpi.size; i++) {
      if (processedCount < jobsCount) {
        bool shouldProcessAJob = true;
        mpi_send(&shouldProcessAJob, 1, MPI_C_BOOL, i, 0);
        sendJobData(&jobsData[processedCount], i);
        processedCount++;
      } else {
        bool shouldProcessAJob = false;
        mpi_send(&shouldProcessAJob, 1, MPI_C_BOOL, i, 0);
        mpi_receive(&DISCONNECT_SIGNAL, 1, MPI_INT, i, 0, NULL);
        disconnectedCount++;
      }
    }

    // receive jobs results
    while (disconnectedCount < (mpi.size - 1))
    { 
      int* source = malloc(1 * sizeof(int));
      SimulationResult result;
      receiveJobResult(&result, MPI_ANY_SOURCE, source);
      results[result.jobIndex] = result;
      if (processedCount < jobsCount) {
        bool shouldProcessAJob = true;
        mpi_send(&shouldProcessAJob, 1, MPI_C_BOOL, *source, 0);
        sendJobData(&jobsData[processedCount], *source);
      } else {
        bool shouldProcessAJob = false;
        mpi_send(&shouldProcessAJob, 1, MPI_C_BOOL, *source, 0);
        mpi_receive(&DISCONNECT_SIGNAL, 1, MPI_INT, *source, 0, NULL);
        disconnectedCount++;
        printf("Me, process %d, received a disconnect signal from process %d\n", mpi.rank, *source);
      }

      processedCount++;
    }


    writeJobsResult(jobsData, results, jobsCount, "output.txt");
    destroyJobsData(jobsData, jobsCount);
    destroySimulationResult(results, jobsCount);
    printf("Me, process %d, im disconnecting\n", mpi.rank);

  } else {
    while (true)
    { 
      bool shouldProcessAJob;
      mpi_receive(&shouldProcessAJob, 1, MPI_C_BOOL, MAIN_PROCESS, 0, NULL);
      if (shouldProcessAJob) {
        JobData jobData;
        receiveJobData(&jobData, MAIN_PROCESS);
        printf("Me, process %d, received the job number %d\n", mpi.rank, jobData.jobIndex);
        SimulationResult result = processJob(jobData);
        result.jobIndex = jobData.jobIndex;
        sendJobResult(&result, MAIN_PROCESS);
        printf("Me, process %d, sent the result of job number %d\n", mpi.rank, result.jobIndex);
      } else {
        mpi_send(&DISCONNECT_SIGNAL, 1, MPI_INT, MAIN_PROCESS, 0);
        break;
      }
    }
  }

  mpi_finalize();
  clock_gettime(CLOCK_MONOTONIC, &end);
  double elapsedTime = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec -
    start.tv_nsec) / 1e6;
  printf("Elapsed time: %.2f ms\n", elapsedTime);


  return EXIT_SUCCESS;
}

void sendJobData(JobData* jobData, int dest) {
    int plateFileLength = strlen(jobData->plateFile) + 1;
    int directoryLength = strlen(jobData->directory) + 1;

    mpi_send(&plateFileLength, 1, MPI_INT, dest, 0);
    mpi_send(jobData->plateFile, plateFileLength, MPI_CHAR, dest,1);
    mpi_send(&directoryLength, 1, MPI_INT, dest, 2);
    mpi_send(jobData->directory, directoryLength, MPI_CHAR, dest,3);

    mpi_send(&jobData->duration, 1, MPI_DOUBLE, dest, 4);
    mpi_send(&jobData->thermalDiffusivity, 1, MPI_DOUBLE, dest, 5);
    mpi_send(&jobData->plateCellDimmensions, 1, MPI_DOUBLE, dest, 6);
    mpi_send(&jobData->balancePoint, 1, MPI_DOUBLE, dest, 7);
    mpi_send(&jobData->threadCount, 1, MPI_INT, dest, 8);
    mpi_send(&jobData->jobIndex, 1, MPI_INT, dest, 9);
}

void receiveJobData(JobData* jobData, int source) {
  int plateFileLength, directoryLength;

  mpi_receive(&plateFileLength, 1, MPI_INT, source, 0, NULL);
  jobData->plateFile = (char*) malloc(plateFileLength);
  mpi_receive(jobData->plateFile, plateFileLength, MPI_CHAR, source, 1, NULL);

  mpi_receive(&directoryLength, 1, MPI_INT, source, 2, NULL);
  jobData->directory = (char*) malloc(directoryLength);
  mpi_receive(jobData->directory, directoryLength, MPI_CHAR, source, 3, NULL);

  mpi_receive(&jobData->duration, 1, MPI_DOUBLE, source, 4, NULL);
  mpi_receive(&jobData->thermalDiffusivity, 1, MPI_DOUBLE, source, 5, NULL);
  mpi_receive(&jobData->plateCellDimmensions, 1, MPI_DOUBLE, source, 6, NULL);
  mpi_receive(&jobData->balancePoint, 1, MPI_DOUBLE, source, 7, NULL);
  mpi_receive(&jobData->threadCount, 1, MPI_DOUBLE, source, 8, NULL);
  mpi_receive(&jobData->jobIndex, 1, MPI_INT, source, 9, NULL);

}

void sendJobResult(SimulationResult* result, int dest) {
  size_t rows = result->plate->rows;
  size_t cols = result->plate->cols;

  mpi_send(&rows, 1, MPI_INT, dest, 0);
  mpi_send(&cols, 1, MPI_INT, dest, 1);
  for (size_t i = 0; i < rows; i++) {
    mpi_send(result->plate->data[i], cols, MPI_DOUBLE, dest, i + 2);
  }



  mpi_send(&result->iterations, 1, MPI_INT, dest, rows + 2);
  mpi_send(&result->jobIndex, 1, MPI_INT, dest, rows + 3);
}

void receiveJobResult(SimulationResult* result, int source, int* sourceCb) {

  size_t rows = 0;
  size_t cols = 0;

  mpi_receive(&rows, 1, MPI_INT, source, 0, NULL);
  mpi_receive(&cols, 1, MPI_INT, source, 1, NULL);

  Plate* plate = malloc(sizeof(Plate));
  plate->rows = rows;
  plate->cols = cols;
  plate->data = malloc(rows * sizeof(double*));
  for (size_t i = 0; i < rows; i++) {
    plate->data[i] = malloc(cols * sizeof(double));
    mpi_receive(plate->data[i], cols, MPI_DOUBLE, source, i + 2, NULL);
  }

  result->plate = plate;

  mpi_receive(&result->iterations, 1, MPI_INT, source, rows + 2, NULL);
  mpi_receive(&result->jobIndex, 1, MPI_INT, source, rows + 3, sourceCb);
}



SimulationResult processJob(JobData jobData) {
  Plate* plate = readPlate(jobData.plateFile, jobData.directory);
  SimulationResult result = simulate(jobData, plate);
  // writeJobResult(jobData, result, stdout);
  return result;
}

SimulationResult simulate(JobData jobData, Plate* plate) {
  Plate* readPlate = copyPlate(plate);
  Plate* writePlate = plate;
  const size_t totalCells = readPlate->rows * readPlate->cols;
  SharedData* sharedData = malloc(sizeof(SharedData));
  sharedData->readPlate = readPlate;
  sharedData->writePlate = writePlate;
  sharedData->threadCount = jobData.threadCount > totalCells ? totalCells
    : jobData.threadCount;
  sharedData->jobData = jobData;
  sharedData->totalIterations = 0;
  sharedData->currentCell = 0;

  calcNewTemperature(sharedData);



  SimulationResult result;
  result.plate = writePlate;
  result.iterations = sharedData->totalIterations + 1;

  // free memory
  destroyPlate(readPlate);
  free(sharedData);
  return result;
}

void calcNewTemperature(SharedData* sharedData) {
    const JobData jobData = sharedData->jobData;
    const double factor = (jobData.duration * jobData.thermalDiffusivity) /
                    (jobData.plateCellDimmensions * jobData.plateCellDimmensions);

    while (1) {
        double** currentPlateData = sharedData->readPlate->data;
        double** newPlateData = sharedData->writePlate->data;
        int isBalanced = 1;

        if (sharedData->writePlate->isBalanced == 2) {
            break;
        }

        const size_t rows = sharedData->readPlate->rows;
        const size_t cols = sharedData->readPlate->cols;

        #pragma omp parallel for reduction(&:isBalanced)
        for (size_t row = 1; row < rows - 1; ++row) {
            int localIsBalanced = 1;
            for (size_t col = 1; col < cols - 1; ++col) {
                double left = currentPlateData[row][col - 1];
                double right = currentPlateData[row][col + 1];
                double up = currentPlateData[row - 1][col];
                double down = currentPlateData[row + 1][col];
                double cell = currentPlateData[row][col];

                double newTemperature = cell + factor * (left + right + up + down - 4 * cell);
                newPlateData[row][col] = newTemperature;

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

    return;
}


Plate* copyPlate(Plate* plate) {
  Plate* newPlate = malloc(sizeof(Plate));
  newPlate->rows = plate->rows;
  newPlate->cols = plate->cols;
  newPlate->isBalanced = plate->isBalanced;
  newPlate->data = malloc(newPlate->rows * sizeof(double*));
  for (size_t i = 0; i < newPlate->rows; i++) {
    newPlate->data[i] = malloc(newPlate->cols * sizeof(double));
    memcpy(newPlate->data[i], plate->data[i], newPlate->cols * sizeof(double));
  }
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
  for (size_t i = 0; i < plate->rows; i++) {
    free(plate->data[i]);
  }
  free(plate->data);
  free(plate);
}

void destroySimulationResult(SimulationResult* results, size_t resultsCount) {
    for (size_t i = 0; i < resultsCount; i++) {
        destroyPlate(results[i].plate);
    }
    free(results);
}

