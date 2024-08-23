#pragma once
#include "types.h"
#include "output.h"

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
