
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct 
{
    int thread_id;
    int num_threads;
} t_args;


void *Print(void *arg) {

    t_args args = *(t_args*) arg;

    printf("bolo de morando (thread) %d out of %d threads\n", args.thread_id, args.num_threads);

    free(arg);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
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
        printf("INFO: Creating thread %ld\n", i+1);
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

    printf("INFO: Main thread finished\n");
   
    // hold main thread
    pthread_exit(NULL);

    return 0;
}