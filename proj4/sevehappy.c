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
void * get_happys(void *);
int get_num_primes(unsigned char *, long);
int get_next_num(unsigned char *, int, int *, int, unsigned int);
void get_primes_unthreaded();
void get_primes_processes(int);
void get_primes_threaded(int);
void happy_parallel(unsigned char *, int, unsigned int, int);
int is_happy(int);
void * mark_between(void *);
void mark_to_index(unsigned char *, int, int, unsigned int);
void mark(unsigned char *, unsigned int);
bool marked(unsigned char *, unsigned int);
int sum_square_digits(int);
void usage();

struct bounds {
    unsigned char * bitmap;
    int base; // Base number to iterate over
    int * current_nums; // Numbers other threads are working on.
    int num_threads;
    int index; // Index for this thread's current_num
    unsigned int lower_bound;
    unsigned int upper_bound;
};

struct happy_info {
    unsigned char * bitmap;
    int num_threads;
    int * current_nums;
    int index;
    int cur_num;
};

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage();
        exit(0);
    }
    int c;
    int num_threads = 0;
    while ((c = getopt(argc, argv, "hpt:")) != -1)
    {
        switch(c)
        {
            case 't': // num_threads
                num_threads = atoi(optarg);
                get_primes_threaded(num_threads);
                break;
            case 'p': // num processes
                num_threads = atoi(optarg);
                get_primes_processes(num_threads);
            case 'h': // Provide usage for the command
                usage();
                exit(0);
            default:
                break;
        }
    }
    //get_primes_unthreaded(); // Serialized version of the program.
}

void get_primes_processes(int num_threads) {
    unsigned char * bitmap = (unsigned char *) malloc((UINT_MAX/BITS_PER_BYTE) + 1);
    unsigned int upper_bound = UINT_MAX-1;

    printf("Generating Prime List for '%d' threads, up to '%ld'.\n", num_threads, UINT_MAX-1);
    count_parallel(bitmap, 2, upper_bound, num_threads);

    //printf("Found %d primes", get_num_primes(bitmap, upper_bound));

    //happy_parallel(bitmap, 2, upper_bound, num_threads);

    //printf("Number of happy prime numbers: %d\n", happy_numbers);

}

void get_primes_unthreaded() {
    unsigned char * bitmap = (unsigned char *) malloc((UINT_MAX/BITS_PER_BYTE) + 1);
    unsigned int max = UINT_MAX-1;
    int i;
    int num_primes = max;
    for (i=2; i<=sqrt(max); i++) {
        if (marked(bitmap, i) == 0) {
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
    unsigned int upper_bound = UINT_MAX-1;

    printf("Generating Prime List for '%d' threads, up to '%ld'.\n", num_threads, UINT_MAX-1);
    count_parallel(bitmap, 2, upper_bound, num_threads);

    //printf("Found %d primes", get_num_primes(bitmap, upper_bound));

    //happy_parallel(bitmap, 2, upper_bound, num_threads);

    //printf("Number of happy prime numbers: %d\n", happy_numbers);

}

/* Serialized checking of number of primes. */
int get_num_primes(unsigned char * bitmap, long max) {
    int num_primes = 0;
    for (long i=2; i<max; i++) {
        if (!marked(bitmap, i)) {
            num_primes++;
        }
    }
    return num_primes;
}

/* Parent function to starting and running all threads for happy number calculations*/
void happy_parallel(unsigned char * bitmap, int start, unsigned int upper_bound, int num_threads) {
    pthread_t tids[num_threads];
    struct happy_info hi[num_threads];
    int * current_nums = (int *) malloc (num_threads * sizeof(int *));
    int current_num = 2;
    for (int i=0; i< num_threads; i++) {
        hi[i].bitmap = bitmap;
        hi[i].cur_num = current_num;
        current_nums[i] = current_num;
        hi[i].current_nums = current_nums;
        hi[i].num_threads = num_threads;
        hi[i].index = i;
        // Allows the bounds to be set correctly.
        pthread_create(&tids[i], NULL, get_happys, &hi[i]);
        current_num = get_next_num(bitmap, current_num, current_nums, num_threads, UINT_MAX-1);
    }
    for (int i=0; i<num_threads; i++) {
        pthread_join(tids[i], NULL);
    }
}

/* Parent spawns off this function for each thread to do happy number calculations */
void * get_happys(void * ptr) {
    struct happy_info * hi = (struct happy_info *) ptr;
    while (hi->cur_num > 0) {
        if (is_happy(hi->cur_num)) {
            // This makes the program take forever. Like.... damn.
            //printf("%d: Happy! :)\n", hi->cur_num);
        }
        hi->cur_num = get_next_num(hi->bitmap, hi->cur_num, hi->current_nums, hi->num_threads, UINT_MAX-1);
        hi->current_nums[hi->index] = hi->cur_num;
    }
    pthread_exit(NULL);
    return (void *) NULL;
}

/* Checks to see if a number is happy */
int is_happy(int start) {
    int unhappy_numbers[] = {4, 16, 37, 58, 89, 145, 42, 20};
    while (1) {
        start  = sum_square_digits(start);
        if (start == 1) {
            return 1;
        }
        for (int i=0; i<8; i++) {
            if (start == unhappy_numbers[i]) {
                return 0;
            }
        }
    }
    return 0;
}

/* Parent function to starting and running all threads for marking primes*/
void count_parallel(unsigned char * bitmap, int base, unsigned int upper_bound, int num_threads) {
    pthread_t tids[num_threads];
    struct bounds b[num_threads];
    int * current_nums = (int *) malloc (num_threads * sizeof(int *));
    int current_num = 2;
    for (int i=0; i< num_threads; i++) {
        b[i].bitmap = bitmap;
        b[i].base = current_num;
        current_nums[i] = current_num;
        b[i].current_nums = current_nums;
        b[i].num_threads = num_threads;
        b[i].index = i;
        // Allows the bounds to be set correctly.
        b[i].lower_bound = b[i].base*2;
        b[i].upper_bound = upper_bound;
        pthread_create(&tids[i], NULL, mark_between, &b[i]);
        current_num = get_next_num(bitmap, current_num, current_nums, num_threads, UINT_MAX-1);
    }
    for (int i=0; i<num_threads; i++) {
        //printf("Joining thread '%d'...\n", i);
        pthread_join(tids[i], NULL);
    }
    return;
}

/* Gets the next unmarked prime from a bitmap not being used by other threads. */
int get_next_num(unsigned char * bitmap, int current_num, int * current_nums, int num_threads, unsigned int upper_bound) {
    int max = current_num;
    if (max <= upper_bound) {
        for (int i=max+1; i < upper_bound; i++) {
            if (!marked(bitmap, i)) {
                int valid_num = 1;
                // Double checks for repeats.
                for (int j=0; j < num_threads; j++) {
                    if (current_nums[j] == 0) {
                        continue;
                    }
                    if (current_nums[j] >= i || current_nums[j] % i == 0 ||
                            i % current_nums[j] == 0) {
                        valid_num = 0;
                    }
                }
                if (valid_num == 1) {
                    return i;
                }
            }
        }
    }
    // Threads should exit if there are no more numbers.
    pthread_exit(NULL);
    return -1;
}

/* Function pass to thread for marking primes. */
void * mark_between(void * ptr) {
    struct bounds * b = (struct bounds *) ptr;
    while (b->base > 0) {
        b->lower_bound = b->base*2;
        //printf("Marking multiples of index '%d' with thread '%d'!\n", b->base, b->index);
        mark_to_index(b->bitmap, b->base, b->lower_bound, b->upper_bound);
        b->base = get_next_num(b->bitmap, b->base, b->current_nums, b->num_threads, (unsigned int) sqrt(UINT_MAX-1));
        b->current_nums[b->index] = b->base;
    }
    pthread_exit(NULL);
    return (void *) NULL;
}

/* Used in a previous implementation */
unsigned int adjust_lower_bound(int base, unsigned int lower_bound){
    while(lower_bound % base != 0) {
        lower_bound++;
    }
    return lower_bound;
}

/* Gets the sum of squares */
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

/* Marks primes from start, to end, at interval m on bitmap. */
void mark_to_index(unsigned char * bitmap, int m, int start, unsigned int end) {
    for (long i = start; i < UINT_MAX-1; i=i+m) {
        if (!marked(bitmap, i)) {
            mark(bitmap, i);
        }
    }
}

/* Checks to see if a bitmap is marked at index ind. */
bool marked(unsigned char bitmap[], unsigned int ind) {
    unsigned int remainder = ind % BITS_PER_BYTE;
    unsigned int bit_ind = ind / BITS_PER_BYTE;
    // & Compares bits at the byte level, so we just check to see
    // if bit bit_ind is set by setting a new byte and comparing.
    return (bitmap[bit_ind] & (1 << remainder)) == pow(2, remainder);
}

/* Marks a bitmap at index ind. */
void mark(unsigned char bitmap[], unsigned int ind) {
    unsigned int remainder = ind % BITS_PER_BYTE;
    unsigned int bit_ind = ind / BITS_PER_BYTE;
    bitmap[bit_ind] |= (1 << remainder);
}

/* Usage message. */
void usage() {
    printf("Usage: \n\t./sevehappy -t <num threads>\n\t./sevehappy -p <num processes>");
}
