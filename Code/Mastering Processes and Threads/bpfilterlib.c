#include <seccomp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/prctl.h>

/*
 * Install a filter that:
 *  - Allows everything
 *  - Except one syscall -> returns errno
 */
static int install_filter(int syscall_nr, int f_errno)
{
    scmp_filter_ctx ctx;

    /* Default action: allow everything */
    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) {
        perror("seccomp_init");
        return -1;
    }

    /* Add rule: if syscall == syscall_nr -> return errno */
    if (seccomp_rule_add(ctx, SCMP_ACT_ERRNO(f_errno), syscall_nr, 0) < 0) {
        perror("seccomp_rule_add");
        seccomp_release(ctx);
        return -1;
    }
  /*   if (seccomp_rule_add(ctx, SCMP_ACT_LOG, SCMP_SYS(getpid), 0) < 0) {
        perror("seccomp_rule_add");
        seccomp_release(ctx);
        exit(EXIT_FAILURE);
    }
*/

    /* Load filter into kernel */
    if (seccomp_load(ctx) < 0) {
        perror("seccomp_load");
        seccomp_release(ctx);
        return -1;
    }

    seccomp_release(ctx);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <syscall_nr> <errno> <prog> [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int syscall_nr = strtol(argv[1], NULL, 0);
    int err = strtol(argv[2], NULL, 0);

    /* Required before installing seccomp filters */
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
        perror("prctl");
        exit(EXIT_FAILURE);
    }

    if (install_filter(syscall_nr, err) < 0)
        exit(EXIT_FAILURE);

   // printf("\nPID: %d", getpid());

    execv(argv[3], &argv[3]);
    perror("execv");
    return EXIT_FAILURE;
}