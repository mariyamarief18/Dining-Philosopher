/**
 * phil.c
 * 
 * Program to implement a solution to dining-philosopher problem using
 * POSIX mutex locks and POSIX condition variables. 
 * 
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// defining N as 5 for the number of philosophers
#define N 5 

// 3 states: thinking, hungry, eating
#define THINKING 0
#define HUNGRY 1
#define EATING 2

// defining left and right of philosopher
#define LEFT (philN + 4) % N
#define RIGHT (philN + 1) % N

// number of times a philosopher should eat
int times = 2;

// state of philosophers
int state[N];

// philophers index
int phil[N] = {0, 1, 2, 3, 4};

// mutex variable for synchronization
pthread_mutex_t mutex;

// philosopher condition variable
pthread_cond_t condition[N];

// function to check if philosopher can eat or not
void check(int philN)
{
    if (state[philN] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) 
    {
        printf("Philosopher %d is eating\n", philN+1);
        
        // setting state of philosopher to EATING
        state[philN] = EATING;

        // to wake up hungry philosopher blocked on the condition
        pthread_cond_signal(&condition[philN]);
    }
}

// function for picking up forks
void pickup_forks(int philN)
{
    // acquire mutex lock
    pthread_mutex_lock(&mutex);

    printf("Philosopher %d is hungry\n", philN+1);

    // setting state of philosopher to HUNGRY
    state[philN] = HUNGRY;

    // check whether the philosopher can eat or not
    check(philN);

    // if philosopher does not have forks available for EATING
    if (state[philN] != EATING)
    {
        // waiting on condition
        pthread_cond_wait(&condition[philN], &mutex);
    }

    // release mutex lock
    pthread_mutex_unlock(&mutex);
}

void return_forks(int philN)
{
    // acquire mutex lock
    pthread_mutex_lock(&mutex);

    printf("Philosopher %d is thinking\n", philN+1);

    // setting state of philosopher to THINKING
    state[philN] = THINKING;

    // checking whether left and right neighbours are HUNGRY
    check(LEFT);
    check(RIGHT);

    // release mutex lock
    pthread_mutex_unlock(&mutex);
}

// function for philosopher to do actions
void *philospher(void *num)
{
    int c = 0;
    while(c < times)
    {
        int *i = num;

        sleep(2);

        pickup_forks(*i);

        sleep(1);

        return_forks(*i);

        c++;
    }
}

int main()
{
    pthread_t thread_id[N];

    // initialize mutex object
    pthread_mutex_init(&mutex, NULL);

    // initialize condition variables
    for (int i = 0; i < N; i++) 
    {
        pthread_cond_init(&condition[i], NULL);
    }

    // creating thread 
    for (int i = 0; i < N; i++)
    {
        pthread_create(&thread_id[i], NULL, philospher, &phil[i]);

        printf("Philosopher %d is thinking\n", i+1);

        sleep(1);
    }

    // joining thread
    for (int i = 0; i < N; i++)
    {
        pthread_join(thread_id[i], NULL);
    }

    // destroy condition variables
    for (int i = 0; i < N; i++) 
    {
        pthread_cond_destroy(&condition[i]);
    }
 
    // destroy mutex object
    pthread_mutex_destroy(&mutex);

    pthread_exit(NULL);
 
    return 0;
}