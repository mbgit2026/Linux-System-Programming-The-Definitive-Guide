#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++) 
        if (n % i == 0) return false;
    
    return true;
}

int main(void) {
    int *primes = NULL;  

    while (1) {
        int n;
        printf("Number of prime numbers to generate (0 to quit): ");
        if (scanf("%d", &n) != 1 || n < 0) {
            printf("Invalid input. Exiting.\n");
            break;
        }
        if (n == 0) break; 

        //primes = realloc(primes, n * sizeof(int));
        primes = reallocarray(primes, n, sizeof(int));
        if (!primes) {
            fprintf(stderr, "Memory allocation failed\n");
            return 1;
        }

        int count = 0;
        int num = 2;
        while (count < n) {
            if (is_prime(num)) {
                primes[count] = num;
                count++;
            }
            num++;
        }

        printf("First %d prime numbers:\n", n);
        for (int i = 0; i < n; i++) 
            printf("%d ", primes[i]);
        
        printf("\n\n");
    }

    free(primes); 
    printf("%d", sizeof(long));
    return 0;
}
