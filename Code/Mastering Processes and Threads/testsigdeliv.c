#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t flag = 0;

void handler(int sig)
{
    flag = 1;
}

int main(void)
{
    sigset_t block_alarm;
    struct sigaction sa;

    /* Install signal handler */
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    /* Initialize the signal mask. */
    sigemptyset(&block_alarm);
    sigaddset(&block_alarm, SIGALRM);

    alarm(3);

    while (1)
    {
        /* Check if a signal has arrived; if so, reset the flag. */
        sigprocmask(SIG_BLOCK, &block_alarm, NULL);

        if (flag)
        {
            write(STDOUT_FILENO, "ALRM received\n", 14);
            flag = 0;
            alarm(3);
        }
        else
        {
            // action if not received
        }

        sigprocmask(SIG_UNBLOCK, &block_alarm, NULL);
        sleep (1);

        /* Other work */
    }
}