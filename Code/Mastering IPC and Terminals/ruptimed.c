#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <sys/stat.h>

#define PORT 12345
#define QLEN 10
#define BUFLEN 128

/* Daemonize the process */
void daemonize(void) {
    pid_t pid;

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS); // parent exits

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    signal(SIGHUP, SIG_IGN);

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);
    if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);
    chdir("/");

    for (int i = 0; i < 64; i++)
        close(i);

    open("/dev/null", O_RDWR); // stdin
    dup(0); // stdout
    dup(0); // stderr
}

/* Create and bind server socket */
int initserver(void) {
    int sockfd;
    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        return -1;

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        goto err;

    if (listen(sockfd, QLEN) < 0)
        goto err;

    return sockfd;

err:
    close(sockfd);
    return -1;
}

/* Get uptime from /proc/uptime */
void get_uptime(char *buf, size_t len) {
    FILE *fp = fopen("/proc/uptime", "r");
    if (!fp) {
        snprintf(buf, len, "error: %s\n", strerror(errno));
        return;
    }

    double uptime;
    if (fscanf(fp, "%lf", &uptime) != 1) {
        snprintf(buf, len, "error reading uptime\n");
        fclose(fp);
        return;
    }
    fclose(fp);

    int days = uptime / 86400;
    int hours = ((int)uptime % 86400) / 3600;
    int mins = ((int)uptime % 3600) / 60;

    snprintf(buf, len, "uptime: %d days, %02d:%02d\n", days, hours, mins);
}

/* Serve clients */
void serve(int sockfd) {
    int clfd;
    char buf[BUFLEN];

    for (;;) {
        clfd = accept(sockfd, NULL, NULL);
        if (clfd < 0) {
            syslog(LOG_ERR, "accept error: %s", strerror(errno));
            continue;
        }

        get_uptime(buf, sizeof(buf));

        if (send(clfd, buf, strlen(buf), 0) < 0) {
            syslog(LOG_ERR, "send error: %s", strerror(errno));
        }

        close(clfd);
    }
}


/*
void serve(int sockfd) {
    int clfd;

    for (;;) {
        clfd = accept(sockfd, NULL, NULL);
        if (clfd < 0) {
            syslog(LOG_ERR, "accept error: %s", strerror(errno));
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            syslog(LOG_ERR, "fork error: %s", strerror(errno));
            close(clfd);
            continue;
        }

        if (pid == 0) { // child
            close(sockfd); 

            dup2(clfd, STDOUT_FILENO);
            dup2(clfd, STDERR_FILENO);
            close(clfd);

            // Option 1: use printf
            // char buf[128];
            // get_uptime(buf, sizeof(buf));
            // printf("%s", buf);

            // Option 2 
            execl("/usr/bin/uptime", "uptime", (char *)NULL);
            perror("execl");
            exit(1);
        } else { // parent
            close(clfd); 
        }
    }
}
*/

int main(void) {
    int sockfd;

    openlog("ruptimed", LOG_PID, LOG_DAEMON);

    daemonize();

    sockfd = initserver();
    if (sockfd < 0) {
        syslog(LOG_ERR, "initserver failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "server started on port %d", PORT);

    serve(sockfd);

    close(sockfd);
    closelog();
    return 0;
}