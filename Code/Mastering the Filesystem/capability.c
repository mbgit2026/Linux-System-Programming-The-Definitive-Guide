#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/capability.h>
#include <errno.h>
#include <string.h>
#include <sys/prctl.h>
#include <linux/securebits.h>

int main(int argc, char*argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <filename>", argv[0]);
        exit(EXIT_FAILURE);
    }

    cap_t caps = cap_get_proc();
    if (!caps) {
        perror("cap_get_proc");
        exit(EXIT_FAILURE);
    }

    cap_t current = cap_get_proc();
    char *caps_text = cap_to_text(current, NULL);
    printf("Current capabilities: %s\n", caps_text);
    cap_free(caps_text);
    cap_free(current);

    cap_value_t cap_list[] = { CAP_DAC_OVERRIDE };
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, CAP_SET) == -1) {
        perror("cap_set_flag");
        cap_free(caps);
        exit(EXIT_FAILURE);
    }

    if (cap_set_proc(caps) == -1) {
        perror("cap_set_proc");
        cap_free(caps);
        exit(EXIT_FAILURE);
    }

    cap_free(caps);

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    printf("Successfully opened %s!\n", argv[1]);

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }

    fclose(fp);
    exit(EXIT_SUCCESS);
}

