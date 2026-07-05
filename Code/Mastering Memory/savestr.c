#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * savestring(const char *ptr, size_t len) {
    char *ntstr = malloc(len + 1);
    if (!ntstr) return NULL;
    ntstr[len] = '\0';
    return memcpy(ntstr, ptr, len);
}

int main(void) {

    char *hello = savestring("hello", 5);
   
}