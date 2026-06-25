/*
 * CSC 308 - Operating Systems
 * Practical Session 4: Shared Memory Programming
 * Objective: Inter-process communication using shared memory segments
 *
 * Compile: gcc -o shm_ipc shm_ipc.c
 * Run:     ./shm_ipc
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MEM_KEY   5678
#define MEM_SIZE  1024

typedef struct {
    char  data[512];
    int   sequence;
    int   finished;
} SharedRegion;

int main() {
    int          seg_id;
    SharedRegion *mem;
    pid_t        child_pid;

    printf("=== CSC 308 | Practical 4: Shared Memory IPC ===\n\n");

    /* Step 1: Allocate shared memory segment */
    seg_id = shmget(MEM_KEY, sizeof(SharedRegion), IPC_CREAT | 0666);
    if (seg_id < 0) { perror("shmget"); exit(1); }
    printf("[PARENT] Segment created (ID: %d, Size: %lu bytes)\n",
           seg_id, sizeof(SharedRegion));

    /* Step 2: Attach to parent address space */
    mem = (SharedRegion *)shmat(seg_id, NULL, 0);
    if (mem == (SharedRegion *)-1) { perror("shmat"); exit(1); }

    memset(mem, 0, sizeof(SharedRegion));
    printf("[PARENT] Memory attached and cleared.\n\n");

    child_pid = fork();
    if (child_pid < 0) { perror("fork"); exit(1); }

    if (child_pid == 0) {
        /* CHILD: attach and write to shared memory */
        mem = (SharedRegion *)shmat(seg_id, NULL, 0);
        if (mem == (SharedRegion *)-1) { perror("child shmat"); exit(1); }

        printf("[CHILD]  Writer started (PID: %d)\n", getpid());

        for (int n = 1; n <= 5; n++) {
            mem->sequence = n;
            snprintf(mem->data, sizeof(mem->data),
                     "Message #%d from child (PID %d)", n, getpid());
            printf("[CHILD]  Sent: \"%s\"\n", mem->data);
            sleep(1);
        }

        mem->finished = 1;
        printf("[CHILD]  Done writing. Detaching...\n");
        shmdt(mem);
        exit(0);

    } else {
        /* PARENT: poll and read from shared memory */
        printf("[PARENT] Waiting for messages...\n\n");
        sleep(1);

        int last_seq = 0;
        while (!mem->finished) {
            if (mem->sequence > last_seq) {
                printf("[PARENT] Received: \"%s\"\n", mem->data);
                last_seq = mem->sequence;
            }
            usleep(300000);
        }
        printf("[PARENT] Received: \"%s\"\n", mem->data);

        wait(NULL);

        /* Step 5: Detach and delete shared memory */
        shmdt(mem);
        shmctl(seg_id, IPC_RMID, NULL);

        printf("\n[PARENT] Memory detached and segment removed.\n");
        printf("[PARENT] IPC complete. Final sequence count: %d\n\n", last_seq);

        printf("Real-world use cases:\n");
        printf("  > Database buffer pools   (e.g., MySQL, PostgreSQL)\n");
        printf("  > High-speed web servers  (e.g., Nginx)\n");
        printf("  > Scientific simulations  (parallel data exchange)\n");
        printf("  > Embedded/real-time OS   (sensor data pipelines)\n");
    }

    return 0;
}
