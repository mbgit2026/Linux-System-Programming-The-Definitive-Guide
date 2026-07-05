#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define ALLOC_SIZE 5*1024*1024  // 5 MB per allocation

int main() {
void *ptr = NULL;
int choice;

while (1) {

    printf("\nMemory Menu:\n");
    printf("1. Allocate 5MB\n");
    printf("2. Free memory\n");
    printf("3. Release memory (malloc_trim)\n");
    printf("4. Exit\n");
    printf("Enter your choice: ");
  
    if (scanf("%d", &choice) != 1) {
        int c; 
        while ((c = getchar()) != '\n' && c != EOF);
            continue;
    }

    switch (choice) {
        case 1:
            if (ptr != NULL) {
                printf("Memory already allocated. Free it first.\n");
            } else {
                ptr = malloc(ALLOC_SIZE);
                if (ptr == NULL) {
                    printf("Allocation failed!\n");
                } else {
                    memset(ptr, 0, ALLOC_SIZE); // touch memory
                    printf("Allocated 5MB.\n");
                }
            }
            break;
        case 2:
            if (ptr == NULL) {
                printf("No memory to free.\n");
            } else {
                free(ptr);
                ptr = NULL;
                printf("Memory freed.\n");
            }
            break;
        case 3:
            if (malloc_trim(0)) {
                printf("Unused heap memory released to OS.\n");
            } else {
                printf("No memory released.\n");
            }
            break;
        case 4:
            if (ptr != NULL) free(ptr);
            printf("Exiting.\n");
            return 0;
        default:
            printf("Invalid choice.\n");
    }
}

return 0;

}
