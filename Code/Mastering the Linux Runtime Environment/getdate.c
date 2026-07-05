#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

int main(int argc, char *argv[]) 
{

    if (argc != 2) {
        printf("\nUsage %s '1 January 1980 15:00' ", argv[0]);
        exit(EXIT_FAILURE);
    }

    setenv("DATEMSK", "./datemsk.txt", 1);
    const char *date_str = argv[1];

    struct tm *tm_ptr = getdate(date_str);

    if (tm_ptr == NULL) {
        perror("getdate");
        fprintf(stderr, "getdate error code: %d\n", getdate_err);
        return 1;
    }


    printf("Parsed date and time:\n");
    printf("Year: %d\n", tm_ptr->tm_year + 1900);
    printf("Month: %d\n", tm_ptr->tm_mon + 1);
    printf("Day: %d\n", tm_ptr->tm_mday);
    printf("Hour: %d\n", tm_ptr->tm_hour);
    printf("Minute: %d\n", tm_ptr->tm_min);

    return 0;
}
