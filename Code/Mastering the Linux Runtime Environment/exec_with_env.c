#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]) {
//int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <env_var>", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *path = "./getvar";
    char *const args[] = { "./getvar", argv[1], NULL };

    /*char *const envp[] = {
        "USER=example_user",
        "HOME=/home/example_user",
        "MY_VAR=HelloWorld",
        NULL
    };*/

    if (execve(path, args, envp) == -1) {
        perror("execve");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
