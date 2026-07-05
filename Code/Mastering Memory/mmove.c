#include <stdio.h>
#include <string.h>

int main(void)
{
    char buf[32] = "Hello world";

    printf("\nBuffer: %s", buf);

    /* Remove the space by shifting "world" left by one byte */
    memmove(buf + 5, buf + 6, strlen(buf + 6) + 1);
    /*            ^dest     ^src        ^ include '\0' */

    printf("\nBuffer: %s\n", buf);  /* Prints: Helloworld */

    return 0;
}
