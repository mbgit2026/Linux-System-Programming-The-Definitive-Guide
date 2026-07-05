#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>

#define MAXLINE 1024

static pid_t shell_pgid;
static struct termios shell_tmodes;
static int shell_terminal;
static int shell_is_interactive;

/* Initialize shell job control */
void init_shell()
{
    shell_terminal = STDIN_FILENO;
    shell_is_interactive = isatty(shell_terminal);

    if (shell_is_interactive) {

        /* Loop until we are in foreground */
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
            kill(-shell_pgid, SIGTTIN);

        /* Ignore interactive signals */
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);

        /* Put shell in its own process group */
        shell_pgid = getpid();
        if (setpgid(shell_pgid, shell_pgid) < 0) {
            perror("Couldn't put shell in its own process group");
            exit(1);
        }

        /* Take control of terminal */
        tcsetpgrp(shell_terminal, shell_pgid);

        /* Save terminal modes */
        tcgetattr(shell_terminal, &shell_tmodes);
    }
}

/* Execute a single command in foreground */
void launch_process(char *cmd)
{
    pid_t pid = fork();

    if (pid == 0) {
        /* CHILD */

        /* Put child in new process group */
        setpgid(0, 0);

        /* Give child control of terminal */
        tcsetpgrp(shell_terminal, getpid());

        /* Restore default signals */
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);

        execlp(cmd, cmd, (char *)NULL);
        perror("exec failed");
        exit(1);
    }
    else if (pid < 0) {
        perror("fork failed");
        return;
    }

    /* PARENT */

    /* Set child's process group */
    setpgid(pid, pid);

    /* Give terminal to child */
    tcsetpgrp(shell_terminal, pid);

    /* Wait for foreground job */
    int status;
    waitpid(pid, &status, WUNTRACED);

    /* Restore shell as foreground process group */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Restore terminal modes */
    tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
}

int main()
{
    char line[MAXLINE];
    init_shell();
    while (1) {
        printf("mini-shell$ ");
        fflush(stdout);

        if (!fgets(line, MAXLINE, stdin))
            break;

        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0)
            break;

        if (strlen(line) > 0)
            launch_process(line);
    }
    return 0;
}