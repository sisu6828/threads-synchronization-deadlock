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

#define LOOPS           10
#define NTHREADS        3
#define MAX_SLEEP_TIME  3 // Seconds

/* Declare global semaphore variables. Note, they must be initialized before use. */

psem_t *sem;


char flip(char x) {
    switch (x) {
        case 'A': 
            return 'B';
        case 'B':
            return 'A';
        default: 
             return '?';
    }
}

void trace(char id) {
    static char next = '?';
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    

    pthread_mutex_lock(&mutex);

    if (next == '?') {
        printf("%c\n", id);
        next = flip(id);
    } else {
        if (id == next) {
            next = '?';
            printf("%c\n\n", id);
        } else {
            printf("%c <===== ERROR: should have been %c\n\n", id, next);
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_unlock(&mutex);
}


/* TODO: Make the two threads perform their iterations in lockstep. */


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


/* TODO: Make the two threads perform their iterations in lockstep. */

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

    // Todo: Initialize semaphores.

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

    // Todo: Destroy semaphores.
    psem_destroy(sem);

    return 0;
}
