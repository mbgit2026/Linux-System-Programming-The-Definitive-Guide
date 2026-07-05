#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

jmp_buf return_to_top_level;
volatile sig_atomic_t waiting_for_input = 0;

void handle_sigint(int signum)
{
    waiting_for_input = 0;
    write(STDOUT_FILENO, "\nInterrupted\n", 13);
    longjmp(return_to_top_level, 1);
}

void prepare_for_command(void)
{
    printf("shell> ");
    fflush(stdout);
}

char *read_data(void)
{
    static char buffer[128];

    waiting_for_input = 1;

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        waiting_for_input = 0;
        return NULL;
    }

    waiting_for_input = 0;
    return buffer;
}

void read_and_execute_command(void)
{
    char *data = read_data();
    if (data != NULL) {
        printf("You entered: %s", data);
    }
}

int main(void)
{
    signal(SIGINT, handle_sigint);
    
    while (1) {
        prepare_for_command();

        if (setjmp(return_to_top_level) == 0) {
            read_and_execute_command();
        } 
        /* If we get here via longjmp, the loop restarts */

    }

    return EXIT_SUCCESS;
}