#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <stdio.h>


int main () {

    int fd = open("/proc/self/auxv", O_RDONLY);
    Elf64_auxv_t aux;
    while (read(fd, &aux, sizeof(aux)) == sizeof(aux)) {
        printf("Type: %lu, Value: %lx\n", aux.a_type, aux.a_un.a_val);
    }

    return 0;
}
