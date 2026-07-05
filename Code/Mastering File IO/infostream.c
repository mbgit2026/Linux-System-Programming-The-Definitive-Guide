#define _GNU_SOURCE
#include <stdio.h>
#include <stdio_ext.h>
#include <unistd.h>

void print_stream_info(FILE *fp, const char *name) {
    printf("Stream: %s\n", name);
    printf("  Buffer size     : %zu\n", __fbufsize(fp));
    printf("  Bytes pending   : %zu\n", __fpending(fp));
    printf("  Line buffered   : %s\n", __flbf(fp) ? "yes" : "no");
    printf("  Readable        : %s\n", __freadable(fp) ? "yes" : "no");
    printf("  Writable        : %s\n", __fwritable(fp) ? "yes" : "no");
    printf("  In reading mode : %s\n", __freading(fp) ? "yes" : "no");
    printf("  In writing mode : %s\n", __fwriting(fp) ? "yes" : "no");
    printf("\n");
}

int main(void) {
    char buf[128];

    printf("=== Initial stream state ===\n");
    print_stream_info(stdin,  "stdin");
    print_stream_info(stdout, "stdout");

    printf("Type something (or pipe input) and press Enter:\n");
    fflush(stdout);

    if (fgets(buf, sizeof(buf), stdin)) {
        printf("You typed / piped: %s", buf);
    }

    printf("\n=== After reading ===\n");
    print_stream_info(stdin,  "stdin");
    print_stream_info(stdout, "stdout");

    return 0;
}

