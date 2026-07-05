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

void set_noncanonical(struct termios *newt) {
    newt->c_lflag &= ~(ICANON | ECHO);  // disable canonical mode and echo
    newt->c_cc[VMIN] = 1;               // read returns after 1 byte
    newt->c_cc[VTIME] = 0;              // no timeout

    if (tcsetattr(STDIN_FILENO, TCSANOW, newt) == -1) {
        perror("tcsetattr");
        exit(1);
    }
}

int main() {
    struct termios oldt, newt;
    char c;

    // Save current settings
    if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
        perror("tcgetattr");
        return 1;
    }

    newt = oldt;

    printf("=== Canonical mode ===\n");
    printf("Type something (input is line-buffered, press Enter):\n");

    // Canonical mode: getchar waits for newline
    while ((c = getchar()) != '\n' && c != EOF) {
        printf("Read char: %d ('%c')\n", c, c);
    }

    // Switch to noncanonical mode
    printf("\n=== Noncanonical mode ===\n");
    printf("Type characters (no Enter needed, Ctrl+D to exit):\n");

    set_noncanonical(&newt);

    while (1) {
        if (read(STDIN_FILENO, &c, 1) == 1) {
            printf("Read char: %d\n", c);

            if (c == 4) { // Ctrl+D
                break;
            }
        }
    }

    // Restore original settings
    set_canonical(&oldt);

    printf("\nTerminal restored.\n");
    return 0;
}