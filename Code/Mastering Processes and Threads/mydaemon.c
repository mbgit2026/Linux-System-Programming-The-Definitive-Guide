#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>

int main(void) {
    /* Check if running under systemd */
    if (sd_booted() <= 0) {
        printf("Not running under systemd\n");
    }

    /* Notify systemd that we are starting */
    sd_notify(0, "STATUS=Initializing...");

    sleep(2);  // simulate initialization work

    /* Tell systemd we are ready */
    sd_notify(0, "READY=1");
    sd_notify(0, "STATUS=Service is running");

    /* Check if watchdog is enabled */
    uint64_t usec = 0;
    int watchdog = sd_watchdog_enabled(0, &usec);

    if (watchdog > 0) {
        printf("Watchdog enabled: interval = %lu usec\n", usec);
    }

    /* Main loop */
    while (1) {
        printf("Working...\n");

        /* Notify watchdog (if enabled) */
        if (watchdog > 0) {
            sd_notify(0, "WATCHDOG=1");
        }

        sleep(2);
    }

    return 0;
}