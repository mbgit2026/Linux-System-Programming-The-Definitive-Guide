#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
    printf("Environment variables:\n\n");

    for (int i = 0; envp[i] != NULL; i++) {
        printf("%s\n", envp[i]);
    }

    return 0;
}
