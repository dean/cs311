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

void count_parallel(unsigned char *, int, unsigned int, int);
int get_next_num(unsigned char *, int *, int, unsigned int);
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
    int num_primes = 255;
    //for (int i=2; i<=sqrt(255); i++) {
    //    if (marked(bitmap, i) == 0) {
            count_parallel(bitmap, 2, 255, num_threads);
    //    }
    //}
    /*
    int unhappy_numbers[] = {4, 16, 37, 58, 89, 145, 42, 20}; 
    for (int i=2; i<=255; i++) {
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
    */
}

void count_parallel(unsigned char * bitmap, int base, unsigned int upper_bound, int num_threads) {
    printf("Into count_parallell. b upper: %d %d\n", upper_bound, base);
    pthread_t tids[num_threads];
    struct bounds b[num_threads];
    int current_nums[num_threads];
    printf("found something?");
    int num = get_next_num(bitmap, current_nums, num_threads, upper_bound);
    while(num) {
        for (int i=0; i< num_threads && num >= 2; i++) {
            b[i].bitmap = bitmap;
            b[i].base = num;
            current_nums[i] = base;
            // Allows the bounds to be set correctly.
            b[i].lower_bound = num*2;
            b[i].upper_bound = upper_bound;
            pthread_create(&tids[i], NULL, mark_between, &b[i]);
            num = get_next_num(bitmap, current_nums, num_threads, upper_bound);
        }
    }
}

int get_next_num(unsigned char * bitmap, int current_nums[], int num_threads, unsigned int upper_bound) {
    printf("finished nothing");
    int max = 0;
    for (int i=0; i<num_threads; i++) {
        if (current_nums[i] > max) {
            max = current_nums[i];
        }
    }
    printf("finished first");
    if (max == 0) {
        return 2;
    }
    if (max <= (int)sqrt(upper_bound)) {
        for (int i=max; i < (int)sqrt(upper_bound); i++) {
            if (!marked(bitmap, i)) {
                return i;
            }
        }
    }
    printf("finished second");
    return -1;
}

void * mark_between(void * ptr) {
    printf("Spawned threads.\n");
    struct bounds * b = (struct bounds *) ptr; 
    printf("Lets see the upper bound of b: %d\n", b->upper_bound);
    mark_to_index(b->bitmap, b->base, b->lower_bound, b->upper_bound);
    return (void *) NULL;
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
    printf("top level: m=%d, start=%d, end=%d\n", m, start, end); 
    for (unsigned int i = start; i < end; i+=m) {
        if (end > 255) {
            printf("inner-inner level: m=%d, start=%d, end= %d\n", m, start, end);
            printf("Exitting...");
            exit(-1);
        }
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
