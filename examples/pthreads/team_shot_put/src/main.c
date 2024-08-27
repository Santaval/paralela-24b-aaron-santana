// Copyright 2024 Jeisson Hidalgo
#define _POSIX_C_SOURCE 199506L
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// const size_t team_count = 2;
#define team_count 2
// const size_t shot_count = 3;
#define shot_count 3

struct private_data {
  size_t athlete_number;
  size_t team_number;
  size_t athlete_count;
  double** best_shots;
};

void compete(const size_t athlete_count, double** best_shots);
double** create_double_matrix(const size_t rows, const size_t cols);
void destroy_double_matrix(double** matrix, size_t rows);
void* athlete(void* data);
double shot(const size_t team_number, const size_t athlete_number);
double random_f(const double min, const double max);
void print_result(const size_t athlete_count, double** best_shots);

int main(int argc, char* argv[]) {
  // Get the number of athletes in each team
  size_t athlete_count = 0;
  if (argc >= 2) {
    if (sscanf(argv[1], "%zu", &athlete_count) == 1) {
      // Athlete count must be odd
      if (athlete_count % 2) {
        double** best_shots = create_double_matrix(team_count, athlete_count);
        if (best_shots) {
          srand(time(NULL) + clock());
          compete(athlete_count, best_shots);
          print_result(athlete_count, best_shots);
          destroy_double_matrix(best_shots, team_count);
        } else {
          fprintf(stderr, "Error no enough memory\n");
        }
      } else {
        fprintf(stderr, "error: athlete count must be odd\n");
      }
    }
  } else {
    fprintf(stderr, "Usage: %s athlete_count\n", argv[0]);
  }
}

void compete(const size_t athlete_count, double** best_shots) {
  pthread_t* threads[team_count] = { NULL };
  struct private_data* private_array[team_count] = { NULL };
  // For each team
  for (size_t team_number = 0; team_number < team_count; ++team_number) {
    threads[team_number] = calloc(athlete_count, sizeof(pthread_t));
    assert(threads[team_number]);
    private_array[team_number] = calloc(athlete_count,
        sizeof(struct private_data));
    assert(private_array[team_number]);
    // For each athlete
    for (size_t athlete_number = 0; athlete_number < athlete_count;
        ++athlete_number) {
      private_array[team_number][athlete_number].athlete_number =
          athlete_number;
      private_array[team_number][athlete_number].team_number = team_number;
      private_array[team_number][athlete_number].athlete_count = athlete_count;
      private_array[team_number][athlete_number].best_shots = best_shots;
      pthread_create(&threads[team_number][athlete_number], NULL, athlete,
          &private_array[team_number][athlete_number]);
    }
  }

  for (size_t team_number = 0; team_number < team_count; ++team_number) {
    for (size_t athlete_number = 0; athlete_number < athlete_count;
        ++athlete_number) {
      pthread_join(threads[team_number][athlete_number], NULL);
    }
    free(private_array[team_number]);
    free(threads[team_number]);
  }
}

void* athlete(void* data) {
  struct private_data* my_data = (struct private_data*)data;
  // Score the best shot this athlete reached
  const size_t team_number = my_data->team_number;
  my_data->best_shots[team_number][my_data->athlete_number] =
      shot(team_number, my_data->athlete_number);
  return NULL;
}

double shot(const size_t team_number, const size_t athlete_number) {
  double my_best_shot = 0.0;
  for (size_t shot_number = 0; shot_number < shot_count; ++shot_number) {
    double shot_distance = random_f(0.0, 25.0);
    if (shot_distance > my_best_shot) {
      my_best_shot = shot_distance;
    }
  }
  printf("%zu.%zu: best shot put %.3lfm\n", team_number + 1, athlete_number + 1,
      my_best_shot);
  return my_best_shot;
}

double random_f(const double min, const double max) {
  return min + (double) rand() / RAND_MAX * (max - min);
}

void print_result(const size_t athlete_count, double** best_shots) {
  // Count the scored points of each team
  assert(team_count == 2);
  size_t team_scores[team_count] = { 0 };
  // For each athlete
  for (size_t athlete_number = 0; athlete_number < athlete_count;
      ++athlete_number) {
    // Give a point to the winner team in this round
    if (best_shots[0][athlete_number] > best_shots[1][athlete_number]) {
      ++team_scores[0];
    } else if (best_shots[1][athlete_number] > best_shots[0][athlete_number]) {
      ++team_scores[1];
    } else {
      printf("draw for round %zu\n", athlete_number);
    }
  }

  // Print result
  const size_t winner = team_scores[0] > team_scores[1] ? 1 :
      team_scores[1] > team_scores[0] ? 2 : 0;
  printf("result %zu:%zu, team %zu wins\n", team_scores[0], team_scores[1],
      winner);
}

double** create_double_matrix(const size_t rows, const size_t cols) {
  double** matrix = calloc(rows, sizeof(double*));
  if (matrix) {
    for (size_t row = 0; row < rows; ++row) {
      if ((matrix[row] = calloc(cols, sizeof(double))) == NULL) {
        destroy_double_matrix(matrix, rows);
        return NULL;
      }
    }
  }
  return matrix;
}

void destroy_double_matrix(double** matrix, size_t rows) {
  if (matrix) {
    for (size_t row = 0; row < rows; ++row) {
      free(matrix[row]);
    }
    free(matrix);
  }
}
