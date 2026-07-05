#include <stdio.h>
#include <unistd.h>
#include <sys/capability.h>
#include <stdlib.h>

void print_caps(const char *label) {
    cap_t caps = cap_get_proc();
    char *txt = cap_to_text(caps, NULL);
    printf("[%s] UID=%d  Capabilities: %s\n", label, getuid(), txt);
    cap_free(caps);
    cap_free(txt);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <file>", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("=== Child ===\n");
    print_caps("Child");

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    printf("Successfully opened %s\n", argv[1]);
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp)) {
        printf("%s", buffer);
    }

    fclose(fp);
    exit(EXIT_SUCCESS);
}
