#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
            printf("Invalid input\n");
            break;
        }
        if (n == 0) break;

        int capacity = 0;
        // grow only if needed
        if (n > capacity) {
            int new_capacity = (capacity == 0) ? 8 : capacity;
            while (new_capacity < n) new_capacity *= 2;

            int *tmp = realloc(primes, new_capacity * sizeof(int));
            if (!tmp) {
                fprintf(stderr, "Memory allocation failed\n");
                free(primes);
                return 1;
            }

            primes = tmp;
            capacity = new_capacity;
        }

        int count = 0;
        int num = 2;

        while (count < n) {
            if (is_prime(num)) {
                primes[count++] = num;
            }
            num++;
        }

        printf("First %d prime numbers:\n", n);
        for (int i = 0; i < n; i++)
            printf("%d ", primes[i]);

        printf("\n\n");
    }

    free(primes);
    return 0;
}