#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <linux/securebits.h>
#include <errno.h>
#include <string.h>

void print_caps(const char *label) {
    cap_t caps = cap_get_proc();
    char *txt = cap_to_text(caps, NULL);
    printf("[%s] UID=%d  Capabilities: %s\n", label, getuid(), txt);
    cap_free(caps);
    cap_free(txt);
}

int main(int argc, char *argv[]) {
    if (getuid() != 0) {
        fprintf(stderr, "Must run as root!\n");
        exit(EXIT_FAILURE);
    }

    int use_securebits = (argc > 1 && strcmp(argv[1], "--securebits") == 0);

    printf("=== Launcher (running as root) ===\n");
    printf("=== Current Capabilities ===\n");

    print_caps("Initial");
    if (use_securebits) {
        printf("Setting securebits...\n");
        int bits = SECBIT_NO_SETUID_FIXUP | SECBIT_NO_SETUID_FIXUP_LOCKED;
        if (prctl(PR_SET_SECUREBITS, bits) != 0) {
            perror("prctl(PR_SET_SECUREBITS)");
            exit(EXIT_FAILURE);
        }
    }

    print_caps("Before drop");

    printf("Dropping to UID 1000...\n");
    if (setuid(1000) != 0) {
        perror("setuid(1000)");
        exit(EXIT_FAILURE);
    }

    print_caps("After drop");
    printf("Attempting to regain UID 0...\n");
    if (setuid(0) != 0) {
        perror("setuid(0)");
    }

    print_caps("After regain");
    char *args[] = {"./securebits_child", "/etc/shadow", NULL};
    execv("./securebits_child", args);
    perror("execv");
    exit(EXIT_FAILURE);
}
