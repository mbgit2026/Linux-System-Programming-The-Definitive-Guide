#include <stdio.h>
#include <unistd.h>
#include <sys/auxv.h>

int main() {
    printf("\n Page size: %d", getpagesize());
    printf("\n Page size: %d", sysconf(_SC_PAGE_SIZE));
    printf("\n Page size: %d", sysconf(_SC_PAGESIZE));
    printf("AT_PAGESZ: %lu\n", getauxval(AT_PAGESZ));
    return 0;
}