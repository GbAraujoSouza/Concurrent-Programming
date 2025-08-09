#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *Print(void *arg) {
    printf("hmm, bolo de morango!\n");

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int num_threads;

    if (argc<2) {
        printf("ERROR: no argument!\n Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }
    // convert char (ascii) to int
    num_threads = atoi(argv[1]);

    pthread_t tid_system[num_threads];

    for (int i = 0; i < num_threads; i++) {
        printf("INFO: Creating thread %d\n", i);

        if (pthread_create(&tid_system[i], NULL, Print, NULL)) {
            printf("ERROR: error at pthread_create()");
            return 2;
        }
    }

    printf("INFO: Main thread finished\n");
   
    // hold main thread
    pthread_exit(NULL);

    return 0;
}