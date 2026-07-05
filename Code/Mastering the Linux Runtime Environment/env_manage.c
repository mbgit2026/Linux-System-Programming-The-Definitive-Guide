#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[]) {
    if (argc < 2 || (argc != 3 && strcmp(argv[1], "print") != 0)) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s add <VAR_NAME> <VALUE>\n", argv[0]);
        fprintf(stderr, "  %s change <VAR_NAME> <NEW_VALUE>\n", argv[0]);
        fprintf(stderr, "  %s delete <VAR_NAME>\n", argv[0]);
        fprintf(stderr, "  %s print\n", argv[0]);
        return 1;
    }

    const char *action = argv[1];
    const char *var_name = argv[2];
    const char *var_value = (argc >= 4) ? argv[3] : NULL;

    if (strcmp(action, "add") == 0) {
        if (var_value == NULL) {
            fprintf(stderr, "Missing value for 'add'\n");
            return 1;
        }

        if (getenv(var_name)) {
            printf("Variable %s already exists: %s\n", var_name, getenv(var_name));
        } else {
            if (setenv(var_name, var_value, 0) != 0) {
                perror("setenv");
                return 1;
            }
            printf("Added %s=%s\n", var_name, getenv(var_name));
        }

    } else if (strcmp(action, "change") == 0) {
        if (var_value == NULL) {
            fprintf(stderr, "Missing value for 'change'\n");
            return 1;
        }

        if (setenv(var_name, var_value, 1) != 0) {
            perror("setenv");
            return 1;
        }
        printf("Changed %s=%s\n", var_name, getenv(var_name));

    } else if (strcmp(action, "delete") == 0) {
        if (unsetenv(var_name) != 0) {
            perror("unsetenv");
            return 1;
        }
        printf("Deleted %s (current value: %s)\n", var_name, getenv(var_name) ? getenv(var_name) : "NULL");

    } else if (strcmp(action, "print") == 0) {
        for (int i = 0; envp[i] != NULL; i++) {
            printf("%s\n", envp[i]);
        }
    } else {
        fprintf(stderr, "Unknown action: %s (use add, change, delete or print)\n", action);
        return 1;
    }

    return 0;
}
