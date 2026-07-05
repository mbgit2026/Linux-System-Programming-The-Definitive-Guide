#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(void)
{
    pid_t child;
    int pidfd, new_fd;

    child = fork();
    if (child == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child == 0) {
        int fd = open("child_output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            _exit(1);
        }

        dup2(fd, STDOUT_FILENO);
        printf("Message written by child\n");
        fflush(stdout);
        sleep(5); 
        _exit(0);
    }
    sleep (3);

    pidfd = syscall(SYS_pidfd_open, child, 0);
    if (pidfd == -1) {
        perror("pidfd_open");
        exit(EXIT_FAILURE);
    }

    new_fd = syscall(SYS_pidfd_getfd, pidfd, STDOUT_FILENO, 0);
    if (new_fd == -1) {
        perror("pidfd_getfd");
        exit(EXIT_FAILURE);
    }

    dprintf(new_fd, "Message written by parent via pidfd_getfd\n");
    close(new_fd);
    close(pidfd);
    waitpid(child, NULL, 0);
    return 0;
}
