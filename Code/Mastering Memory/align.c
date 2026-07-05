#include <stdio.h>
#include <unistd.h>


int main() {

    int x = 2;
    char a = 'a';
    double p = 3.14;

    printf("%d", sizeof(double));
    printf("\n%d", sizeof(long double));

    return 0;
}

