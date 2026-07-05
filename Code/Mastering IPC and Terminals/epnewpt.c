#include "epollnewpt.h"
//#include <pty.h>

static int pidfd_open(pid_t pid) {
    return syscall(SYS_pidfd_open, pid, 0);
}

int main() {
    int master, slave;

    if (openpty(&master, &slave, NULL, NULL, NULL) == -1) {
    if (open_pty_pair(&master, &slave) == -1) {
        perror("open_pty_pair");
        exit(1);
    }

    // non-blocking I/O
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    fcntl(master, F_SETFL, O_NONBLOCK);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(1);
    }

    struct epoll_event ev, events[MAX_EVENTS];

    // monitor stdin
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

    // monitor PTY master
    ev.events = EPOLLIN;
    ev.data.fd = master;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, master, &ev);

    printf("PTY-Shell Ready.\n> ");
    fflush(stdout);

    char cmd_buf[MAX_CMD];
    int cmd_ptr = 0;

    int current_pidfd = -1;
    pid_t current_pid = -1;

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            // =========================
            // CHILD EXIT (pidfd)
            // =========================
            if (fd == current_pidfd) {
                waitpid(current_pid, NULL, 0);

                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_pidfd, NULL);
                close(current_pidfd);

                current_pidfd = -1;
                current_pid = -1;

                printf("\n> ");
                fflush(stdout);
                continue;
            }

            // =========================
            // USER INPUT
            // =========================
            if (fd == STDIN_FILENO) {
                char c;
                ssize_t n = read(STDIN_FILENO, &c, 1);

                if (n <= 0) continue;

                if (c == '\n') {
                    cmd_buf[cmd_ptr] = '\0';

                    if (cmd_ptr == 0) {
                        printf("> ");
                        fflush(stdout);
                        continue;
                    }

                    if (strcmp(cmd_buf, "exit") == 0) {
                        goto cleanup;
                    }

                    char *argv[MAX_ARGS];
                    parse_command(cmd_buf, argv);

                    pid_t pid = fork();

                    if (pid == 0) {
                        // CHILD

                        dup2(slave, STDIN_FILENO);
                        dup2(slave, STDOUT_FILENO);
                        dup2(slave, STDERR_FILENO);

                        execvp(argv[0], argv);
                        perror("execvp");
                        exit(1);
                    }

                    // PARENT: track process
                    current_pid = pid;
                    current_pidfd = pidfd_open(pid);

                    if (current_pidfd == -1) {
                        perror("pidfd_open");
                        exit(1);
                    }

                    ev.events = EPOLLIN;
                    ev.data.fd = current_pidfd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, current_pidfd, &ev);

                    cmd_ptr = 0;
                } else {
                    if (cmd_ptr < MAX_CMD - 1) {
                        cmd_buf[cmd_ptr++] = c;
                    }
                }
            }

            // =========================
            // PTY OUTPUT
            // =========================
            if (fd == master) {
                char buf[1024];

                while (1) {
                    ssize_t n = read(master, buf, sizeof(buf));

                    if (n > 0) {
                        write(STDOUT_FILENO, buf, n);
                    } else if (n == -1 && errno == EAGAIN) {
                        break;
                    } else {
                        break;
                    }
                }
            }
        }
    }

cleanup:
    close(master);
    close(slave);
    close(epoll_fd);
    return 0;
}