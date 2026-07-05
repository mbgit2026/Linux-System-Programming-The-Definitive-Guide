#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
//#include <math.h>

int main(void)
{
    Dl_info info;

    //void *addr = (void *)printf;
    //void *addr = (void *)cos;

    void *addr = (void *)((char *)printf + 230);

    /* void *h = dlopen("libc.so.6", RTLD_NOW);
    void *addr = dlsym(h, "printf"); */

    if (dladdr(addr, &info) == 0) {
        fprintf(stderr, "dladdr() failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Address:            %p\n", addr);
    printf("Shared object file: %s\n", info.dli_fname);
    printf("Symbol name:        %s\n", info.dli_sname);
    printf("Symbol address:    %p\n", info.dli_saddr);

    return 0;
}
