#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

void sigio_handler(int signo) {
    char buf[1024];
    int n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("[SIGIO handler] Received: %s", buf);
        fflush(stdout);
    }
}

int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]); 

        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sigio_handler;
        sigaction(SIGIO, &sa, NULL);

        fcntl(pipefd[0], F_SETOWN, getpid());

        int flags = fcntl(pipefd[0], F_GETFL);
        fcntl(pipefd[0], F_SETFL, flags | O_ASYNC);

        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        printf("Child waiting for input on pipe (PID=%d)\n", getpid());
        while (1) pause();
    } else {
        close(pipefd[0]); 

        sleep(1); 
        const char *msg1 = "hello via pipe\n";
        const char *msg2 = "second message\n";

        write(pipefd[1], msg1, strlen(msg1));
        sleep(1);
        write(pipefd[1], msg2, strlen(msg2));

        sleep(2);
        close(pipefd[1]);
        printf("Parent done.\n");
    }
    return 0;
}
