#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/capability.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main() {
    printf("Attempting to raise priority using CAP_SYS_NICE...\n");

    // Step 1: Get current capabilities
    cap_t caps = cap_get_proc();
    if (caps == NULL) {
        perror("cap_get_proc");
        return 1;
    }

    // Step 2: Enable CAP_SYS_NICE in effective set
    cap_value_t cap_list[] = { CAP_SYS_NICE };
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, CAP_SET) == -1) {
        perror("cap_set_flag");
        cap_free(caps);
        return 1;
    }

    // Step 3: Apply new capabilities
    if (cap_set_proc(caps) == -1) {
        perror("cap_set_proc");
        cap_free(caps);
        return 1;
    }

    cap_free(caps);

    // Step 4: Try to increase priority of this process
    pid_t pid = getpid();
    int result = setpriority(PRIO_PROCESS, pid, -10);  // Increase priority (lower nice value)
    if (result == -1) {
        perror("setpriority");
    } else {
        printf("Successfully changed priority of process %d.\n", pid);
    }

    return 0;
}
