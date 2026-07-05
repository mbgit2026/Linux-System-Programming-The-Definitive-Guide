#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
   // int dirfd = open("/bin", O_RDONLY | O_DIRECTORY);
   // if (dirfd == -1) {
    int fd = open("/bin/echo", O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char *argv[] = { "echo", "Hello from execveat", NULL };
    char *envp[] = { "PATH=/bin:/usr/bin", NULL };

   //  if (execveat(dirfd, "echo", argv, envp, 0) == -1) {
   //  if (execveat(AT_FDCWD, "/bin/echo", argv, envp, 0) == -1) {
    if (execveat(fd, "", argv, envp, AT_EMPTY_PATH) == -1) {
        perror("execveat");
        exit(EXIT_FAILURE);
    }

    return 0;

    



}
