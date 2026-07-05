#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("\nUsage: %s <syscall_number>");
        return 1;
    }

    int n = syscall(atoi(argv[1]));

    if (n == -1){
        perror("syscall");
        return 1;
    }
    return 0;

}