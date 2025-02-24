/**
 * bank.c
 *
 * The implementation of the simple bank API defined in bank.h.
 *
 * Author
 *   Karl Marklund <karl.marklund@it.uu.se>
 *
 */

#include "bank.h"  // The bank API

#include <stdbool.h>  // true, false
#include <stdio.h>    // perror()
#include <stdlib.h>   // exit(), EXIT_FAILURE
#include <unistd.h>   // usleep()

#define MIN_SLEEP 10000
#define MAX_RANDOM_SLEEP 10000

void RANDOM_SLEEP() { usleep(MIN_SLEEP + (rand() % MAX_RANDOM_SLEEP)); }

account_t *account_new(unsigned int balance) {
  account_t *account = malloc(sizeof(account_t));
  if (!account )
  {
    perror("failed to allocate memory for account");
    exit(EXIT_FAILURE);
  }
  
  

  account->balance = balance;
  
  if (pthread_mutex_init(&account->lock, NULL) != 0) {
    perror("failed to initialize mutex");
    free(account);
    exit(EXIT_FAILURE);
  }


  return account;
}

void account_destroy(account_t *account) {
  if (account != NULL) {
    pthread_mutex_destroy(&account->lock);
    free(account);
  }
}

/**
 * A purposefully stupid way to add two numbers that makes data
 * races more likely.
 *
 * Do NOT add synchronization here.
 */
int add(int a, int b) {
  int result = a + b;
  RANDOM_SLEEP();
  return result;
}

/**
 * A purposefully stupid way to subtract two numbers that makes data
 * races more likely.
 *
 * Do NOT add synchronization here.
 */
int sub(int a, int b) {
  int result = a - b;
  RANDOM_SLEEP();
  return result;
}

int transfer(int amount, account_t *from, account_t *to) {
    int res_a = pthread_mutex_trylock(&to->lock);
    int res_b = pthread_mutex_trylock(&from->lock);

    if (res_a != 0 || res_b != 0) {
          pthread_mutex_unlock(&to->lock);
          pthread_mutex_unlock(&from->lock);
        return -1;
    }

    if (from->balance >= amount) {

      
      
    from->balance = sub(from->balance, amount);

    /**
     * Don't remove this RANDOM_SLEEP. This is used to enforce a more
     * randomized interleaving of the threads.
     */
    RANDOM_SLEEP();

    to->balance = add(to->balance, amount);

    pthread_mutex_unlock(&to->lock);
    pthread_mutex_unlock(&from->lock);
    return 0;
  } else {
    // pthread_mutex_unlock(&to->lock);
    // pthread_mutex_unlock(&from->lock);
    return -1;
  }
}
