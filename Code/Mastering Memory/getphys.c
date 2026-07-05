#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>

int main(void)
{
    printf("This system has %ld pages of physical memory and "
            "%ld pages of physical memory available.\n",
            get_phys_pages(), get_avphys_pages());
    exit(EXIT_SUCCESS);


}
