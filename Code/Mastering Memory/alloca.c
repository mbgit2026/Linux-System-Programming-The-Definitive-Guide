#include <stdio.h>
#include <alloca.h>

void demo_alloca(size_t n) {
    printf("\nAllocating %zu bytes on the stack...\n", n);

    char *buffer = alloca(n);

    for (size_t i = 0; i < n; ++i)
        buffer[i] = 'A' + (i % 26);

    printf("Buffer content: %s\n", buffer);

}

int main(void) {
    demo_alloca(64);    
    demo_alloca(1024);  
    demo_alloca(10 * 1024 * 1024); // VERY dangerous, stack overflow!

    return 0;
}
