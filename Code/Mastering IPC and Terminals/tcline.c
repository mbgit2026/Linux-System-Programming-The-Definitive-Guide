#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

int main() {
    struct termios oldt, newt;
    int c;

    // Get current terminal settings
    if (tcgetattr(STDIN_FILENO, &oldt) == -1) {
        perror("tcgetattr");
        return 1;
    }

    newt = oldt;

    // Disable canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);

    // Apply new settings
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1) {
        perror("tcsetattr");
        return 1;
    }

    printf("Noncanonical mode. Type characters (q to quit).\n");

    while ((c = getchar()) != 'q') {
        printf("Read: %c (%d)\n", c, c);

        if (c == 's') {
            printf("Stopping output for 3 seconds...\n");
            tcflow(STDOUT_FILENO, TCOOFF);  // suspend output
            sleep(3);
            tcflow(STDOUT_FILENO, TCOON);   // resume output
            printf("Output resumed.\n");
        }

        if (c == 'f') {
            printf("Flushing input buffer...\n");
            tcflush(STDIN_FILENO, TCIFLUSH);
        }
    }

    // Restore original settings
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) == -1) {
        perror("tcsetattr");
        return 1;
    }

    printf("Terminal restored.\n");
    return 0;
}