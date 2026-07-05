#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <utmp.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <grp.h>

void do_login_record(const char *username) {
    struct utmp ut;
    char *tty_full = ttyname(STDIN_FILENO);
    if (!tty_full) return;

    char *tty = tty_full + strlen("/dev/");
    memset(&ut, 0, sizeof(ut));
    ut.ut_type = USER_PROCESS;
    ut.ut_pid = getpid();
    strncpy(ut.ut_line, tty, sizeof(ut.ut_line));
    strncpy(ut.ut_user, username, sizeof(ut.ut_user));
    ut.ut_tv.tv_sec = time(NULL);
    ut.ut_tv.tv_usec = 0;

    login(&ut);
}

void do_logout_record(void) {
    char *tty_full = ttyname(STDIN_FILENO);
    if (tty_full) logout(tty_full + strlen("/dev/"));
    endutent();
}

int main(void) {
    char username[64];
    char *password;
    struct passwd *pw;
    struct spwd *sp;
    char *crypted, *input_crypted;

    printf("Login: ");
    fflush(stdout);
    if (!fgets(username, sizeof(username), stdin)) return 1;
    username[strcspn(username, "\n")] = 0;

    password = getpass("Password: ");

    pw = getpwnam(username);
    if (!pw) {
        fprintf(stderr, "Login incorrect.\n");
        return 1;
    }

    sp = getspnam(username);
    if (!sp) {
        fprintf(stderr, "Cannot read shadow entry.\n");
        return 1;
    }

    crypted = sp->sp_pwdp;
    input_crypted = crypt(password, crypted);

    if (strcmp(input_crypted, crypted) != 0) {
        fprintf(stderr, "Login incorrect.\n");
        return 1;
    }

    printf("Welcome %s!\n", username);

    // Record login
    do_login_record(username);

    // Set user/group ID
    if (initgroups(username, pw->pw_gid) < 0 || setgid(pw->pw_gid) < 0 || setuid(pw->pw_uid) < 0) {
        perror("Failed to drop privileges");
        return 1;
    }

    // Start shell
    char *shell = pw->pw_shell && pw->pw_shell[0] ? pw->pw_shell : "/bin/sh";
    execlp(shell, shell, (char *)NULL);

    // If we got here, exec failed
    perror("Failed to exec shell");
    do_logout_record();
    return 1;
}
