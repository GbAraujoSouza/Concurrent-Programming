#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 6
#define BUFFER_SIZE 100

int M, N;
int buffer[BUFFER_SIZE];
int primes_by_thread[NTHREADS - 1] = {0};
int in = 0, out = 0;
int produced_nums = 0;
int consumed_nums = 0;

sem_t mutex;
sem_t full_sem;
sem_t empty_sem;

int ehPrimo(long long int n) {
  if (n <= 1)
    return 0;
  if (n == 2)
    return 1;
  if (n % 2 == 0)
    return 0;
  for (int i = 3; i * i <= n; i += 2)
    if (n % i == 0)
      return 0;
  return 1;
}

void *producer(void *args) {
  while (1) {
    sem_wait(&mutex);
    if (produced_nums >= N) {
      sem_post(&mutex);
      break;
    }

    int batch_size = M;
    if (produced_nums + batch_size > N)
      batch_size = N - produced_nums;

    int start_num = produced_nums + 1;
    produced_nums += batch_size;
    sem_post(&mutex);

    // aguarda espaço disponível
    for (int i = 0; i < batch_size; i++) {
      sem_wait(&empty_sem);
    }

    sem_wait(&mutex);
    for (int i = 0; i < batch_size; i++) {
      buffer[in] = start_num + i;
      in = (in + 1) % M;
    }
    sem_post(&mutex);

    for (int i = 0; i < batch_size; i++) {
      sem_post(&full_sem);
    }
  }

  for (int i = 0; i < NTHREADS - 1; i++) {
    sem_post(&full_sem);
  }

  pthread_exit(NULL);
}

void *consumer(void *args) {
  int id = *(int *)args;
  int local_prime_count = 0;

  while (1) {
    sem_wait(&full_sem);
    sem_wait(&mutex);

    if (consumed_nums >= N) {
      sem_post(&mutex);
      break;
    }

    int n = buffer[out];
    out = (out + 1) % M;
    consumed_nums++;

    sem_post(&mutex);
    sem_post(&empty_sem);

    if (ehPrimo(n)) {
      local_prime_count++;
    }
  }

  sem_wait(&mutex);
  primes_by_thread[id] = local_prime_count;
  sem_post(&mutex);

  free(args);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  if (argc >= 3) {
    N = atoi(argv[1]);
    M = atoi(argv[2]);
  } else {
    fprintf(stderr, "Uso: %s <N> <M>\n", argv[0]);
    return 1;
  }

  sem_init(&mutex, 0, 1);
  sem_init(&full_sem, 0, 0);
  sem_init(&empty_sem, 0, M);

  pthread_t *tid = malloc(sizeof(pthread_t) * NTHREADS);

  for (int i = 0; i < NTHREADS; i++) {
    if (i == 0) {
      // produtor
      if (pthread_create(&tid[i], NULL, producer, NULL)) {
        perror("pthread_create");
        return 1;
      }
    } else {
      // consumidores
      int *a = malloc(sizeof(int));
      *a = i - 1;
      if (pthread_create(&tid[i], NULL, consumer, a)) {
        perror("pthread_create");
        return 1;
      }
    }
  }

  for (int i = 0; i < NTHREADS; i++) {
    if (pthread_join(tid[i], NULL)) {
      perror("pthread_join");
    }
  }

  int total_primos = 0, max = 0, vencedor = 0;

  printf("\n=== CONTAGEM POR CONSUMIDOR ===\n");
  for (int i = 0; i < NTHREADS - 1; i++) {
    printf("Consumidor %d encontrou %d primos\n", i, primes_by_thread[i]);
    total_primos += primes_by_thread[i];
    if (primes_by_thread[i] > max) {
      max = primes_by_thread[i];
      vencedor = i;
    }
  }

  printf("\n=== RESULTADOS ===\n");
  printf("Total de números processados: %d\n", N);
  printf("Tamanho do buffer: %d\n", M);
  printf("Total de primos encontrados: %d\n", total_primos);
  printf("Consumidor vencedor: %d (com %d primos)\n", vencedor, max);

  sem_destroy(&mutex);
  sem_destroy(&full_sem);
  sem_destroy(&empty_sem);
  free(tid);

  return 0;
}
