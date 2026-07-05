#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_disposition(int signo)
{
    struct sigaction oldact;

    if (sigaction(signo, NULL, &oldact) == -1) {
        perror("sigaction");
        return;
    }

    printf("Signal %d (%s): ", signo, strsignal(signo));

    if (oldact.sa_handler == SIG_DFL) 
        printf("SIG_DFL");
    else if (oldact.sa_handler == SIG_IGN) 
        printf("SIG_IGN");
    else
        printf("custom handler installed");
    
    printf(", flags: 0x%lx\n", (unsigned long) oldact.sa_flags);
}

int main(void)
{
    int signals[] = { SIGINT, SIGTERM, SIGSEGV, SIGPIPE };
    size_t i;

    for (i = 0; i < sizeof(signals)/sizeof(signals[0]); i++) 
        print_disposition(signals[i]);

    return 0;
}