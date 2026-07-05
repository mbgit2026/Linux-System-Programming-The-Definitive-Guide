#define _GNU_SOURCE
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <grp.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {

        uid_t uid = 65534;
        gid_t gid = 65534;

        // Drop supplementary groups
        if (setgroups(0, NULL) != 0) {
            perror("setgroups");
            _exit(1);
        }

        // Drop GID
        if (setresgid(gid, gid, gid) != 0) {
            perror("setresgid");
            _exit(1);
        }

        // Drop UID
        if (setresuid(uid, uid, uid) != 0) {
            perror("setresuid");
            _exit(1);
        }

        char *argv[] = { "/usr/bin/id", NULL };
        char *envp[] = { "PATH=/usr/bin:/bin", NULL };

        execve("/usr/bin/id", argv, envp);

        perror("execve");
        _exit(1);
    }

    waitpid(pid, NULL, 0);
    return 0;
}

