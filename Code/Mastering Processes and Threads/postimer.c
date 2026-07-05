#include <err.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

static void print_siginfo(siginfo_t *si)
{
    int      or;
    timer_t  *tidp;

    tidp = si->si_value.sival_ptr;

    printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
    printf("    *sival_ptr = %#jx\n", (uintmax_t) *tidp);

    or = timer_getoverrun(*tidp);
    if (or == -1)
        err(EXIT_FAILURE, "timer_getoverrun");

    printf("    overrun count = %d\n", or);
}

static void handler(int sig, siginfo_t *si, void *uc)
{
    printf("Caught signal %d\n", sig);
    print_siginfo(si);
    signal(sig, SIG_IGN);
}

int main(int argc, char *argv[])
{
    timer_t            timerid;
    sigset_t           mask;
    long long          freq_nanosecs;
    struct sigevent    sev;
    struct sigaction   sa;
    struct itimerspec  its;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <sleep-secs> <freq-nanosecs>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Establishing handler for signal %d\n", SIG);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) == -1)
        err(EXIT_FAILURE, "sigaction");

    /* Block timer signal temporarily.  */
    printf("Blocking signal %d\n", SIG);
    sigemptyset(&mask);
    sigaddset(&mask, SIG);
    if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
        err(EXIT_FAILURE, "sigprocmask");

    /* Create the timer.  */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCKID, &sev, &timerid) == -1)
        err(EXIT_FAILURE, "timer_create");

    printf("timer ID is %#jx\n", (uintmax_t) timerid);

    /* Start the timer.  */
    freq_nanosecs = atoll(argv[2]);
    its.it_value.tv_sec = freq_nanosecs / 1000000000;
    its.it_value.tv_nsec = freq_nanosecs % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if (timer_settime(timerid, 0, &its, NULL) == -1)
        err(EXIT_FAILURE, "timer_settime");

    /*
    struct itimerspec  curr;  
    if (timer_gettime(timerid, &curr) == -1)
	    err(EXIT_FAILURE, "timer_gettime");
    printf("Time until next expiration: %ld sec %ld nsec\n", curr.it_value.tv_sec, curr.it_value.tv_nsec);
    */

    /* Sleep for a while; meanwhile, the timer may expire multiple times.  */
    printf("Sleeping for %d seconds\n", atoi(argv[1]));
    sleep(atoi(argv[1]));

    /* Unlock the timer signal, so that timer notification can be delivered.  */
    printf("Unblocking signal %d\n", SIG);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
        err(EXIT_FAILURE, "sigprocmask");

    /*
    if (timer_delete(timerid) == -1)
	    err(EXIT_FAILURE, "timer_delete");
    */
    exit(EXIT_SUCCESS);
}   
