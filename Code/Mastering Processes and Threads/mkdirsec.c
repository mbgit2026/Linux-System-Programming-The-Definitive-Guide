#include <seccomp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

static void install_sandbox(void)
{
    scmp_filter_ctx ctx;

    ctx = seccomp_init(SCMP_ACT_ERRNO(EPERM));
    if (!ctx) {
        perror("seccomp_init");
        exit(1);
    }

    /* =========================
       Essential syscalls
       ========================= */

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0);

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

    /* Memory management */
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 0);

    /* =========================
       mkdir restrictions
       allow ONLY mode 0700
       ========================= */

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mkdir), 1, SCMP_CMP(1, SCMP_CMP_EQ, 0700));

    /* =========================
       openat restrictions
       allow ONLY read-only open
       ========================= */

    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 1, SCMP_CMP(2, SCMP_CMP_EQ, O_RDONLY));

    /* ========================= */

    if (seccomp_load(ctx) < 0) {
        perror("seccomp_load");
        exit(1);
    }

    seccomp_release(ctx);
}

int main(void)
{
    printf("Installing sandbox...\n");
    install_sandbox();

    printf("Sandbox installed\n");

    /* Allowed */
    printf("\n[+] mkdir 0700 should work\n");
    if (mkdir("/tmp/sandbox_ok", 0700) < 0)
        perror("mkdir ok");

    /* Blocked */
    printf("\n[-] mkdir 0755 should fail\n");
    if (mkdir("/tmp/sandbox_fail", 0755) < 0)
        perror("mkdir fail");

    /* Allowed */
    printf("\n[+] open read-only should work\n");
    int fd = open("/etc/passwd", O_RDONLY);
    if (fd < 0)
        perror("open ro");
    else
        close(fd);

    /* Blocked */
    printf("\n[-] open write should fail\n");
    fd = open("/tmp/test", O_WRONLY | O_CREAT, 0644);
    if (fd < 0)
        perror("open write");

    return 0;
}