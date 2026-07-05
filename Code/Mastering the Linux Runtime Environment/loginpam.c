#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <utmp.h>
#include <time.h>
#include <sys/types.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <security/pam_ext.h>


char *captured_password = NULL;
static int pam_conversation(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
    
    struct pam_response *reply = calloc(num_msg, sizeof(struct pam_response));
    
    if (!reply) 
        return PAM_CONV_ERR;

    for (int i = 0; i < num_msg; ++i) {
        switch (msg[i]->msg_style) {
            case PAM_PROMPT_ECHO_OFF: {
                char *input = getpass(msg[i]->msg);
                reply[i].resp = input ? strdup(input) : NULL;
                reply[i].resp_retcode = 0;

                if (captured_password) {
                    free(captured_password);
                }
                captured_password = strdup(input);
                break;
            }
            case PAM_PROMPT_ECHO_ON: {
                char buffer[512];
                printf("%s", msg[i]->msg);
                if (!fgets(buffer, sizeof(buffer), stdin)) return PAM_CONV_ERR;
                buffer[strcspn(buffer, "\n")] = 0;
                reply[i].resp = strdup(buffer);
                reply[i].resp_retcode = 0;
                break;
            }
            case PAM_TEXT_INFO:
                printf("%s\n", msg[i]->msg);
                break;
            case PAM_ERROR_MSG:
                fprintf(stderr, "%s\n", msg[i]->msg);
                break;
            default:
                free(reply);
                return PAM_CONV_ERR;
        }
    }

    *resp = reply;
    return PAM_SUCCESS;
}

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
    struct passwd *pw;
    pam_handle_t *pamh = NULL;
    int pam_result, ret;
    struct pam_conv conv = { pam_conversation, NULL };


    printf("Login: ");
    fflush(stdout);
    if (!fgets(username, sizeof(username), stdin)) return 1;
    username[strcspn(username, "\n")] = 0;

    pam_result = pam_start("loginpam", username, &conv, &pamh);
    if (pam_result != PAM_SUCCESS) {
        fprintf(stderr, "pam_start failed: %s\n", pam_strerror(pamh, pam_result));
        return 1;
    }

    pam_result = pam_authenticate(pamh, 0);
    if (pam_result != PAM_SUCCESS) {
        fprintf(stderr, "Authentication failed: %s\n", pam_strerror(pamh, pam_result));
        pam_end(pamh, pam_result);
        return 1;
    }

    pam_result = pam_acct_mgmt(pamh, 0);
    if (pam_result != PAM_SUCCESS) {
        fprintf(stderr, "Account check failed: %s\n", pam_strerror(pamh, pam_result));
        pam_end(pamh, pam_result);
        return 1;
    }


    pam_result = pam_open_session(pamh, 0);
    if (pam_result != PAM_SUCCESS) {
        fprintf(stderr, "Failed to open PAM session: %s\n", pam_strerror(pamh, pam_result));
        pam_end(pamh, pam_result);
        return 1;
    }


    pw = getpwnam(username);
    if (!pw) {
        fprintf(stderr, "getpwnam failed.\n");
        pam_close_session(pamh, 0);
        pam_end(pamh, PAM_SUCCESS);
        return 1;
    }

    do_login_record(username);
    setgid(pw->pw_gid);

    if (initgroups(username, pw->pw_gid) < 0 || setgid(pw->pw_gid) < 0 || setuid(pw->pw_uid) < 0) {
        perror("Failed to drop privileges");
        pam_close_session(pamh, 0);
        pam_end(pamh, PAM_SUCCESS);
        return 1;
    }

    setuid(pw->pw_uid);
    setenv("HOME", pw->pw_dir, 1);
    setenv("USER", pw->pw_name, 1);
    setenv("LOGNAME", pw->pw_name, 1);
    setenv("SHELL", pw->pw_shell, 1); 

    char **envlist = pam_getenvlist(pamh);
    if (envlist) {
        for (char **env = envlist; *env; ++env) {
            printf("  %s\n", *env);
            putenv(*env); 
        }
        free(envlist);
    }

    const void *user, *service;

    ret = pam_get_item(pamh, PAM_USER, &user);
    if (ret != PAM_SUCCESS || !user) {
        fprintf(stderr, "pam_get_item failed: %s\n", pam_strerror(pamh, ret));
        pam_end(pamh, ret);
    } else {
        printf("\nPAM USER: %s", user);
    }

    ret = pam_get_item(pamh, PAM_SERVICE, &service);
    if (ret != PAM_SUCCESS || !service) {
        fprintf(stderr, "pam_get_item failed: %s\n", pam_strerror(pamh, ret));
        pam_end(pamh, ret);
    } else {
        printf("\nPAM SERVICE: %s", service);
    }

    if (captured_password) {
        printf("\nPassword captured: %s\n", captured_password);  
        pam_set_item(pamh, PAM_AUTHTOK, captured_password);
        free(captured_password);
    } else {
        printf("\nPassword not captured.\n");
    }

    if (chdir(pw->pw_dir) < 0) {
        fprintf(stderr, "Warning: Failed to change to home directory %s\n", pw->pw_dir);
    }
    printf("\nWelcome %s!\n", username);

    char *shell = pw->pw_shell && pw->pw_shell[0] ? pw->pw_shell : "/bin/sh";
    execlp(shell, shell, (char *)NULL);
    perror("Failed to exec shell");
    do_logout_record();
    pam_close_session(pamh, 0);
    pam_end(pamh, PAM_SUCCESS);

    return 1;
}
