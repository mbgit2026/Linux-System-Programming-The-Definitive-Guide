#include <stdio.h>
#include <seccomp.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <syscall_number>\n", argv[0]);
        return 1;
    }

    int num = atoi(argv[1]);
    const char *name = seccomp_syscall_resolve_num_arch(SCMP_ARCH_X86_64, num);

    if (name)
        printf("Syscall %d -> %s\n", num, name);
    else
        printf("Syscall %d -> UNKNOWN (not valid for this arch)\n", num);

    return 0;
}
