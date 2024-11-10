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

  // init concurrency controls
    pthread_mutex_init(&sharedData->can_accsess_isBalanced, NULL);
    pthread_mutex_init(&sharedData->barrierMutex, NULL);
    pthread_mutex_init(&sharedData->can_accsess_currentCell, NULL);
    sem_init(&sharedData->turnstile1, 0, 0);
    sem_init(&sharedData->turnstile2, 0, 1);
    sharedData->barrierCount = 0;

    struct private_data* team = create_threads(sharedData->threadCount,
      calcNewTemperature, sharedData);


    join_threads(sharedData->threadCount, team);
    pthread_mutex_destroy(&sharedData->can_accsess_isBalanced);
    pthread_mutex_destroy(&sharedData->barrierMutex);
    pthread_mutex_destroy(&sharedData->can_accsess_currentCell);
    sem_destroy(&sharedData->turnstile1);
    sem_destroy(&sharedData->turnstile2);

  SimulationResult result;
  result.plate = writePlate;
  result.iterations = sharedData->totalIterations + 1;

  // free memory
  destroyPlate(readPlate);
  free(sharedData);
  return result;
}

void* calcNewTemperature(void* data) {
    const struct private_data* privateData = (struct private_data*)data;
    SharedData* sharedData = (SharedData*) privateData->data;

    const size_t threadCount = privateData->thread_count;

    const JobData jobData = sharedData->jobData;
    const double factor = (jobData.duration * jobData.thermalDiffusivity) /
                    (jobData.plateCellDimmensions *
                    jobData.plateCellDimmensions);
    const size_t rows = sharedData->readPlate->rows;
    const size_t cols = sharedData->readPlate->cols;

    // Ajuste para manejar divisiones no exactas
    size_t rowsPerThread = rows / threadCount;
    // Filas extra a distribuir entre los primeros hilos
    size_t extraRows = rows % threadCount;

    size_t startRow, endRow;

    if (privateData->thread_number < extraRows) {
        // Los primeros 'extraRows' hilos procesan una fila extra
        startRow = privateData->thread_number * (rowsPerThread + 1);
        endRow = startRow + rowsPerThread + 1;
    } else {
        // Los hilos restantes procesan solo 'rowsPerThread' filas
        startRow = privateData->thread_number * rowsPerThread + extraRows;
        endRow = startRow + rowsPerThread;
    }

    while (1) {
        double** currentPlateData = sharedData->readPlate->data;
        double** newPlateData = sharedData->writePlate->data;
        int localIsBalanced = 1;

        if (sharedData->writePlate->isBalanced == 2) {
            break;
        }

        // --------------------------
        #ifndef DYNAMIC_MAPPING
      printf("Iteration %zu\n", sharedData->totalIterations);

        // Procesar las celdas en el rango de filas asignadas al hilo
        for (size_t row = startRow; row < endRow; ++row) {
            for (size_t col = 1; col < cols - 1; ++col) {
                if (row > 0 && row < rows - 1) {
                    double left = currentPlateData[row][col - 1];
                    double right = currentPlateData[row][col + 1];
                    double up = currentPlateData[row - 1][col];
                    double down = currentPlateData[row + 1][col];
                    double cell = currentPlateData[row][col];

                    double newTemperature = cell + factor *
                    (left + right  + up + down - 4 * cell);
                      newPlateData[row][col] = newTemperature;

                    if (fabs(newTemperature - cell) > jobData.balancePoint) {
                        localIsBalanced = 0;  // No está balanceado
                    }
                }
            }
        }
        // ---------------------------------------
        #else
        // dynamic mapping
        size_t totalCells = rows * cols;
        size_t currentCell;
        while (1) {
            pthread_mutex_lock(&sharedData->can_accsess_currentCell);
            currentCell = sharedData->currentCell;
            sharedData->currentCell++;
            pthread_mutex_unlock(&sharedData->can_accsess_currentCell);
            if (currentCell >= totalCells) {
                break;
            }

            size_t row = currentCell / cols;
            size_t col = currentCell % cols;

            if (row > 0 && row < (rows - 1) && col > 0 && col < cols - 1) {
                double left = currentPlateData[row][col - 1];
                double right = currentPlateData[row][col + 1];
                double up = currentPlateData[row - 1][col];
                double down = currentPlateData[row + 1][col];
                double cell = currentPlateData[row][col];

                double newTemperature = cell + factor * (left
                + right + up + down - 4 * cell);
                newPlateData[row][col] = newTemperature;

                if (fabs(newTemperature - cell) > jobData.balancePoint) {
                    localIsBalanced = 0;  // No está balanceado
                }
            }
        }
        #endif
        pthread_mutex_lock(&sharedData->can_accsess_isBalanced);
        sharedData->writePlate->isBalanced =
          sharedData->writePlate->isBalanced && localIsBalanced;
        pthread_mutex_unlock(&sharedData->can_accsess_isBalanced);

        // Esperar a que todos los hilos terminen
        pthread_mutex_lock(&sharedData->barrierMutex);
        if (++sharedData->barrierCount == sharedData->threadCount) {
            sem_wait(&sharedData->turnstile2);
            sem_post(&sharedData->turnstile1);
            pthread_mutex_lock(&sharedData->can_accsess_isBalanced);
            if (sharedData->writePlate->isBalanced) {
              sharedData->writePlate->isBalanced = 2;
            } else {
              sharedData->writePlate->isBalanced = 1;
              // swap plates
              Plate* temp = sharedData->readPlate;
              sharedData->readPlate = sharedData->writePlate;
              sharedData->writePlate = temp;
              sharedData->totalIterations++;
              sharedData->currentCell = 0;
            }
            pthread_mutex_unlock(&sharedData->can_accsess_isBalanced);
        }
        pthread_mutex_unlock(&sharedData->barrierMutex);
        // Esperar a que todos los hilos lleguen
        sem_wait(&sharedData->turnstile1);
        sem_post(&sharedData->turnstile1);  // Despertar a los demás hilos

        pthread_mutex_lock(&sharedData->barrierMutex);
        if (--sharedData->barrierCount == 0) {
            sem_wait(&sharedData->turnstile1);
            sem_post(&sharedData->turnstile2);
        }
        pthread_mutex_unlock(&sharedData->barrierMutex);

        sem_wait(&sharedData->turnstile2);
        sem_post(&sharedData->turnstile2);
    }

    return NULL;
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

struct private_data* create_threads(size_t thread_count,
  void* (*routine)(void* data), void* data) {
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
  for (size_t thread_number = 0; thread_number < thread_count;
    ++thread_number) {
      int error = pthread_join(team[thread_number].thread_id, NULL);
      if (result == EXIT_SUCCESS) {
          result = error;
      }
  }

  free(team);
  return result;
}
