#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <time.h>
#include <errno.h>

static volatile sig_atomic_t got_sigint = 0;

void handler(int sig) {
    if (sig == SIGINT) {
        got_sigint = 1;
    }
}

int main(void) {
    sigset_t mask;
    struct sigaction sa;
    struct pollfd fds[1];
    struct timespec ts = { .tv_sec = 5, .tv_nsec = 0 };


    // Install SIGINT handler
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // Empty mask → signals are not blocked during ppoll
    sigemptyset(&mask);

    // Dummy fd (stdin)
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;


    printf("Process PID = %d\n", getpid());
    printf("Try sending SIGINT (Ctrl-C) to terminate\n\n");

    while (1) {
        int ret = ppoll(fds, 1, &ts, &mask);

        if (ret == -1) {
            if (errno == EINTR) {
                // Signal interrupted ppoll
                goto handle_signals;
            }
            perror("ppoll");
            exit(1);
        } else if (ret == 0) {
            printf("ppoll timeout...\n");
        } else {
            if (fds[0].revents & POLLIN) {
                char buf[128];
                ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
                if (n > 0) {
                    buf[n] = '\0';
                    printf("Read: %s", buf);
                }
            }
        }

    handle_signals:
        if (got_sigint) {
            printf("SIGINT received → exiting gracefully\n");
            break;
        }
    }

    printf("Goodbye!\n");
    return 0;
}



