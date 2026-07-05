#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/acct.h>
#include <pwd.h>
#include <time.h>

#define ACCT_FILE "/var/account/pacct"

unsigned long comp_t_to_ulong(comp_t c)
{
    unsigned long mant = c & 0x1FFF;
    unsigned long exp  = (c >> 13) & 0x7;
    return mant << (exp * 3);
}

int main()
{
    int fd;
    struct acct_v3 record;

    fd = open(ACCT_FILE, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Reading accounting records...\n\n");

    while (read(fd, &record, sizeof(record)) == sizeof(record)) {

        struct passwd *pw = getpwuid(record.ac_uid);
        char *username = pw ? pw->pw_name : "unknown";

        time_t start_time = record.ac_btime;

        char timebuf[64];
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S",
                 localtime(&start_time));

        printf("Command     : %.16s\n", record.ac_comm);
        printf("User        : %s (UID %u)\n", username, record.ac_uid);
        printf("Flags decode: ");
        if (record.ac_flag & AFORK) printf("AFORK ");
        if (record.ac_flag & ASU)   printf("ASU ");
        if (record.ac_flag & ACORE) printf("ACORE ");
        if (record.ac_flag & AXSIG) printf("AXSIG ");
        printf("\n");
        printf("Start Time  : %s\n", timebuf);
        printf("User CPU    : %lu\n", comp_t_to_ulong(record.ac_utime));
        printf("System CPU  : %lu\n", comp_t_to_ulong(record.ac_stime));
        printf("Elapsed Time: %lu\n", comp_t_to_ulong(record.ac_etime));
        printf("--------------------------------\n");
    }

    close(fd);
    return 0;
}