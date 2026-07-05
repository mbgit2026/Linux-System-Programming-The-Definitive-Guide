#include <stdio.h>
#include <sys/auxv.h>

int main() {
    unsigned long uid     = getauxval(AT_UID);
    unsigned long pagesz  = getauxval(AT_PAGESZ);    
    unsigned long secure  = getauxval(AT_SECURE);
    unsigned long entry   = getauxval(AT_ENTRY);
    unsigned long base    = getauxval(AT_BASE);
    const char *execfn    = (const char *) getauxval(AT_EXECFN); 
    
    printf("AT_BASE    = 0x%lx\n", base);
    printf("AT_EXECFN  = %s\n", execfn);
    printf("AT_UID     = %lu\n", uid);
    printf("AT_PAGESZ  = %lu bytes\n", pagesz);
    printf("AT_SECURE  = %lu\n", secure);
    printf("AT_ENTRY   = 0x%lx\n", entry);

    return 0;
}
