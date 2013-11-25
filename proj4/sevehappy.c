#include <ar.h> // /usr/include/ar.h for reference
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#define S_IRWXUGO S_IRWXU | S_IRWXG | S_IRWXO
#define S_IRWUGO S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#define MAX_LEN 46
#define BLOCKSIZE 1
#define BITS_PER_BYTE 8

void get_primes_unthreaded();
void mark_to_index(unsigned char *, int, int, unsigned int, int *);
bool marked(unsigned char *, int);
void mark(unsigned char *, int);
int sum_square_digits(int);
void usage();

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage();
        exit(0);
    }
    int c;
    int num_threads = 0;
    while ((c = getopt(argc, argv, "ht:")) != -1)
    {
        switch(c)
        {
            case 't': // num_threads
                num_threads = atoi(optarg);
                break;
            case 'h': // Provide usage for the command
                usage();
                exit(0);
            default:
                break;
        }
    }
    get_primes_unthreaded();
}

void get_primes_unthreaded() {
    unsigned char * bitmap = (unsigned char *) malloc((UINT_MAX/BITS_PER_BYTE) + 1);
    unsigned int max = UINT_MAX;
    int i;
    int num_primes = 255;
    for (i=2; i<=sqrt(255); i++) {
        if (marked(bitmap, i) == 0) {
            mark_to_index(bitmap, i, i*2, 255, &num_primes);
            printf("num primes=%d\n", num_primes);
        }
    }
    int unhappy_numbers[] = {4, 16, 37, 58, 89, 145, 42, 20}; 
    for (i=2; i<=255; i++) {
        if (!marked(bitmap, i)) {
            int num = i;
            bool end = false;
            bool happy = false;
            while (!end) {
                num  = sum_square_digits(num);
                if (num == 1) {
                    happy = true;
                    break;
                }
                for (int i=0; i<8; i++) {
                    if (num == unhappy_numbers[i]) {
                        end = true;
                        break;
                    }
                }
            }
            
            printf("%d: %s\n", i, happy == true ? "Happy! :)" : "Sad :(");
        }
    }
}

int sum_square_digits(int num) {
    // Number won't go above 1000
    int sum = 0;
    for (int i=0; i < 4; i++) {
        if (num < 10) {
            sum += pow(num, 2);
            return sum;
        }
        sum += pow(num % 10, 2);
        num /= 10;
    }
    return sum;
    
    
}

void mark_to_index(unsigned char bitmap[], int m, int start, unsigned int end, int * num_primes) {
    for (unsigned int i = start; i < end; i+=m) {
        if (!marked(bitmap, i)) {
            printf("marking %d as composite\n", i);
            mark(bitmap, i);
            *num_primes -= 1;
        }
    }
}

bool marked(unsigned char bitmap[], int ind) {
    int remainder = ind % BITS_PER_BYTE;
    int bit_ind = ind / BITS_PER_BYTE;
    // & Compares bits at the byte level, so we just check to see
    // if bit bit_ind is set by setting a new byte and comparing.
    return (bitmap[bit_ind] & (1 << remainder)) == pow(2, remainder);
}

void mark(unsigned char bitmap[], int ind) {
    int remainder = ind % BITS_PER_BYTE;
    int bit_ind = ind / BITS_PER_BYTE;
    printf("marked before?=%d\n", marked(bitmap, ind));
    bitmap[bit_ind] = (int)bitmap[bit_ind] + (int)pow(2, remainder);
    printf("marked after?=%d\n\n", marked(bitmap, ind));
}

void usage() {
    printf("Usage: Don't fuck up.");
}
