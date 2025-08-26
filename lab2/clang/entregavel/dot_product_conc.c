#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  long int n;
  float *vector1;
  float *vector2;
  short int nthreads;
  short int id;
} t_args;

void *partialDotProduct(void *args) {
  t_args *arg = (t_args *)args;
  int ini, fim, fatia;
  float partial_sum = 0, *ret;

  fatia = arg->n / arg->nthreads;
  ini = arg->id * fatia;
  fim = ini + fatia;
  float *vector1 = arg->vector1;
  float *vector2 = arg->vector2;
  if (arg->id == (arg->nthreads - 1))
    fim = arg->n;

  for (int i = ini; i < fim; i++) {
    partial_sum += (vector1[i]) * (vector2[i]);
  }

  ret = (float *)malloc(sizeof(float));
  if (ret != NULL)
    *ret = partial_sum;
  else
    printf("--ERRO: malloc() thread\n");
  pthread_exit((void *)ret);
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, "Usage: %s <input_file> <number_of_threads>\n", argv[0]);
    return EXIT_FAILURE;
  }
  int nthreads = atoi(argv[2]);
  if (nthreads == 0) {
    fprintf(stderr, "ERROR: number of threads is either 0 or not a number");
    return EXIT_FAILURE;
  }

  FILE *pfile;
  pfile = fopen(argv[1], "rb");
  if (!pfile) {
    fprintf(stderr, "ERROR: failure to open file %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  size_t read_objects;
  long int n;
  read_objects = fread(&n, sizeof(long int), 1, pfile);

  float *vector1, *vector2;
  vector1 = (float *)malloc(sizeof(float) * n);
  vector2 = (float *)malloc(sizeof(float) * n);
  if (!vector1 || !vector2) {
    fprintf(stderr, "ERROR: failure to allocate memory for vectors\n");
    return EXIT_FAILURE;
  }

  read_objects = fread(vector1, sizeof(float), n, pfile);
  read_objects = fread(vector2, sizeof(float), n, pfile);

  if (nthreads > n)
    nthreads = n;

  pthread_t *tid_system;

  tid_system = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
  if (tid_system == NULL) {
    fprintf(stderr, "ERROR: malloc()\n");
    return EXIT_FAILURE;
  }

  for (long int i = 0; i < nthreads; i++) {
    t_args *args = (t_args *)malloc(sizeof(t_args));
    args->id = i;
    args->n = n;
    args->vector1 = vector1;
    args->vector2 = vector2;
    args->nthreads = nthreads;
    if (pthread_create(&tid_system[i], NULL, partialDotProduct, (void *)args)) {
      fprintf(stderr, "ERROR: pthread_create()\n");
      return EXIT_FAILURE;
    }
  }

  float total_dot_product = 0;
  float *ret_sum;
  for (int i = 0; i < nthreads; i++) {
    if (pthread_join(tid_system[i], (void *)&ret_sum)) {
      printf("ERROR: pthread_join()\n");
      return EXIT_FAILURE;
    }
    total_dot_product += *ret_sum;
    free(ret_sum);
  }

  float original_dot_product;
  printf("soma_concorrente             = %.26f\n", total_dot_product);
  read_objects = fread(&original_dot_product, sizeof(double), 1, pfile);
  printf("\nSoma-ori                     = %.26lf\n", original_dot_product);

  free(vector1);
  free(vector2);
  free(tid_system);
  fclose(pfile);

  return 0;
}
