#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    struct tm tm;
   
    if (argc != 3) {
        printf("\nUsage: %s <datetime> <specifiers>", argv[0]);
        exit(EXIT_FAILURE);
    }

    strptime(argv[1], argv[2], &tm);
   
    printf("\nDAY: %d", tm.tm_mday);
    printf("\nMONTH: %d", tm.tm_mon + 1);
    printf("\nYEAR: %d", tm.tm_year + 1900);
    printf("\nHOUR: %d", tm.tm_hour);
    printf("\nMIN: %d", tm.tm_min);


    char outstr[200];
    if (strftime(outstr, sizeof(outstr), "Time: %Y-%m-%d", &tm) == 0) {
              fprintf(stderr, "strftime returned 0");
              exit(EXIT_FAILURE);
    }
        
    printf("\n%s", outstr);

    exit(EXIT_SUCCESS);
}
