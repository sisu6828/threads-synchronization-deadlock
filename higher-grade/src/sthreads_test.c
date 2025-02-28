#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>  // exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <stdio.h>   // printf(), fprintf(), stdout, stderr, perror(), _IOLBF
#include <stdbool.h> // true, false
#include <limits.h>  // INT_MAX


#include "sthreads.h" // init(), spawn(), yield(), done()
#define ADD_TEST(test) (CU_add_test(sthreads_test_suite, #test, test) == NULL) ||
/*******************************************************************************
                   Functions to be used together with spawn()

    You may add your own functions or change these functions to your liking.
********************************************************************************/
int init_suite(void)
{
    // Change this function if you want to do something *before* you
    // run a test suite
    return 0;
}

int clean_suite(void)
{
    // Change this function if you want to do something *after* you
    // run a test suite
    return 0;
}

/* Prints the sequence 0, 1, 2, .... INT_MAX over and over again.
 */
void numbers()
{
  int n = 0;
  while (true)
  {
    printf(" n = %d\n", n);
    n = (n + 1) % (INT_MAX);
    if (n > 3)
      done();
    yield();
  }
}

/* Prints the sequence a, b, c, ..., z over and over again.
 */
void letters()
{
  char c = 'a';

  while (true)
  {
    printf(" c = %c\n", c);
    if (c == 'f')
      done();
    yield();
    c = (c == 'z') ? 'a' : c + 1;
  }
}

/* Calculates the nth Fibonacci number using recursion.
 */
int fib(int n)
{
  switch (n)
  {
  case 0:
    return 0;
  case 1:
    return 1;
  default:
    return fib(n - 1) + fib(n - 2);
  }
}

/* Print the Fibonacci number sequence over and over again.

   https://en.wikipedia.org/wiki/Fibonacci_number

   This is deliberately an unnecessary slow and CPU intensive
   implementation where each number in the sequence is calculated recursively
   from scratch.
*/

void fibonacci_slow()
{
  int n = 0;
  int f;
  while (true)
  {
    f = fib(n);
    if (f < 0)
    {
      // Restart on overflow.
      n = 0;
    }
    printf(" fib(%02d) = %d\n", n, fib(n));
    n = (n + 1) % INT_MAX;
  }
}

/* Print the Fibonacci number sequence over and over again.

   https://en.wikipedia.org/wiki/Fibonacci_number

   This implementation is much faster than fibonacci().
*/
void fibonacci_fast()
{
  int a = 0;
  int b = 1;
  int n = 0;
  int next = a + b;

  while (true)
  {
    printf(" fib(%02d) = %d\n", n, a);
    next = a + b;
    a = b;
    b = next;
    n++;
    if (a < 0)
    {
      // Restart on overflow.
      a = 0;
      b = 1;
      n = 0;
    }
  }
}

/* Prints the sequence of magic constants over and over again.

   https://en.wikipedia.org/wiki/Magic_square
*/
void magic_numbers()
{
  int n = 3;
  int m;
  while (true)
  {
    m = (n * (n * n + 1) / 2);
    if (m > 0)
    {
      printf(" magic(%d) = %d\n", n, m);
      n = (n + 1) % INT_MAX;
    }
    else
    {
      // Start over when m overflows.
      n = 3;
    }
    yield();
  }
}

void print_yes_and_terminate()
{
  printf("yes");
  done();
}

void print_no_and_terminate()
{
  printf("no");
  done();
}

/*******************************************************************************
                                     main()

            Here you should add code to test the Simple Threads API.
********************************************************************************/


// void test_thread_manager_init(){
//   CU_ASSERT_EQUAL(init(),1);
// }

// void test_can_we_spawn_a_thread(){
//   tid_t A = spawn(numbers);
//   CU_ASSERT_EQUAL(A,0);
// }

// void can_two_threads_run(){
//   tid_t A = spawn(numbers);
//   tid_t B = spawn(letters);
//   CU_ASSERT_EQUAL(A,0);
//   CU_ASSERT_EQUAL(B,1);
// }

// int main()
// {
//     // First we try to set up CUnit, and exit if we fail
//     if (CU_initialize_registry() != CUE_SUCCESS)
//         return CU_get_error();

//     // We then create an empty test suite and specify the name and
//     // the init and cleanup functions
//     CU_pSuite sthreads_test_suite = CU_add_suite("sthreads_test.c", init_suite, clean_suite);
//     if (sthreads_test_suite == NULL)
//     {
//         // If the test suite could not be added, tear down CUnit and exit
//         CU_cleanup_registry();
//         return CU_get_error();
//     }

//     // This is where we add the test functions to our test suite.
//     // For each call to CU_add_test we specify the test suite, the
//     // name or description of the test, and the function that runs
//     // the test in question. If you want to add another test, just
//     // copy a line below and change the information
//     if (
//       ADD_TEST(test_thread_manager_init)
//         0)
//     {
//         // If adding any of the tests fails, we tear down CUnit and exit
//         CU_cleanup_registry();
//         return CU_get_error();
//     }

//     // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
//     // Use CU_BRM_NORMAL to only print errors and a summary
//     CU_basic_set_mode(CU_BRM_VERBOSE);

//     // This is where the tests are actually run!
//     CU_basic_run_tests();

//     // Tear down CUnit before exiting
//     CU_cleanup_registry();
//     return CU_get_error();
// }


int main(){

  int res = init(); // Initialization
  if (!res)
  {
    perror("Failed to initialize the thread manager");
    exit(EXIT_FAILURE);
  }
  
  // tid_t A = spawn(magic_numbers);


  return 0;
}
