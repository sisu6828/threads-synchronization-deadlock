/**
 * Barrier
 *  
 * Two threads executing chunks of work in lock step.
 *
 * Author(s): Nikos Nikoleris <nikos.nikoleris@it.uu.se>
 *             
 *            Original version. 
 * 
 *            Karl Marklund <karl.markund@it.uu.se> 
 * 
 *            Added the trace() function and automatic error detection.
 */

#include <stdio.h>     /* printf() */
#include <stdlib.h>    /* abort(), [s]rand() */
#include <unistd.h>    /* sleep() */
#include <pthread.h>   /* pthread_...() */

#include "psem.h"

#define LOOPS           10  // Number of iterations.
#define MAX_SLEEP_TIME  3   // Thread sleep time in seconds.

/**
 *  Declare global semaphore variables. Note, they must be initialized before
 *  use. 
 */

psem_t *sem;

/**
 *  Helper function used by trace.
 */

char next(char x) {
    switch (x) {
        case 'A': 
            return 'B';
        case 'B':
            return 'A';
        default: 
             return '?';
    }
}

/**
 * Function used to trace the execution of the threads and detect
 * invalid barrier synchronization.
 */

void trace(char id) {
    static char nxt = '?';
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    
    pthread_mutex_lock(&mutex);

    if (nxt == '?') {
        printf("%c\n", id);
        nxt = next(id);
    } else {
        if (id == nxt) {
            nxt = '?';
            printf("%c\n\n", id);
        } else {
            printf("%c <===== ERROR: should have been %c\n\n", id, nxt);
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_unlock(&mutex);
}

/**
 *  TODO: Enforce barrier synchronization between the two thread A and B, i.e., 
 *        make the threads perform their iterations in lockstep. 
 */

void *
threadA(void *param __attribute__((unused)))
{
    int i;

    for (i = 0; i < LOOPS; i++) {
        trace('A');
        sleep(rand() % MAX_SLEEP_TIME);
    }

    pthread_exit(0);
}

/**
 *  TODO: Enforce barrier synchronization between the two thread A and B, i.e., 
 *        make the threads perform their iterations in lockstep. 
 */

void *
threadB(void *param  __attribute__((unused)))
{
    int i;

    for (i = 0; i < LOOPS; i++) {
        trace('B');
        sleep(rand() % MAX_SLEEP_TIME);
    }

    pthread_exit(0);
}

int
main()
{
    pthread_t tidA, tidB;

    /**
     * Todo: Initialize semaphores.
     */

    sem = psem_init(666);
    
    srand(time(NULL));
    pthread_setconcurrency(3);

    if (pthread_create(&tidA, NULL, threadA, NULL) ||
        pthread_create(&tidB, NULL, threadB, NULL)) {
        perror("pthread_create");
        abort();
    }
    if (pthread_join(tidA, NULL) != 0 ||
        pthread_join(tidB, NULL) != 0) {
        perror("pthread_join");
        abort();
    }

    /**
     * Todo: Destroy semaphores.
     */

    psem_destroy(sem);

    return 0;
}
