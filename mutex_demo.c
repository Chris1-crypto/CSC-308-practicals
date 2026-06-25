/*
 * CSC 308 - Operating Systems
 * Practical Session 1: Mutex Lock Demonstration
 * Objective: Show mutual exclusion using pthread mutex locks
 *
 * Compile: gcc -o mutex_demo mutex_demo.c -lpthread
 * Run:     ./mutex_demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_COUNT   8
#define LOOP_SIZE      80000

long safe_total   = 0;
long unsafe_total = 0;

pthread_mutex_t lock;

void *safe_increment(void *arg) {
    for (int i = 0; i < LOOP_SIZE; i++) {
        pthread_mutex_lock(&lock);
        safe_total++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *unsafe_increment(void *arg) {
    for (int i = 0; i < LOOP_SIZE; i++) {
        unsafe_total++;
    }
    return NULL;
}

int main() {
    pthread_t tid[THREAD_COUNT];
    int expected = THREAD_COUNT * LOOP_SIZE;

    printf("=== CSC 308 | Practical 1: Mutex Lock Demo ===\n\n");
    printf("Thread count   : %d\n", THREAD_COUNT);
    printf("Loops/thread   : %d\n", LOOP_SIZE);
    printf("Expected value : %d\n\n", expected);

    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_create(&tid[i], NULL, safe_increment, NULL);
    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_join(tid[i], NULL);

    pthread_mutex_destroy(&lock);
    printf("[PROTECTED]   Result = %ld | Accurate: %s\n",
           safe_total, safe_total == expected ? "YES" : "NO");

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_create(&tid[i], NULL, unsafe_increment, NULL);
    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_join(tid[i], NULL);

    printf("[UNPROTECTED] Result = %ld | Accurate: %s\n",
           unsafe_total, unsafe_total == expected ? "YES" : "NO");

    printf("\nTip: Run unprotected version multiple times — result changes each run!\n");
    return 0;
}
