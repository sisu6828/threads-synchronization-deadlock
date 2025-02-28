/* On Mac OS (aka OS X) the ucontext.h functions are deprecated and requires the
   following define.
*/
#define _XOPEN_SOURCE 700

/* On Mac OS when compiling with gcc (clang) the -Wno-deprecated-declarations
   flag must also be used to suppress compiler warnings.
*/

#include <signal.h>   /* SIGSTKSZ (default stack size), MINDIGSTKSZ (minimal
                         stack size) */
#include <stdio.h>    /* puts(), printf(), fprintf(), perror(), setvbuf(), _IOLBF,
                         stdout, stderr */
#include <stdlib.h>   /* exit(), EXIT_SUCCESS, EXIT_FAILURE, malloc(), free() */
#include <ucontext.h> /* ucontext_t, getcontext(), makecontext(),
                         setcontext(), swapcontext() */
#include <stdbool.h>  /* true, false */

#include <sys/time.h> // ITIMER_REAL, ITIMER_VIRTUAL, ITIMER_PROF, struct itimerval, setitimer()
#include "sthreads.h"

#include <sys/ucontext.h>

/* Stack size for each context. */
#define STACK_SIZE SIGSTKSZ * 100

/*******************************************************************************
                             Global data structures

                Add data structures to manage the threads here.
********************************************************************************/
/* Shared variable */

volatile int PID_COUNTER = -1;

thread_t *CURRENTLY_RUNNING_THREAD = NULL;

thread_t *MASTER_THREAD_MANAGER = NULL;

thread_t *FIRST_THREAD_IN_READY_QUEUE = NULL;

thread_t *LAST_THREAD_IN_READY_QUEUE = NULL;

thread_t *FIRST_THREAD_IN_WAITING_QUEUE = NULL;

thread_t *FIRST_THREAD_IN_TERMINATED_QUEUE = NULL;

volatile bool MANAGER_RUNNING = true;

bool PRINT_DEBUGGING = true;

/* Shared variable used to implement a spinlock */
volatile int lock = false;

/*******************************************************************************
                             Auxiliary functions

                      Add internal helper functions here.
********************************************************************************/

void link_context(ucontext_t *a, ucontext_t *b)
{
   a->uc_link = b;
}

void init_context(ucontext_t *ctx, ucontext_t *next)
{
   /* Allocate memory to be used as the stack for the context. */
   void *stack = malloc(STACK_SIZE);

   if (stack == NULL)
   {
      perror("Allocating stack");
      exit(EXIT_FAILURE);
   }

   if (getcontext(ctx) < 0)
   {
      perror("getcontext");
      exit(EXIT_FAILURE);
   }

   /* Before invoking makecontext(ctx), the caller must allocate a new stack for
      this context and assign its address to ctx->uc_stack, and define a successor
      context and assigns address to ctx->uc_link.
   */

   ctx->uc_link = next;
   ctx->uc_stack.ss_sp = stack;
   ctx->uc_stack.ss_size = STACK_SIZE;
   ctx->uc_stack.ss_flags = 0;
}

void thread_manager_init()
{
   thread_t *thread0 = (thread_t *)malloc(sizeof(thread_t));
   ucontext_t *ctx = malloc(sizeof(ucontext_t));
   int res = getcontext(ctx);

   if (res < 0)
   {
      perror("getcontext");
      exit(EXIT_FAILURE);
   }
   
   thread0->ctx = *ctx;
   thread0->state = ready;
   thread0->next = NULL;
   thread0->tid = 0;

   thread0->next = thread0;
   MASTER_THREAD_MANAGER = thread0;
}
void thread_manager_manage()
{
}

/* Initialize context ctx  with a call to function func with zero argument.
 */
void init_context0(ucontext_t *ctx, void (*func)(), ucontext_t *next)
{
   init_context(ctx, next);
   makecontext(ctx, func, 0);
}

/* Initialize context ctx with a call to function func with one string argument.
 */
void init_context1(ucontext_t *ctx, void (*func)(), const char *str, ucontext_t *next)
{
   init_context(ctx, next);
   makecontext(ctx, func, 1, str);
}

void timer_handler(int signal)
{
   if (PRINT_DEBUGGING)
   {
      printf("Received signal, <<ONLY PRINTS>> %d\n", signal);
   }

   exit(EXIT_SUCCESS);
}

void add_thread_to_ready_queue(thread_t *thread)
{
   if (FIRST_THREAD_IN_READY_QUEUE == NULL)
   {
      FIRST_THREAD_IN_READY_QUEUE = thread;
      LAST_THREAD_IN_READY_QUEUE = thread;
   }
   else
   {
      LAST_THREAD_IN_READY_QUEUE->next = thread;
      LAST_THREAD_IN_READY_QUEUE = thread;
   }
}

void run_first_task_in_ready_queue()
{
   thread_t *thread = FIRST_THREAD_IN_READY_QUEUE;

   // FIRST_THREAD_IN_READY_QUEUE = thread->next;
   CURRENTLY_RUNNING_THREAD = thread;
   CURRENTLY_RUNNING_THREAD->state = running;
   setcontext(&CURRENTLY_RUNNING_THREAD->ctx);
}

void master_context()
{
   printf("Master context\n");
}

/*******************************************************************************
                    Implementation of the Simple Threads API
********************************************************************************/
void init_context_thread_manager(ucontext_t *ctx, void (*start)(), void *arg)
{
   init_context(ctx, NULL);
   makecontext(ctx, start, 0);
}

int init()
{
   // Set up the signal handler
   if (signal(SIGALRM, timer_handler) == SIG_ERR) //
   {
      perror("Failed to register signal handler");
      exit(EXIT_FAILURE);
   }

   // setvbuf(stdout, 0, _IOLBF, 0);

   thread_manager_init();

   add_thread_to_ready_queue(MASTER_THREAD_MANAGER);

   run_first_task_in_ready_queue();

   return 1;
}

tid_t spawn(void (*start)())
{
   thread_t *new = malloc(sizeof(thread_t));
   ucontext_t ctx;
   if (!getcontext(&ctx))
   {
      perror("getcontext");
      free(new);
      return (-1);
   }

   new->state = ready;
   new->next = NULL;
   ++PID_COUNTER;

   new->tid = PID_COUNTER;

   add_thread_to_ready_queue(new);
   return new->tid;
}

void yield()
{
   CURRENTLY_RUNNING_THREAD->state = ready;
   // signal dispatch
   CURRENTLY_RUNNING_THREAD = MASTER_THREAD_MANAGER;
}

void done()
{
   CURRENTLY_RUNNING_THREAD->state = terminated;
   CURRENTLY_RUNNING_THREAD = MASTER_THREAD_MANAGER;
}

tid_t join(tid_t thread) { return -1; }
