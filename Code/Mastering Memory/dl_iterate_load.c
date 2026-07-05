#define _GNU_SOURCE
#include <link.h>
#include <stdio.h>
#include <stdint.h>

static int callback(struct dl_phdr_info *info, size_t size, void *data)
{
    uintptr_t start = (uintptr_t)-1;
    uintptr_t end   = 0;

    for (int i = 0; i < info->dlpi_phnum; i++) {
        const ElfW(Phdr) *ph = &info->dlpi_phdr[i];

        if (ph->p_type != PT_LOAD)
            continue;

        uintptr_t seg_start = info->dlpi_addr + ph->p_vaddr;
        uintptr_t seg_end   = seg_start + ph->p_memsz;

        if (seg_start < start) start = seg_start;
        if (seg_end   > end)   end   = seg_end;
    }

    if (start <= end) {
        printf("%s\n", info->dlpi_name[0] ? info->dlpi_name : "[exe]");
        printf("  mapped range: [%p - %p]\n", (void *)start, (void *)end);
    }

    return 0;
}

int main(void)
{
    dl_iterate_phdr(callback, NULL);
    //getchar();
    return 0;
}
