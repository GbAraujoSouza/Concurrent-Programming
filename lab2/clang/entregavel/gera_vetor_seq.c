#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VALUE 1000

int main(int argc, char *argv[]) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <vector_size>\n", argv[0]);
    return 1;
  }

  long int size = atoi(argv[1]);

  double *vector1, *vector2;

  vector1 = (double *)malloc(sizeof(double) * size);
  vector2 = (double *)malloc(sizeof(double) * size);
  if (!vector1 || !vector2) {
    fprintf(stderr, "ERROR: memory allocation error\n");
    return 2;
  }

  srand(time(NULL));
  double dot_product_sum = 0.0;
  double elem1, elem2;
  for (long int i = 0; i < size; i++) {
    elem1 = (rand() % MAX_VALUE) * 1.0;
    vector1[i] = elem1;
    elem2 = (rand() % MAX_VALUE) * 1.0;
    vector2[i] = elem2;

    dot_product_sum += elem1 * elem2;
  }


  FILE *pfile = fopen("vectors", "wb");
  if (!pfile) {
    fprintf(stderr, "ERROR: failure to initialize file\n");
    return 3;
  }

  size_t writen_objects;
  writen_objects = fwrite(&size, sizeof(long int), 1, pfile);
  writen_objects = fwrite(vector1, sizeof(double), size, pfile);
  if (writen_objects < size) {
    fprintf(stderr, "ERROR: failure to write first vector to file\n");
    return 4;
  }
  writen_objects = fwrite(vector2, sizeof(double), size, pfile);
  if (writen_objects < size) {
    fprintf(stderr, "ERROR: failure to write second vector to file\n");
    return 5;
  }

  writen_objects = fwrite(&dot_product_sum, sizeof(double), 1, pfile);

  fclose(pfile);
  free(vector1);
  free(vector2);

  return 0;
}
