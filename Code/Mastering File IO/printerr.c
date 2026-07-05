#define _GNU_SOURCE
#include <stdio.h>
#include <error.h>
#include <errno.h>

void printmsg(void) {
    fprintf(stderr, "\nprinterr-msg: ");
}

int main(int argc, char *argv[]) {
    error_print_progname = printmsg;

    if (argc != 2) 
        error(1, 0, "Usage: %s <file>", argv[0]);
    

    FILE *fp = fopen(argv[1], "r");
    if (!fp)
        error_at_line(1, errno, __FILE__, __LINE__, "Failed while trying to open %s", argv[1]);

    char buf[32];
    while (fgets(buf, sizeof(buf), fp) != NULL) 
        printf("%s", buf);

    printf("\nErrors: %d\n", error_message_count);
    printf("End of program\n");
    fclose(fp);
    return 0;
}

