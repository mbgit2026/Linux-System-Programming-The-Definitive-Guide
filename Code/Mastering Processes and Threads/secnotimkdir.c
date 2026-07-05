#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <wait.h>

static int seccomp_syscall(unsigned int op, unsigned int flags, void *args) {
    return syscall(SYS_seccomp, op, flags, args);
}

static int install_filter() {

    struct sock_filter filter[] = {
        /* Load arch */
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, arch)),
        /* If arch != x86_64 → kill */
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 1, 0),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
        /* Load syscall number */
        BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(struct seccomp_data, nr)),
        /* If syscall == mkdir → notify */
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, SYS_mkdir, 0, 1),
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_USER_NOTIF),
        /* Else allow */
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
    };

    struct sock_fprog prog = {
        .len = sizeof(filter)/sizeof(filter[0]),
        .filter = filter,
    };

    return seccomp_syscall(SECCOMP_SET_MODE_FILTER, SECCOMP_FILTER_FLAG_NEW_LISTENER,&prog);
}

int main() {

    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
        perror("prctl");
        exit(1);
    }

    int notify_fd = install_filter();
    if (notify_fd < 0) {
        perror("seccomp");
        exit(1);
    }

    printf("Listener FD = %d\n", notify_fd);

    pid_t pid = fork();
    if (pid == 0) {
        sleep(1);
        printf("Child calling mkdir()\n");
        mkdir("/tmp/seccomp_test", 0700);
        printf("Child done\n");
        exit(0);
    }

    /* Supervisor */

    struct seccomp_notif_sizes sizes;
    seccomp_syscall(SECCOMP_GET_NOTIF_SIZES, 0, &sizes);

    struct seccomp_notif *req = malloc(sizes.seccomp_notif);
    struct seccomp_notif_resp *resp = malloc(sizes.seccomp_notif_resp);

    while (1) {
        memset(req, 0, sizes.seccomp_notif);
        if (ioctl(notify_fd, SECCOMP_IOCTL_NOTIF_RECV, req) < 0) {
            if (errno == EINTR) continue;
            perror("NOTIF_RECV");
            break;
        }

        printf("Supervisor: mkdir requested by pid %d\n", req->pid);
        resp->id = req->id;
        resp->val = 0;
        resp->error = -EPERM;   /* Deny syscall */
        resp->flags = 0;

        if (ioctl(notify_fd, SECCOMP_IOCTL_NOTIF_SEND, resp) < 0) {
            perror("NOTIF_SEND");
            break;
        } else 
            break;
    }


    return 0;
}