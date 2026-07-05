#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void sieve(int n) {
    int limit = n * 20; // rough upper bound for nth primes
    bool *is_prime = calloc(limit + 1, sizeof(bool));

    for (int i = 2; i <= limit; i++)
        is_prime[i] = true;

    for (int i = 2; i * i <= limit; i++) {
        if (is_prime[i]) {
            for (int j = i * i; j <= limit; j += i)
                is_prime[j] = false;
        }
    }

    int count = 0;
    printf("First %d primes:\n", n);

    for (int i = 2; i <= limit && count < n; i++) {
        if (is_prime[i]) {
            printf("%d ", i);
            count++;
        }
    }

    printf("\n");
    free(is_prime);
}

int main(void) {
    int n;
    printf("Enter number of primes: ");
    scanf("%d", &n);

    if (n > 0)
        sieve(n);

    return 0;
}