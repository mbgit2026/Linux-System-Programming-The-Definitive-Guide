#define _GNU_SOURCE
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]) {

    char *resolved;

    if (argc != 2) {
        printf("\nUsage: %s <path>", argv[0]);
        exit(EXIT_FAILURE);
    }

    resolved = realpath(argv[1], NULL);
    if (resolved == NULL) {
        perror("realpath");
        exit(EXIT_FAILURE);
    } 

/*    if ((resolved = canonicalize_file_name(argv[1])) == NULL) {
    	perror("canonicalize_file_name");   
    	exit(EXIT_FAILURE); 
    }
*/
    printf("\nAbsolute canonicalized path: \n%s", resolved); 
    exit(EXIT_SUCCESS);
}