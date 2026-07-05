#include <stdio.h>
#include <unistd.h>   

int main(void) {

    
    setvbuf(stdout, NULL, _IOFBF, 0);
    printf("Hello");
    sleep(2);  
    printf("Linux");
    sleep(1);
/*
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("Hello\n");
    sleep(2);  
    printf("Linux");
    sleep(1);
*/

/*
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("Hello");
    sleep(2);  
    printf("Linux");
    sleep(1);
*/

    return 0;
}