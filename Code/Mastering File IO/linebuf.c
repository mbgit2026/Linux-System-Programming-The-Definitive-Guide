#include <stdio.h>
#include <unistd.h>

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 1024);

    printf("This is line buffering.\n");
    printf("Line 1 (should appear immediately)");
    sleep(2);
    printf("Line 2 (should also appear immediately)\n");
    sleep(2);
    printf("Line 3 (again, immediate)\n");

    return 0;
}
