#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    int status;

    status = system("date");
    if (status == -1) {
        perror("system");
        return 1;
    }

    status = system("ls -l");
    if (status == -1) {
        perror("system");
        return 1;
    }

    return 0;
}
