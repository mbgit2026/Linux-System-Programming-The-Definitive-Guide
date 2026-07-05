#define _GNU_SOURCE
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int my_conv(int num_msg, const struct pam_message **msg,
                   struct pam_response **resp, void *appdata_ptr) {
    struct pam_response *reply = calloc(num_msg, sizeof(struct pam_response));
    if (!reply) return PAM_CONV_ERR;

    for (int i = 0; i < num_msg; ++i) {
        const char *prompt = msg[i]->msg ? msg[i]->msg : "";

        switch (msg[i]->msg_style) {
            case PAM_PROMPT_ECHO_OFF: {
                char *input = getpass(prompt);
                reply[i].resp = input ? strdup(input) : NULL;
                reply[i].resp_retcode = 0;
                break;
            }
            case PAM_PROMPT_ECHO_ON: {
                char buffer[512];
                printf("%s", prompt);
                if (!fgets(buffer, sizeof(buffer), stdin)) {
                    free(reply);
                    return PAM_CONV_ERR;
                }
                buffer[strcspn(buffer, "\n")] = '\0';
                reply[i].resp = strdup(buffer);
                reply[i].resp_retcode = 0;
                break;
            }
            case PAM_TEXT_INFO:
                printf("%s\n", prompt);
                break;
            case PAM_ERROR_MSG:
                fprintf(stderr, "%s\n", prompt);
                break;
            default:
                free(reply);
                return PAM_CONV_ERR;
        }
    }

    *resp = reply;
    return PAM_SUCCESS;
}

int main() {
    const char *service = "passwd"; 
    char username[64];
    pam_handle_t *pamh = NULL;
    struct pam_conv conv = { my_conv, NULL };
    int ret;

    printf("Username: ");
    fflush(stdout);
    if (!fgets(username, sizeof(username), stdin)) return 1;
    username[strcspn(username, "\n")] = 0;

    ret = pam_start(service, username, &conv, &pamh);
    if (ret != PAM_SUCCESS) {
        fprintf(stderr, "pam_start failed: %s\n", pam_strerror(pamh, ret));
        return 1;
    }

    ret = pam_authenticate(pamh, 0);
    if (ret != PAM_SUCCESS) {
        fprintf(stderr, "Authentication failed: %s\n", pam_strerror(pamh, ret));
        pam_end(pamh, ret);
        return 1;
    }

    ret = pam_acct_mgmt(pamh, 0);
    if (ret != PAM_SUCCESS) {
        fprintf(stderr, "Account check failed: %s\n", pam_strerror(pamh, ret));
        pam_end(pamh, ret);
        return 1;
    }

    ret = pam_chauthtok(pamh, PAM_SILENT);
    if (ret != PAM_SUCCESS) {
        fprintf(stderr, "Password change failed: %s\n", pam_strerror(pamh, ret));
    } else {
        printf("Password changed successfully.\n");
    }

    pam_end(pamh, PAM_SUCCESS);
    return 0;
}
