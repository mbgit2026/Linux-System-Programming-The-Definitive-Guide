
#include <stdio.h> 
#include <unistd.h>
#include <fcntl.h>

int main() {


    printf("\nFD %d %s a terminal",  STDIN_FILENO,
            isatty(STDIN_FILENO) ? "is" : "is NOT");
    
    printf("\nFD %d %s a terminal",  STDOUT_FILENO,
            isatty(STDOUT_FILENO) ? "is" : "is NOT");
    
    printf("\nFD %d %s a terminal",  STDERR_FILENO,
            isatty(STDERR_FILENO) ? "is" : "is NOT");


    int fd = open("./file.txt", O_RDONLY); 
    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("\nFD %d %s a terminal",  fd,
            isatty(fd) ? "is" : "is NOT");
        

    return 0;

}