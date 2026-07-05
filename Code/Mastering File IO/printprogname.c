#include <stdio.h>
#include <unistd.h>

int main() {
    char path[64]; 
    char buf[4096];
    pid_t pid = getpid();
    
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
    
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror("fopen");
        _exit(1);
    }

    fgets(buf, 4096, fp);
    printf("\nCommand line of the process %d: %s", pid, buf);
    
    return 0;
}
