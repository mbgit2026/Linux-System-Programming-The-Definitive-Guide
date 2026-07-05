#include <stdio.h>
#include <unistd.h>

int main(void) {
    setvbuf(stdout, NULL, _IONBF , 0);

    printf("This is unbuffered.\n");
    printf("Line 1 (should appear immediately)\n");
    sleep(2);
    printf("Line 2 (should also appear immediately)\n");
    printf("Line 3 (again, immediate)\n");

    return 0;
}
