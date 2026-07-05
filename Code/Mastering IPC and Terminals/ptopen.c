#include <unistd.h> 
#include <fcntl.h> 
#include <stdio.h>

int main() {

    int fd = open("/dev/ptmx", O_RDWR);
    if (fd < 0)
        perror("open");

    printf("FD: %d", fd);
    sleep (60);
    return 0;
}