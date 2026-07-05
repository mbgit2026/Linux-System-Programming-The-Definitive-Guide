#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    double loadavg[3];

    int n = getloadavg(loadavg, 3);
    if (n == -1) {
        perror("getloadavg");
        exit(EXIT_FAILURE);
    }

    printf("Load average (1 min):  %.2f\n", loadavg[0]);
    printf("Load average (5 min):  %.2f\n", loadavg[1]);
    printf("Load average (15 min): %.2f\n", loadavg[2]);

    return 0;
}
