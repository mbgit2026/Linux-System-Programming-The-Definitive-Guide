#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

void handle_winch(int sig) {
    struct winsize ws;

    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl");
        return;
    }

    printf("\nSIGWINCH received: new size = %d rows, %d cols\n",
           ws.ws_row, ws.ws_col);

    fflush(stdout);
}

int main() {
    struct sigaction sa;

    sa.sa_handler = handle_winch;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGWINCH, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("Resize the terminal window to trigger SIGWINCH...\n");

    while (1) {
        pause();  // wait for signals
    }

    return 0;
}