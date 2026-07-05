#include <stdio.h>

int sum(int x, int y) {
    int z = x + y;
    return z;
}

int main(int argc, char *argv[]) {
    int a = 3;
    int b = 2;

    int ret = sum(a,b);

    printf("SUM: %d", ret);
    return(0);
}