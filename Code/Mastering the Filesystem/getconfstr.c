#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_confstr(const char *name, int conf_name) {
    size_t size = confstr(conf_name, NULL, 0);

    if (size == 0) {
        printf("%s: not supported\n", name);
        return;
    }

    char *buf = malloc(size);
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    confstr(conf_name, buf, size);

    printf("%s: %s\n", name, buf);

    free(buf);
}

int main(void) {
    printf("\nConfiguration-dependent strings (confstr)\n\n");

    print_confstr("_CS_PATH", _CS_PATH);
    print_confstr("_CS_GNU_LIBC_VERSION", _CS_GNU_LIBC_VERSION);
    print_confstr("_CS_GNU_LIBPTHREAD_VERSION", _CS_GNU_LIBPTHREAD_VERSION);

    return 0;
}