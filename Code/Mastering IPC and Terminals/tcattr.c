#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>


int main() {
    struct termios oldt, newt;


    if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
        perror("tcgetattr");
        return 1;
    }

    newt = oldt;
    newt.c_cc[VINTR] = _POSIX_VDISABLE;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1) {
        perror("tcsetattr");
        return 1;
    }

    printf("Ctrl+C is now disabled. Press Enter to continue...\n");
    getchar();

    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) == -1) {
        perror("tcsetattr");
        return 1;
    }

    printf("Ctrl+C restored.\n"); 
    return 0;
}