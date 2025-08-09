#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct 
{
    int thread_id;
    int num_threads;
} t_args;


void *Print(void *arg) {

    t_args args = *(t_args*) arg;

    // Sleep for a random duration (max at 0.1 seconds)
    // This is to add "fake complexity" to the task to show that they run independently
    usleep((rand() % 100000));

    printf("bolo de morando (thread) %d out of %d threads\n", args.thread_id, args.num_threads);

    free(arg);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // change random seed
    srand(time(NULL));

    t_args *args;
    int num_threads;

    if (argc<2) {
        printf("ERROR: no argument!\n Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }
    // convert char (ascii) to int
    num_threads = atoi(argv[1]);

    pthread_t tid_system[num_threads];

    for (long i = 0; i < num_threads; i++) {
        // printf("INFO: Creating thread %ld\n", i+1);
        args = malloc(sizeof(t_args));
        if (args == NULL) {
            printf("ERROR: error at malloc()");
            return 2;

        }
        args->thread_id = i+1;
        args->num_threads = num_threads;

        if (pthread_create(&tid_system[i], NULL, Print, (void*) args)) {
            printf("ERROR: error at pthread_create()");
            return 3;
        }
    }

    for (long i = 0; i < num_threads; i++) {
        if (pthread_join(tid_system[i], NULL)) {
            printf("ERROR: pthread_join() of thread %ld\n", i);
            return 4;
        }
    }

    printf("INFO: Main thread finished\n");
   
    // hold main thread
    pthread_exit(NULL);

    return 0;
}