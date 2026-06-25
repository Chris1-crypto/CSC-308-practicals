# CSC 308 – Operating Systems Practicals
**Nnamdi Azikiwe University (UNIZIK)**
**Student:** [Muoneke Christian] | **Reg. No:** 2023514239

---

## Files

| # | File | Topic |
|---|------|-------|
| 1 | `mutex_demo.c` | Mutex Lock Demonstration |
| 2 | `prod_cons.c` | Producer-Consumer Simulation |
| 3 | `sem_compare.c` | Semaphore vs Mutex |
| 4 | `shm_ipc.c` | Shared Memory IPC |

---

## Compile & Run

### Session 1 – Mutex
```bash
gcc -o mutex_demo mutex_demo.c -lpthread
./mutex_demo
```

### Session 2 – Producer-Consumer
```bash
gcc -o prod_cons prod_cons.c -lpthread
./prod_cons
```

### Session 3 – Semaphore
```bash
gcc -o sem_compare sem_compare.c -lpthread
./sem_compare
```

### Session 4 – Shared Memory
```bash
gcc -o shm_ipc shm_ipc.c
./shm_ipc
```

---

## Concepts Covered

- Mutex locks and race conditions
- Producer-Consumer with bounded circular buffer
- Binary vs counting semaphores
- Shared memory inter-process communication (IPC)

---

*CSC 308 – Operating Systems | UNIZIK*
