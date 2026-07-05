#include <stdio.h> 
#include <unistd.h>
#include <fcntl.h>

int main() {

    if (isatty(STDIN_FILENO))
        printf("\nFD %d is a terminal", STDIN_FILENO);
    if (isatty(STDOUT_FILENO))
        printf("\nFD %d is a terminal", STDOUT_FILENO);
    if (isatty(STDERR_FILENO))
        printf("\nFD %d is a terminal", STDERR_FILENO);
    
    int fd = open("./file.txt", O_RDONLY); 
        if (fd == -1) {
        perror("open");
        return 1;
    }

    if (isatty(fd))
        printf("\nFD: %d is a terminal", fd);
    else 
        printf("\nFD: %d not a terminal", fd);

    

    return 0;

}