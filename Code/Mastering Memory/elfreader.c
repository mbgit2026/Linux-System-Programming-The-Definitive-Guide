#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

void read_elf_header(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    Elf64_Ehdr ehdr;
    if (fread(&ehdr, 1, sizeof(ehdr), f) != sizeof(ehdr)) {
        perror("fread");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "%s is not an ELF file\n", filename);
        fclose(f);
        exit(EXIT_FAILURE);
    }

    printf("ELF Header for %s:\n", filename);
    printf("  Magic: ");
    for (int i = 0; i < EI_NIDENT; i++) 
        printf("%02x ", ehdr.e_ident[i]);
    
    printf("\n");    printf("  Class: %s\n", ehdr.e_ident[EI_CLASS] == ELFCLASS64 ? "ELF64" : "ELF32");
    printf("  Endianness: %s\n", ehdr.e_ident[EI_DATA] == ELFDATA2LSB ? "Little endian" : "Big endian");
    printf("  Type: %u\n", ehdr.e_type);
    printf("  Machine: %u\n", ehdr.e_machine);
    printf("  Entry point: 0x%lx\n", (unsigned long)ehdr.e_entry);
    printf("  Program header offset: %ld\n", (long)ehdr.e_phoff);
    printf("  Section header offset: %ld\n", (long)ehdr.e_shoff);
    printf("  Number of section headers: %d\n", ehdr.e_shnum);
    printf("  Number of program headers: %d\n", ehdr.e_phnum);

    fclose(f);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    read_elf_header(argv[1]);
    return EXIT_SUCCESS;
}
