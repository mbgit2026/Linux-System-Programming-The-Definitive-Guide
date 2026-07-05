#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    FILE *fp;
    time_t now;
    struct tm *tm_info;

    fp = fopen("current_time.txt", "w");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    time(&now);
    tm_info = localtime(&now);
    if (!tm_info) {
        perror("localtime");
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%s", asctime(tm_info));

    fclose(fp);
    return 0;
}
