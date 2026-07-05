#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>


int open_pty_pair (int *amaster, int *aslave) {

    int master, slave;
    char *name;

    master = posix_openpt (O_RDWR | O_NOCTTY);
    if (master < 0)
        return -1;
    
    if (grantpt (master) < 0 || unlockpt (master) < 0) {
        close(master);
        return -1;
    }

    name = ptsname (master);
    if (name == NULL) {
        close(master);
        return -1;
    }
    
    slave = open (name, O_RDWR);
    if (slave == -1) {
        close(master);
        return -1; 
    }

    *amaster = master;
    *aslave = slave;
    
    return 0;
}

int main () {
    int master, slave;
    
    if (open_pty_pair(&master, &slave) == -1) {
        perror("open_pty_pair");
        exit(EXIT_FAILURE);
    }

    printf("master fd: %d, slave fd: %d\n", master, slave);
    sleep(60);
    return 0;
}