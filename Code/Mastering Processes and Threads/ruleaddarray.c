#include <seccomp.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main() {
    scmp_filter_ctx ctx;

    /* Default: deny */
    ctx = seccomp_init(SCMP_ACT_ERRNO(EPERM));
    if (!ctx) {
        perror("seccomp_init");
        return 1;
    }

    /* Allow exit */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

    /*
        Build comparison array:
        write(fd == STDOUT AND len < 20)
    */
    struct scmp_arg_cmp cmp[2];

    /* arg0 = fd == STDOUT */
    cmp[0].arg = 0;
    cmp[0].op = SCMP_CMP_EQ;
    cmp[0].datum_a = STDOUT_FILENO;
    cmp[0].datum_b = 0;

    /* arg2 = len < 20 */
    cmp[1].arg = 2;
    cmp[1].op = SCMP_CMP_LT;
    cmp[1].datum_a = 20;
    cmp[1].datum_b = 0;

    if (seccomp_rule_add_array(ctx,
                               SCMP_ACT_ALLOW,
                               SCMP_SYS(write),
                               2,
                               cmp) < 0) {
        perror("seccomp_rule_add_array");
        seccomp_release(ctx);
        return 1;
    }

    /* Load filter */
    if (seccomp_load(ctx) < 0) {
        perror("seccomp_load");
        seccomp_release(ctx);
        return 1;
    }

    /* Test allowed */
    write(STDOUT_FILENO, "Hello OK\n", 9);

    /* Test blocked (too big) */
    char big[50] = {0};
    write(STDOUT_FILENO, big, sizeof(big));

    seccomp_release(ctx);
    return 0;
}