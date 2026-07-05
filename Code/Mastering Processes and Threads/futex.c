//#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <stdatomic.h>

// Futex wrapper
int futex_wait(int *addr, int val) {
    return syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
}

int futex_wake(int *addr, int n) {
    return syscall(SYS_futex, addr, FUTEX_WAKE, n, NULL, NULL, 0);
}

// Shared futex word
atomic_int futex_var = 0;

// Thread that waits
void* waiter(void *arg) {
    printf("Waiter: waiting for futex_var to become 1...\n");

    while (1) {
        if (atomic_load(&futex_var) == 1)
            break;

        // Sleep only if value is still 0
        futex_wait((int*)&futex_var, 0);
    }
    printf("Waiter: woke up!\n");
    return NULL;
}

// Thread that signals
void* waker(void *arg) {
    sleep(2);  // simulate work
    printf("Waker: setting futex_var = 1\n");
    atomic_store(&futex_var, 1);
    printf("Waker: waking waiter\n");
    futex_wake((int*)&futex_var, 1);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, waiter, NULL);
    pthread_create(&t2, NULL, waker, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}