#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/mman.h>

#ifndef SYS_cachestat
#define SYS_cachestat 451   /* x86-64 as of Linux 6.5 */
#endif


int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct cachestat_range range = {
        .off = 0,
        .len = (unsigned long)-1   /* entire file */
    };

    struct cachestat cs = {0};

    if (syscall(SYS_cachestat, fd, &range, &cs, 0) < 0) {
        perror("cachestat");
        return 1;
    }

    printf("Cached pages:        %lu\n", cs.nr_cache);
    printf("Dirty pages:         %lu\n", cs.nr_dirty);
    printf("Writeback pages:     %lu\n", cs.nr_writeback);
    printf("Evicted pages:       %lu\n", cs.nr_evicted);
    printf("Recently accessed:   %lu\n", cs.nr_recently_evicted);

    close(fd);
    return 0;
}
