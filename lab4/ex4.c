#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

long int soma = 0;
int continue_flag = 1;
int finished_threads = 0;
int global_nthreads;

pthread_mutex_t mutex;
pthread_cond_t cond_extra;
pthread_cond_t cond_executa;

void *ExecutaTarefa(void *arg) {
  long int id = (long int)arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i = 0; i < 100000; i++) {
    pthread_mutex_lock(&mutex);

    while (soma > 0 && soma % 1000 == 0 && !continue_flag) {
      pthread_cond_signal(&cond_extra);
      pthread_cond_wait(&cond_executa, &mutex);
    }

    soma++;

    if (soma % 1000 == 0) {
      continue_flag = 0;
      pthread_cond_signal(&cond_extra);
    }

    pthread_mutex_unlock(&mutex);
  }

  pthread_mutex_lock(&mutex);
  finished_threads++;
  if (finished_threads == global_nthreads) {
    pthread_cond_signal(&cond_extra);
  }
  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}

void *extra(void *args) {
  printf("Extra : esta executando...\n");

  pthread_mutex_lock(&mutex);

  while (finished_threads < global_nthreads) {
    while ((soma == 0 || soma % 1000 != 0 || continue_flag) &&
           finished_threads < global_nthreads) {
      pthread_cond_wait(&cond_extra, &mutex);
    }

    if (finished_threads >= global_nthreads) {
      break;
    }

    if (soma % 1000 == 0 && soma > 0 && !continue_flag) {
      printf("soma = %ld \n", soma);
      continue_flag = 1;
      pthread_cond_broadcast(
          &cond_executa);
    }
  }

  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t *tid;
  int nthreads;

  if (argc < 2) {
    printf("Usage: %s <numero de threads>\n", argv[0]);
    return 1;
  }
  nthreads = atoi(argv[1]);
  global_nthreads = nthreads;

  tid = (pthread_t *)malloc(sizeof(pthread_t) * (nthreads + 1));
  if (tid == NULL) {
    puts("ERROR: malloc");
    return 2;
  }

  pthread_mutex_init(&mutex, NULL);

  pthread_cond_init(&cond_extra, NULL);
  pthread_cond_init(&cond_executa, NULL);

  if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
    printf("ERROR: pthread_create()\n");
    exit(-1);
  }

  for (long int t = 0; t < nthreads; t++) {
    if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
      printf("ERROR: pthread_create()\n");
      exit(-1);
    }
  }

  for (int t = 0; t < nthreads + 1; t++) {
    if (pthread_join(tid[t], NULL)) {
      printf("ERROR: pthread_join() \n");
      exit(-1);
    }
  }

  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond_extra);
  pthread_cond_destroy(&cond_executa);

  printf("Valor de 'soma' = %ld\n", soma);

  return 0;
}
