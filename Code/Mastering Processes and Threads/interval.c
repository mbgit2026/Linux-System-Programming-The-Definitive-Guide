#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <wait.h>

volatile sig_atomic_t timer_fired = 0;

void timer_handler(int signum) {
    timer_fired = 1;
}

int main() {
    struct itimerval timer;
    struct sigaction sa;
    sigset_t blockmask, oldmask;

    sigemptyset(&blockmask);
    sigaddset(&blockmask, SIGALRM);
    sigprocmask(SIG_BLOCK, &blockmask, &oldmask);
       
    sa.sa_handler = timer_handler;
    sa.sa_mask = blockmask;
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);
    
    // First expiration after 2 seconds
    timer.it_value.tv_sec = 2;
    timer.it_value.tv_usec = 0;

    // Interval for periodic timer (every 2 seconds)
    timer.it_interval.tv_sec = 2;
    timer.it_interval.tv_usec = 0;

    // Start the timer
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        perror("setitimer");
        exit(1);
    }

    while (1) {
        while (!timer_fired)
            sigsuspend(&oldmask);

        if (timer_fired)
        {
            timer_fired = 0;
            printf("\033[H\033[J");
            fflush(stdout);

            pid_t child = fork();
            if (child < 0) 
                perror("fork");
            else if (child == 0) {
                printf("Every 2 seconds: ls\n\n");
                execl("/usr/bin/ls", "ls", NULL);
            _exit(1);
            } 
            wait(NULL);
        }    
    }
    return 0;
}