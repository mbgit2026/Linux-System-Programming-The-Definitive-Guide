#include <stdio.h>
#include <syslog.h>

int main(void) {
    /* Open connection to syslog */
    openlog("myapp", LOG_PID | LOG_CONS, LOG_USER);

    /* Set log mask: allow messages up to LOG_WARNING */
    setlogmask(LOG_UPTO(LOG_WARNING));

    /* These messages will be logged */
    syslog(LOG_ERR, "This is an error message");
    syslog(LOG_WARNING, "This is a warning message");

    /* These messages will be ignored due to the mask */
    syslog(LOG_INFO, "This is an informational message");
    syslog(LOG_DEBUG, "This is a debug message");

    /* Example using %m (prints strerror(errno)) */
    FILE *f = fopen("nonexistent.txt", "r");
    if (!f) {
        syslog(LOG_ERR, "Failed to open file: %m");
    }

    /* Close connection */
    closelog();

    return 0;
}