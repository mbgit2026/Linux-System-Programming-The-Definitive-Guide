#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main() {
    struct winsize ws;

    // Set desired window size
    ws.ws_row = 30;
    ws.ws_col = 100;
    ws.ws_xpixel = 0;
    ws.ws_ypixel = 0;

    // Apply new size to terminal
    if (ioctl(STDOUT_FILENO, TIOCSWINSZ, &ws) == -1) {
        perror("ioctl TIOCSWINSZ");
        return 1;
    }

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl TIOCGWINSZ");
        return 1;
    }

    printf("Terminal size changed to %d:%d\n", ws.ws_row, ws.ws_col);

    return 0;
}