#define _GNU_SOURCE
#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

static void *buffer;
static void handler(int sig, siginfo_t *si, void *unused) 
{
    printf("Got SIGSEGV at address: %p\n", si->si_addr);
    exit(EXIT_FAILURE);
}

int main(void)
{
    int pagesize;
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;

    if (sigaction(SIGSEGV, &sa, NULL) == -1)
        err(EXIT_FAILURE, "sigaction");

    pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1)
        err(EXIT_FAILURE, "sysconf");

    int ret = posix_memalign(&buffer, pagesize, 4 * pagesize);
    if (ret != 0)
        errx(EXIT_FAILURE, "posix_memalign failed: %d", ret);

    printf("Start of region:        %p\n", buffer);
    int pk = pkey_alloc(0, PKEY_DISABLE_ACCESS);
   /* if (pk == -1 && errno == EINVAL) {
        fprintf(stderr, "MPK not supported on this system\n");
        exit(1);
    }*/

    if (pkey_mprotect(buffer, pagesize * 4, PROT_READ | PROT_WRITE, pk) == -1)
        err(EXIT_FAILURE, "mprotect");
    
   sleep(200);
    printf("buffer contains: %d\n", buffer);
    return EXIT_SUCCESS;
}