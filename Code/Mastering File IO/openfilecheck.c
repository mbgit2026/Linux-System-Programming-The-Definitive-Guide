#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd;
    fd = open("./newfile.txt", O_RDONLY);

   // fd = open("./newfile.txt", O_RDONLY | O_CREAT, S_IRWXU);

    //fd=open("./newfile.txt",O_RDONLY|O_CREAT|O_EXCL, S_IRWXU);

    if (fd == -1)
        perror("open");
}