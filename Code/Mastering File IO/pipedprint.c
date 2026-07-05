#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {

    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) { 
        close(pipefd[0]); 
        pid_t pid = getpid();

        for (int i = 0; i < 5; i++) 
            dprintf(pipefd[1], "Message %d from parent PID %d\n", i, pid);
        
        close(pipefd[1]); 
        wait(NULL);      
    } else {        
        close(pipefd[1]); 
        char buffer[256];
        ssize_t n;

        while ((n = read(pipefd[0], buffer, sizeof(buffer)-1)) > 0) {
            buffer[n] = '\0'; // Null-terminate
            printf("\nChild received:\n%s", buffer);
        }
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }
    
    return 0;
}
