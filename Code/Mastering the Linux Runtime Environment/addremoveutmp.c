#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>

int main(void)
{
    struct utmp entry;

    system("echo before adding entry:;who");
    entry.ut_type = USER_PROCESS;
    entry.ut_pid = getpid();
    strcpy(entry.ut_line, ttyname(STDIN_FILENO) + strlen("/dev/"));
    /* only correct for ptys named /dev/tty[pqr][0-9a-z] */
    strcpy(entry.ut_id, ttyname(STDIN_FILENO) + strlen("/dev/tty"));
    entry.ut_tv.tv_sec = time(NULL);
    entry.ut_tv.tv_usec = 0 ;
    strcpy(entry.ut_user, getpwuid(getuid())->pw_name);
    memset(entry.ut_host, 0, UT_HOSTSIZE);
    entry.ut_addr_v6[0] = 0;
    setutent();
    if (pututline(&entry) == NULL) {
        perror("pututline");
        exit(EXIT_FAILURE);
    }

    system("echo after adding entry:;who");
    entry.ut_type = DEAD_PROCESS;
    memset(entry.ut_line, 0, UT_LINESIZE);
    entry.ut_tv.tv_sec = time(NULL);
    entry.ut_tv.tv_usec = 0 ;    memset(entry.ut_user, 0, UT_NAMESIZE);
    setutent();
    pututline(&entry);

    system("echo after removing entry:;who");
    endutent();
    exit(EXIT_SUCCESS);
}
