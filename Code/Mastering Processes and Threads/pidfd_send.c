#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int        pidfd, sig;
    char       path[PATH_MAX];
    siginfo_t  info;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <pid> <signal>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sig = atoi(argv[2]);
    snprintf(path, sizeof(path), "/proc/%s", argv[1]);
    pidfd = open(path, O_RDONLY);
    if (pidfd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    memset(&info, 0, sizeof(info));
    info.si_code = SI_QUEUE;
    info.si_signo = sig;
    info.si_errno = 0;
    info.si_uid = getuid();
    info.si_pid = getpid();
    info.si_value.sival_int = 1234;

    if (syscall(SYS_pidfd_send_signal, pidfd, sig, &info, 0) == -1) {
        perror("pidfd_send_signal");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
