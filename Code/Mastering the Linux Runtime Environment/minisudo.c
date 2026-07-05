#define _GNU_SOURCE
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

static int conv_func(int num_msg, const struct pam_message **msg,
                     struct pam_response **resp, void *appdata_ptr) {
    struct pam_response *reply = calloc(num_msg, sizeof(struct pam_response));
    if (!reply) return PAM_CONV_ERR;

    for (int i = 0; i < num_msg; ++i) {
        switch (msg[i]->msg_style) {
            case PAM_PROMPT_ECHO_OFF:
                reply[i].resp = strdup(getpass(msg[i]->msg));
                break;
            case PAM_PROMPT_ECHO_ON: {
                char buf[512];
                printf("%s", msg[i]->msg);
                if (fgets(buf, sizeof(buf), stdin)) {
                    buf[strcspn(buf, "\n")] = 0;
                    reply[i].resp = strdup(buf);
                }
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

int main(int argc, char *argv[]) {
    pam_handle_t *pamh = NULL;
    struct pam_conv conv = { conv_func, NULL };
    const char *service = "sudo";  // Use PAM service "sudo"
    const char *user = getenv("USER");
    int ret;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    // Start PAM
    ret = pam_start(service, user, &conv, &pamh);
    if (ret != PAM_SUCCESS) {
        fprintf(stderr, "pam_start: %s\n", pam_strerror(pamh, ret));
        return 1;
    }

    // Authenticate the user
    ret = pam_authenticate(pamh, 0);
    if (ret != PAM_SUCCESS) {
        fprintf(stderr, "Authentication failed: %s\n", pam_strerror(pamh, ret));
        pam_end(pamh, ret);
        return 1;
    }

    // Check account access
    ret = pam_acct_mgmt(pamh, 0);
    if (ret != PAM_SUCCESS) {
        fprintf(stderr, "Account check failed: %s\n", pam_strerror(pamh, ret));
        pam_end(pamh, ret);
        return 1;
    }

    // Establish credentials (e.g., tickets, SELinux role)
    ret = pam_setcred(pamh, PAM_ESTABLISH_CRED);
    if (ret != PAM_SUCCESS) {
        fprintf(stderr, "pam_setcred failed: %s\n", pam_strerror(pamh, ret));
    } else {
        printf("Credentials established successfully.\n");
    }

    // Optional: open session (not required for this demo)
    // pam_open_session(pamh, 0);

    // Fork and exec the command as root
    pid_t pid = fork();
    if (pid == 0) {
        // Child process: elevate and exec
        setuid(0); // become root
        execvp(argv[1], &argv[1]);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork");
    }

    // Close session and cleanup
    // pam_close_session(pamh, 0);

    pam_setcred(pamh, PAM_DELETE_CRED); // Clean up
    pam_end(pamh, PAM_SUCCESS);
    return 0;
}
