#define _GNU_SOURCE

#include <stdio.h>
#include <sys/utsname.h>

int main(void) {

    struct utsname sysinfo;

    if (uname(&sysinfo) == -1) {
        perror("uname");
        return 1;
    }

    printf("System Information:\n");
    printf("  Sysname : %s\n", sysinfo.sysname);   
    printf("  Nodename: %s\n", sysinfo.nodename);  
    printf("  Release : %s\n", sysinfo.release);   
    printf("  Version : %s\n", sysinfo.version);   
    printf("  Machine : %s\n", sysinfo.machine);   
    printf("  Domainname: %s\n", sysinfo.domainname); 

    return 0;
}
