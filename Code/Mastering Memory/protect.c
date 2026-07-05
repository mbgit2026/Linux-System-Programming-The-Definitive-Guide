#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

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

    if (mprotect(buffer + pagesize * 2, pagesize, PROT_READ) == -1)
        err(EXIT_FAILURE, "mprotect");

    for (char *p = buffer; ; )
        *(p++) = 'a';

    return EXIT_SUCCESS;
}
