/*
 * CSC 308 - Operating Systems
 * Practical Session 3: Semaphore Implementation in C
 * Objective: Compare mutex locks and semaphores for shared resource protection
 *
 * Compile: gcc -o sem_compare sem_compare.c -lpthread
 * Run:     ./sem_compare
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define N_THREADS    8
#define N_LOOPS      60000
#define POOL_LIMIT   3

long mtx_result = 0;
long sem_result = 0;
int  running    = 0;

pthread_mutex_t mtx;
sem_t           bin_sem;
sem_t           pool_sem;

void *mtx_worker(void *arg) {
    for (int i = 0; i < N_LOOPS; i++) {
        pthread_mutex_lock(&mtx);
        mtx_result++;
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

void *sem_worker(void *arg) {
    for (int i = 0; i < N_LOOPS; i++) {
        sem_wait(&bin_sem);
        sem_result++;
        sem_post(&bin_sem);
    }
    return NULL;
}

void *pool_worker(void *arg) {
    int id = *((int *)arg);
    sem_wait(&pool_sem);
    running++;
    printf("  Worker %2d entered  | Currently active: %d/%d\n", id, running, POOL_LIMIT);
    sleep(1);
    running--;
    printf("  Worker %2d leaving  | Currently active: %d/%d\n", id, running, POOL_LIMIT);
    sem_post(&pool_sem);
    return NULL;
}

double elapsed_ms(struct timespec a, struct timespec b) {
    return (b.tv_sec - a.tv_sec) * 1000.0 + (b.tv_nsec - a.tv_nsec) / 1e6;
}

int main() {
    pthread_t tids[N_THREADS];
    struct timespec t0, t1;
    int expected = N_THREADS * N_LOOPS;

    printf("=== CSC 308 | Practical 3: Semaphore vs Mutex ===\n\n");
    printf("Workers: %d | Loops: %d | Expected: %d\n\n", N_THREADS, N_LOOPS, expected);

    pthread_mutex_init(&mtx, NULL);
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N_THREADS; i++) pthread_create(&tids[i], NULL, mtx_worker, NULL);
    for (int i = 0; i < N_THREADS; i++) pthread_join(tids[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    pthread_mutex_destroy(&mtx);
    printf("[MUTEX]        Result = %ld | OK: %s | %.2f ms\n",
           mtx_result, mtx_result == expected ? "YES" : "NO", elapsed_ms(t0, t1));

    sem_init(&bin_sem, 0, 1);
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N_THREADS; i++) pthread_create(&tids[i], NULL, sem_worker, NULL);
    for (int i = 0; i < N_THREADS; i++) pthread_join(tids[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    sem_destroy(&bin_sem);
    printf("[BINARY SEM]   Result = %ld | OK: %s | %.2f ms\n",
           sem_result, sem_result == expected ? "YES" : "NO", elapsed_ms(t0, t1));

    printf("\n--- Counting Semaphore: Max %d workers at once ---\n", POOL_LIMIT);
    sem_init(&pool_sem, 0, POOL_LIMIT);
    int ids[N_THREADS];
    for (int i = 0; i < N_THREADS; i++) {
        ids[i] = i + 1;
        pthread_create(&tids[i], NULL, pool_worker, &ids[i]);
    }
    for (int i = 0; i < N_THREADS; i++) pthread_join(tids[i], NULL);
    sem_destroy(&pool_sem);

    printf("\nConclusion: Use mutex for simple on/off locking.\n");
    printf("Use counting semaphores when managing a pool of N shared resources.\n");
    return 0;
}
