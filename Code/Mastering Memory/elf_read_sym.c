#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    fstat(fd, &st);

    void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    Elf64_Ehdr *eh = map;
    Elf64_Shdr *sh = (Elf64_Shdr *)((char *)map + eh->e_shoff);
    const char *shstr = (char *)map + sh[eh->e_shstrndx].sh_offset;

    for (int i = 0; i < eh->e_shnum; i++) {
        if (sh[i].sh_type != SHT_DYNSYM &&
            sh[i].sh_type != SHT_SYMTAB)
            continue;

        const char *secname = shstr + sh[i].sh_name;
        Elf64_Sym *sym = (Elf64_Sym *)((char *)map + sh[i].sh_offset);
        size_t nsyms = sh[i].sh_size / sizeof(Elf64_Sym);

        Elf64_Shdr *strsec = &sh[sh[i].sh_link];
        const char *strtab = (char *)map + strsec->sh_offset;

        printf("\nSection: %s (%zu symbols)\n", secname, nsyms);

        for (size_t j = 0; j < nsyms; j++) {
            printf(
                "%4zu: value=%016lx size=%-4lu type=%u name=%s\n",
                j,
                sym[j].st_value,
                sym[j].st_size,
                ELF64_ST_TYPE(sym[j].st_info),
                strtab + sym[j].st_name
            );
        }
    }

    munmap(map, st.st_size);
    close(fd);
    return 0;
}
