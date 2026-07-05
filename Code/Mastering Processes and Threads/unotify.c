#include <seccomp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/wait.h>

/* Install filter and return notification FD */
int install_notify_filter(int *notify_fd_out)
{
    scmp_filter_ctx ctx;
    int rc;
    int notify_fd;

    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) {
        fprintf(stderr, "seccomp_init failed\n");
        return -1;
    }

    rc = seccomp_rule_add(ctx, SCMP_ACT_NOTIFY, SCMP_SYS(getpid), 0);
    if (rc < 0) {
        fprintf(stderr, "seccomp_rule_add failed\n");
        seccomp_release(ctx);
        return -1;
    }

    rc = seccomp_load(ctx);
    if (rc < 0) {
        fprintf(stderr, "seccomp_load failed\n");
        seccomp_release(ctx);
        return -1;
    }

    /* MUST get notify FD BEFORE releasing ctx */
    notify_fd = seccomp_notify_fd(ctx);
    if (notify_fd < 0) {
        perror("seccomp_notify_fd");
        seccomp_release(ctx);
        return -1;
    }

    *notify_fd_out = notify_fd;

    seccomp_release(ctx);
    return 0;
}

int main(void)
{
    int notify_fd;
    pid_t pid;

    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
        perror("prctl");
        exit(EXIT_FAILURE);
    }

    if (install_notify_filter(&notify_fd) < 0) {
        fprintf(stderr, "Failed to install filter\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* Child triggers syscall */
        sleep(1);
        printf("Child calling getpid(): %d\n", getpid());
        _exit(EXIT_SUCCESS);
    }

    /* Parent = notification supervisor */
    struct seccomp_notif *req = NULL;
    struct seccomp_notif_resp *resp = NULL;

    if (seccomp_notify_alloc(&req, &resp) < 0) {
        perror("seccomp_notify_alloc");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for syscall notification...\n");

    if (seccomp_notify_receive(notify_fd, req) == 0) {
        printf("Got notification:\n");
        printf(" PID: %d\n", req->pid);
        printf(" Syscall: %d\n", req->data.nr);

        resp->id = req->id;
        resp->val = 0;
        resp->error = 0;
        resp->flags = 0;

        if (seccomp_notify_respond(notify_fd, resp) < 0) {
            perror("seccomp_notify_respond");
        }
    } else {
        perror("seccomp_notify_receive");
    }

    seccomp_notify_free(req, resp);

    wait(NULL);
    close(notify_fd);

    return 0;
}