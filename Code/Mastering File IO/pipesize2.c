#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {

    int pipefd[2];
    pid_t pid;
	char *buffer;
    int bs;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int psize = fcntl(pipefd[0], F_GETPIPE_SZ);
    bs = psize * 2;
    buffer = malloc(bs);
    memset(buffer, 'A', bs); 

    pid = fork();

	if (pid > 0) {
        printf("\nFrom parent: Pipe size: %d", psize);
        
        ssize_t ret = write(pipefd[1], buffer, bs);
        if (ret == -1) 
            perror("write");
        else {
            printf("\nFrom parent: written %zd bytes\n", ret);
            close(pipefd[1]);
        }

        wait(NULL);

        printf("From parent: child finished, draining pipe...\n");

        char buf[4096];
        ssize_t n;
        while ((n = read(pipefd[0], buf, sizeof buf)) > 0) 
            printf("Parent drained %zd bytes still in the pipe\n", n);
        
        if (n == 0) 
            printf("Pipe is now empty\n");
        else if (n < 0) 
            perror("read");
	
	}
	else if (pid == 0) {
        close(pipefd[1]);        
		ssize_t nread;
         
        while ((nread = read (pipefd[0], buffer, bs)) > 0) {
            if (nread == -1)
                perror("read");
            else 
		        printf ("\nFrom child: Buffer received: %d\n", nread);
        }
        close(pipefd[0]);

    }

    free(buffer);
    exit(EXIT_SUCCESS);
}