#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 4

void print_array(int *array, int size);

typedef struct 
{
    int thread_id;
    int* array;
    int array_size;
} t_args;


void *IncrementArrayElement(void *arg) {

    t_args* args = (t_args*) arg;

    int* array = args->array;
    int current_thread_id = args->thread_id;
    int array_size = args->array_size;

    int num_check_positions = array_size / NUM_THREADS;
    int start_position = num_check_positions * current_thread_id;

    for (int i = start_position; i < start_position+num_check_positions; i++) {
        array[i]++;
    }

    free(arg);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    clock_t start, end;

    start = clock();

    t_args *args;

    if (argc<2) {
        printf("ERROR: no argument!\n Usage: %s <array_size>\n", argv[0]);
        return 1;
    }
    int input_size = atoi(argv[1]);
    if (input_size % NUM_THREADS) {
        printf("ERROR: Array size should be divisible by %d (program uses %d threads)", NUM_THREADS, NUM_THREADS);
        return 2;
    }

    // convert char (ascii) to int
    int array_size = atoi(argv[1]);
    int main_array[array_size];

    // construct array
    for (int i = 0; i < array_size; i++) {
        main_array[i] = 1;
    }

    pthread_t tid_system[NUM_THREADS];

    for (long i = 0; i < NUM_THREADS; i++) {
        args = malloc(sizeof(t_args));
        if (args == NULL) {
            printf("ERROR: error at malloc()");
            return 2;

        }
        args->thread_id = i;
        args->array = main_array;
        args->array_size = array_size;

        if (pthread_create(&tid_system[i], NULL, IncrementArrayElement, (void*) args)) {
            printf("ERROR: error at pthread_create()");
            return 3;
        }
    }

    for (long i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(tid_system[i], NULL)) {
            printf("ERROR: pthread_join() of thread %ld\n", i);
            return 4;
        }
    }

    print_array(main_array, array_size);

    printf("INFO: Main thread finished\n");

    end = clock();

    double cpu_time_used = ((double) (end-start) / CLOCKS_PER_SEC);
    printf("INFO: Took %f seconds", cpu_time_used);
   
    // hold main thread
    pthread_exit(NULL);


    return 0;
}

void print_array(int *array, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d,", array[i]);
    }
    puts("]");
}