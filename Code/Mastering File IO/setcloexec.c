#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char *argv[])
{
    int fd;
    int flags;
    fd = open ("file.txt", O_RDONLY);

    flags = fcntl (fd, F_GETFD);
    //fcntl (fd, F_SETFD, flags | FD_CLOEXEC);
    
    execve ("./hello", NULL, NULL);

    return 0;
}