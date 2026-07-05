#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXTRA_VARS 2

int count_env(char *envp[]) {
    int count = 0;
    while (envp[count] != NULL) count++;
    return count;
}

int main(int argc, char *argv[], char *envp[]) {

    if (argc < 2) {
        printf("\nUsage: %s <var> [value]", argv[0]);
        exit(EXIT_FAILURE);
    }

    int env_count = count_env(envp);
    char *result;
    int total_env = env_count + EXTRA_VARS;
    char **new_envp = malloc((total_env + 1) * sizeof(char *)); // +1 for NULL terminator
    if (!new_envp) {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < env_count; i++) {
        new_envp[i] = envp[i];
    }

    if (argv[2]) {
        size_t len = strlen(argv[1]) + strlen("=") + strlen(argv[2]) + 1;
        result = malloc(len);
        if (!result) {
            perror("malloc");
            return 1;
        }

        strcpy(result, argv[1]);
        strcat(result, "=");
        strcat(result, argv[2]);
    }

    new_envp[env_count]     = argv[2] ? result : argv[1];
    new_envp[env_count + 1] = "DEBUG=1";
    new_envp[env_count + 2] = NULL;

    char *program = "./getvar";
    char *const new_argv[] = { "./getvar", argv[1], NULL };

    if (execve(program, new_argv, new_envp) == -1) {
        perror("execve");
        free(new_envp); // cleanup only reached if execve fails
        return 1;
    }

    exit(EXIT_SUCCESS); // never reached
}
