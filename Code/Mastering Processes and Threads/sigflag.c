#include <signal.h>
#include <stdio.h>
#include <unistd.h>

/* Structure holding the shared data */
struct datastruct {
    int a;
    int b;
    int c;
};

struct datastruct data;

/* If this flag is nonzero, don’t handle the signal right away. */
volatile sig_atomic_t signal_pending = 0;

/* This is nonzero if a signal arrived and was not handled. */
volatile sig_atomic_t defer_signal = 0;

/* Signal handler */
void handler(int signum)
{
    if (defer_signal) {
        signal_pending = signum;
        printf("\nDEFERRED");
    } else {
        printf("\nSignal %d handled immediately\n", signum);
    }
}

void update_data(int frob)
{
    /* Prevent signals from having immediate effect. */
    defer_signal++;

    sleep(2);

    /* Update data safely */
    data.a = 1;
    data.b = 2;
    data.c = frob;

    printf("\nStructure updated:\n");
    printf("a = %d\nb = %d\nc = %d\n", data.a, data.b, data.c);

    /* We have updated the structure. Handle any deferred signal. */
    defer_signal--;

    if (defer_signal == 0 && signal_pending != 0) {
        int sig = signal_pending;
        signal_pending = 0;
        raise(sig);
    }
}

int main() {

    struct sigaction sa;

    /* Set up the handler */
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    /* Install the handler for SIGINT */
    sigaction(SIGINT, &sa, NULL);

    printf("Press Ctrl-C while the program runs.\n");

    while (1) {
        update_data(4);
        sleep(3);
    }

    return 0;
}