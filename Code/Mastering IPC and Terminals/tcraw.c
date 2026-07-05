#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

void set_canonical(struct termios *oldt) {
    if (tcsetattr(STDIN_FILENO, TCSANOW, oldt) == -1) {
        perror("tcsetattr");
        exit(1);
    }
}

int main() {
    struct termios oldt, newt;
    int c;

    if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
        perror("tcgetattr");
        return 1;
    }

    newt = oldt;
    printf("\n=== Raw mode ===\n");
    printf("Type characters (no Enter needed, Ctrl+D to exit):\n");
    cfmakeraw(&newt);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1) {
        perror("tcsetattr");
        exit(1);
    }

    while (1) {
        if (read(STDIN_FILENO, &c, 1) == 1) {
            printf("Read char: %d\n", c);

            if (c == 4) { // Ctrl+D
                break;
            }
        }
    }

    set_canonical(&oldt);
    printf("\nTerminal restored.\n");
    return 0;
}