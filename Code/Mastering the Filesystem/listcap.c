#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <linux/capability.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void print_caps_text(const char *label, cap_flag_t flag, cap_t caps) {
    char *text = cap_to_text(caps, NULL);
    if (text) {
        printf("%s: %s\n", label, text);
        cap_free(text);
    } else {
        perror("cap_to_text");
    }
}

void print_ambient_caps() {
    printf("Ambient: ");
    int any = 0;
    for (int cap = 0; cap <= CAP_LAST_CAP; ++cap) {
        int res = prctl(PR_CAP_AMBIENT, PR_CAP_AMBIENT_IS_SET, cap, 0, 0);
        if (res == 1) {
            char *name = cap_to_name(cap);
            if (name) {
                printf("%s ", name);
                cap_free(name);
            } else {
                printf("cap%d ", cap);
            }
            any = 1;
        }
    }
    if (!any)
        printf("(none)");
    printf("\n");
}

int main() {
    cap_t caps = cap_get_proc();
    if (!caps) {
        perror("cap_get_proc");
        exit(EXIT_FAILURE);
    }

    printf("=== Current Capabilities ===\n");

    for (cap_flag_t flag = 0; flag <= 2; flag++) {
        const char *label = (flag == CAP_EFFECTIVE) ? "Effective" :
                            (flag == CAP_PERMITTED) ? "Permitted" :
                            (flag == CAP_INHERITABLE) ? "Inheritable" : "Unknown";

        cap_flag_value_t val;
        printf("%s:\n", label);
        for (int i = 0; i <= CAP_LAST_CAP; ++i) {
            if (cap_get_flag(caps, i, flag, &val) == 0 && val == CAP_SET) {
                char *name = cap_to_name(i);
                if (name) {
                    printf("  %s\n", name);
                    cap_free(name);
                } else {
                    printf("  cap%d\n", i);
                }
            }
        }
    }

    // Bounding set (requires /proc)
    printf("Bounding:\n");
    
    int any = 0;
    for (int cap = 0; cap <= CAP_LAST_CAP; ++cap) {
        //int res = prctl(PR_CAPBSET_READ,cap, 0, 0);
        int res = cap_get_bound(cap);
        if (res == 1) {
            char *name = cap_to_name(cap);
            if (name) {
                printf("%s\n", name);
                cap_free(name);
            } else {
                printf("cap%d ", cap);
            }
            any = 1;
        }
    }
    if (!any)
        printf("(none)");
    printf("\n");
    
    /*for (int i = 0; i <= CAP_LAST_CAP; ++i) {
        char path[64];
        snprintf(path, sizeof(path), "/proc/self/status");

        FILE *fp = fopen(path, "r");
        if (!fp) continue;

        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "CapBnd:", 7) == 0) {
                unsigned long long capbnd;
                sscanf(line + 7, "%llx", &capbnd);
                if (capbnd & (1ULL << i)) {
                    char *name = cap_to_name(i);
                    if (name) {
                        printf("  %s\n", name);
                        cap_free(name);
                    } else {
                        printf("  cap%d\n", i);
                    }
                }
                break;
            }
        }
        fclose(fp);
    } */

    // Ambient
    print_ambient_caps();

    cap_free(caps);
    return 0;
}
