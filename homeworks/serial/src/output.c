#include "types.h"
#include "output.h"
#include <string.h>

// code adapted from <https://es.stackoverflow.com/questions/358361/escribir-leer-estructuras-en-archivos-binarios>
void writePlate(Plate plate, const char* binaryFilepath) {
  FILE *binaryFile;
  binaryFile = fopen(binaryFilepath, "wb");

  if(!binaryFile){
      printf("Error opening file %s\n",binaryFilepath);
      exit(EXIT_FAILURE);
  }

  fwrite(&plate.rows,sizeof(size_t),1,binaryFile);
  fwrite(&plate.cols,sizeof(size_t),1,binaryFile);

  for(size_t i=0;i<plate.rows;i++){
      fwrite(plate.data[i],sizeof(double),plate.cols,binaryFile);
  }

  fclose(binaryFile);
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

void writeJobData(JobData jobData, SimulationResult result, const char* filepath) {
  FILE *file;
  file = fopen(filepath, "w");

  if(!file){
      printf("Error opening file %s\n",filepath);
      exit(EXIT_FAILURE);
  }

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

  fclose(file);

}
