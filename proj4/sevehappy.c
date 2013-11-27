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
#include <pthread.h>
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

unsigned int adjust_lower_bound(int, unsigned int);
void count_parallel(unsigned char *, int, unsigned int, int);
int get_num_primes(unsigned char *, int);
int get_next_num(unsigned char *, int, int, unsigned int);
void get_primes_unthreaded();
void get_primes_threaded(int);
void * mark_between(void *);
void mark_to_index(unsigned char *, int, int, unsigned int);
bool marked(unsigned char *, unsigned int);
void mark(unsigned char *, unsigned int);
int sum_square_digits(int);
void usage();

struct bounds {
    unsigned char * bitmap;
    int base;
    int current_num;
    int num_threads;
    int index;
    unsigned int lower_bound;
    unsigned int upper_bound;
};

pthread_mutex_t mutex;

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
    //get_primes_unthreaded();
    get_primes_threaded(num_threads);
    //get_primes_unthreaded();
}

void get_primes_unthreaded() {
    unsigned char * bitmap = (unsigned char *) malloc((UINT_MAX/BITS_PER_BYTE) + 1);
    unsigned int max = UINT_MAX-1;
    int i;
    int num_primes = max;
    for (i=2; i<=sqrt(max); i++) {
        if (marked(bitmap, i) == 0) {
            printf("Marking Composites for: %d\n", i);
            mark_to_index(bitmap, i, i*2, max);
        }
    }
    int unhappy_numbers[] = {4, 16, 37, 58, 89, 145, 42, 20};
    for (i=2; i<=max; i++) {
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

void get_primes_threaded(int num_threads) {
    unsigned char * bitmap = (unsigned char *) malloc((UINT_MAX/BITS_PER_BYTE) + 1);
    printf("Addr of bitmap: %p\n", bitmap);
    printf("Size of bitmap: %ld\n", sizeof(bitmap));
    unsigned int upper_bound = UINT_MAX/2-2;
    //unsigned int upper_bound = 500000;
    //for (int i=2; i<=sqrt(255); i++) {
    //    if (marked(bitmap, i) == 0) {
            count_parallel(bitmap, 2, upper_bound, num_threads);
    //    }
    //}
    printf("Done generating prime list...");
    printf("Found %d primes", get_num_primes(bitmap, upper_bound));
    /*
    int happy_numbers = 0;
    int unhappy_numbers[] = {4, 16, 37, 58, 89, 145, 42, 20};
    for (int i=2; i<=UINT_MAX/10000; i++) {
        if (!marked(bitmap, i)) {
            int num = i;
            bool end = false;
            bool happy = false;
            while (!end) {
                num  = sum_square_digits(num);
                if (num == 1) {
                    happy = true;
                    happy_numbers++;
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
    printf("Number of happy prime numbers: %d\n", happy_numbers);
    */
}

int get_num_primes(unsigned char * bitmap, int max) {
    int num_primes = 0;
    for (int i=2; i<max; i++) {
        if (!marked(bitmap, i)) {
            num_primes++;
        }
    }
    return num_primes;
}

void count_parallel(unsigned char * bitmap, int base, unsigned int upper_bound, int num_threads) {
    printf("Into count_parallell. b upper: %u %d\n", upper_bound, base);
    pthread_t tids[num_threads];
    struct bounds b[num_threads];
    int current_num = 2;
    for (int i=0; i< num_threads; i++) {
        b[i].bitmap = bitmap;
        b[i].base = current_num;
        b[i].current_num = current_num;
        b[i].num_threads = num_threads;
        b[i].index = i;
        // Allows the bounds to be set correctly.
        int iter = (upper_bound/num_threads);
        b[i].lower_bound = iter * i + (current_num);
        b[i].upper_bound = iter * (i+1);
        pthread_create(&tids[i], NULL, mark_between, &b[i]);
    }
    for (int i=0; i<num_threads; i++) {
        pthread_join(tids[i], NULL);
    }
}

int get_next_num(unsigned char * bitmap, int current_num, int num_threads, unsigned int upper_bound) {
    int max = current_num;
    if (max <= (int)sqrt(upper_bound)) {
        for (int i=max+1; i < (int)sqrt(upper_bound); i++) {
            if (!marked(bitmap, i)) {
                return i;
            }
        }
    }
    return -1;
}

void * mark_between(void * ptr) {
    struct bounds * b = (struct bounds *) ptr;
    while (b->current_num > 0) {
        printf("Current prime: %d index %d\n", b->current_num, b->index);
        b->base = b->current_num;
        int iter = (b->upper_bound/b->num_threads);
        b->lower_bound = iter * b->index + (b->base);
        unsigned int cur_lower_bound = adjust_lower_bound(b->base, b->lower_bound);
        mark_to_index(b->bitmap, b->base, cur_lower_bound, b->upper_bound);
        b->current_num = get_next_num(b->bitmap, b->current_num, b->num_threads, UINT_MAX-2);
    }
    pthread_exit(NULL);
    return (void *) NULL;
}

unsigned int adjust_lower_bound(int base, unsigned int lower_bound){
    while(lower_bound % base != 0) {
        lower_bound++;
    }
    return lower_bound;
}

int sum_square_digits(int num) {
    // Number won't go above 1000
    int sum = 0;
    while (num >=10) {
        sum += pow(num % 10, 2);
        num /= 10;
    }
    sum += pow(num, 2);
    return sum;
}

void mark_to_index(unsigned char * bitmap, int m, int start, unsigned int end) {
    for (unsigned int i = start; i < end; i+=m) {
        if (!marked(bitmap, i)) {
            mark(bitmap, i);
        }
    }
}

bool marked(unsigned char bitmap[], unsigned int ind) {
    int remainder = ind % BITS_PER_BYTE;
    int bit_ind = ind / BITS_PER_BYTE;
    // & Compares bits at the byte level, so we just check to see
    // if bit bit_ind is set by setting a new byte and comparing.
    return (bitmap[bit_ind] & (1 << remainder)) == pow(2, remainder);
}

void mark(unsigned char bitmap[], unsigned int ind) {
    unsigned int remainder = ind % BITS_PER_BYTE;
    unsigned int bit_ind = ind / BITS_PER_BYTE;
    bitmap[bit_ind] |= (1 << remainder);
}

void usage() {
    printf("Usage: Don't fuck up.");
}
