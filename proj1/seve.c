#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void seve(int *, int, int *);

int main(void) {
    printf("Please enter the high end of the limit you wish to find primes to: ");
    int size;
    scanf("%d", &size);

    int *primes = malloc(size+1 * sizeof(int *)); // We want to approach the limit of size, not 1 before.
    int num_primes = size-1; // 1 is special.
    seve(primes, size, &num_primes);
    printf("There are %d primes less than or equal to %d.\n", num_primes, size);
    printf("They are:\n");

    int i;
    for (i=2; i<=size; i++) {
        if (primes[i] != 1) {
            printf("%d\n", i);
        }
    }
}

// Seve of Aritostenes, that calculates the number of primes too.
void seve(int * primes, int size, int * num_primes) {

    int i, j;
    primes[1] = 1; // 1 is special :)
    for (i=2; i<=sqrt(size); i++) {
        if (primes[i] != 1) {
            primes[i] = i;
            j = i+i;
            while (j <= size) {
                if (primes[j] != 1) {
                    primes[j] = 1;
                    // Rather than num_primes after, we num_primes during!
                    *num_primes -= 1;
                }
                j += i;
            }
        }
    }
}
