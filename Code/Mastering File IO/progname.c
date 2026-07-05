#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>

void main() {

    printf("\n%s", program_invocation_name);
    printf("\n%s", program_invocation_short_name);


}