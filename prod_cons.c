/*
 * CSC 308 - Operating Systems
 * Practical Session 2: Producer-Consumer Simulation
 * Objective: Implement Producer-Consumer using POSIX semaphores
 *
 * Compile: gcc -o prod_cons prod_cons.c -lpthread
 * Run:     ./prod_cons
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_SLOTS   6
#define TOTAL_JOBS  12

int storage[MAX_SLOTS];
int write_pos = 0;
int read_pos  = 0;

sem_t access_lock;
sem_t free_slots;
sem_t used_slots;

void show_storage() {
    printf("  Storage: [");
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (storage[i] != 0)
            printf(" %d", storage[i]);
        else
            printf("  _");
    }
    printf(" ]\n");
}

void *producer_fn(void *arg) {
    for (int job = 1; job <= TOTAL_JOBS; job++) {
        sleep(1);

        sem_wait(&free_slots);
        sem_wait(&access_lock);

        storage[write_pos] = job;
        printf("[PRODUCER] Added job %2d at slot %d\n", job, write_pos);
        write_pos = (write_pos + 1) % MAX_SLOTS;
        show_storage();

        sem_post(&access_lock);
        sem_post(&used_slots);
    }
    return NULL;
}

void *consumer_fn(void *arg) {
    for (int i = 0; i < TOTAL_JOBS; i++) {
        sleep(2);

        sem_wait(&used_slots);
        sem_wait(&access_lock);

        int job = storage[read_pos];
        storage[read_pos] = 0;
        printf("[CONSUMER] Took  job %2d from slot %d\n", job, read_pos);
        read_pos = (read_pos + 1) % MAX_SLOTS;
        show_storage();

        sem_post(&access_lock);
        sem_post(&free_slots);
    }
    return NULL;
}

int main() {
    pthread_t p_thread, c_thread;

    printf("=== CSC 308 | Practical 2: Producer-Consumer ===\n\n");
    printf("Storage capacity : %d\n", MAX_SLOTS);
    printf("Total jobs       : %d\n\n", TOTAL_JOBS);

    for (int i = 0; i < MAX_SLOTS; i++) storage[i] = 0;

    sem_init(&access_lock, 0, 1);
    sem_init(&free_slots,  0, MAX_SLOTS);
    sem_init(&used_slots,  0, 0);

    pthread_create(&p_thread, NULL, producer_fn, NULL);
    pthread_create(&c_thread, NULL, consumer_fn, NULL);

    pthread_join(p_thread, NULL);
    pthread_join(c_thread, NULL);

    sem_destroy(&access_lock);
    sem_destroy(&free_slots);
    sem_destroy(&used_slots);

    printf("\nSimulation complete. No overflow or deadlock occurred.\n");
    return 0;
}
