#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/swap.h>
#include <errno.h>

void usage(const char *prog) {
    fprintf(stderr, "Usage: %s [on|off] <swap_device_or_file>\n", prog);
}

void print_swap_status() {
    FILE *fp = fopen("/proc/swaps", "r");
    if (!fp) {
        perror("fopen /proc/swaps");
        return;
    }

    char line[256];
    printf("\nCurrent swap status:\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    const char *action = argv[1];
    const char *target = argv[2];

    if (access(target, F_OK) != 0) {
        perror("Target path does not exist");
        return 1;
    }

    printf("=== Swap status before ===\n");
    print_swap_status();

    int ret;
    if (strcmp(action, "on") == 0) {
        ret = swapon(target, 0);
        if (ret != 0) {
            perror("swapon failed");
            return 1;
        }
        printf("\n✅ Swap activated on: %s\n", target);
    } else if (strcmp(action, "off") == 0) {
        ret = swapoff(target);
        if (ret != 0) {
            perror("swapoff failed");
            return 1;
        }
        printf("\n🛑 Swap deactivated on: %s\n", target);
    } else {
        usage(argv[0]);
        return 1;
    }

    printf("\n=== Swap status after ===\n");
    print_swap_status();

    return 0;
}
