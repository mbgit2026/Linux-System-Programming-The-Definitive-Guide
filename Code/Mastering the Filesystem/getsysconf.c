#include <stdio.h>
#include <unistd.h>
#include <errno.h>

// ANSI escape codes for colors
#define COLOR_RESET   "\033[0m"
#define COLOR_NAME    "\033[1;34m"  // Blue
#define COLOR_VALUE   "\033[1;32m"  // Green
#define COLOR_WARNING "\033[1;33m"  // Yellow
#define COLOR_ERROR   "\033[1;31m"  // Red

void print_sysconf(const char *name, int name_const) {
    errno = 0;
    long value = sysconf(name_const);
    if (value == -1) {
        if (errno != 0) {
            fprintf(stderr, COLOR_ERROR "%-25s: Error: " COLOR_RESET, name);
            perror("");
        } else {
            printf(COLOR_WARNING "%-25s: Not defined\n" COLOR_RESET, name);
        }
    } else {
        printf(COLOR_NAME "%-25s:" COLOR_VALUE " %ld\n" COLOR_RESET, name, value);
    }
}

int main(void) {
    printf("\n\033[1;36mSystem Configuration (via sysconf)\033[0m\n\n");

    print_sysconf("_SC_ARG_MAX", _SC_ARG_MAX);
    print_sysconf("_SC_OPEN_MAX", _SC_OPEN_MAX);
    print_sysconf("_SC_CLK_TCK", _SC_CLK_TCK);
    print_sysconf("_SC_PAGESIZE", _SC_PAGESIZE);
    print_sysconf("_SC_NPROCESSORS_CONF", _SC_NPROCESSORS_CONF);
    print_sysconf("_SC_NPROCESSORS_ONLN", _SC_NPROCESSORS_ONLN);
    print_sysconf("_SC_PHYS_PAGES", _SC_PHYS_PAGES);
    print_sysconf("_SC_AVPHYS_PAGES", _SC_AVPHYS_PAGES);

    // Calculate total physical memory
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGESIZE);
    if (pages != -1 && page_size != -1) {
        long long total_mem = (long long) pages * page_size;
        printf(COLOR_NAME "%-25s:" COLOR_VALUE " %lld bytes (%.2f GB)\n" COLOR_RESET,
               "Total Physical Memory",
               total_mem,
               total_mem / (1024.0 * 1024 * 1024));
    }

    printf("\n");

    return 0;
}
