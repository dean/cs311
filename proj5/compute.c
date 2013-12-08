#define  _XOPEN_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define S_IRWXUGO S_IRWXU | S_IRWXG | S_IRWXO
#define S_IRWUGO S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#define MAXLINE 4096

void append_str(char *, char *, int, int);
int num_digits(int);
void get_primes(char *, char *);
int is_perfect(int);

int main(int argc, char *argv[])
{
    // Used http://www.thegeekstuff.com/2011/12/c-socket-programming/ as a reference
    // to writing the client portion of the C portion.

    char * data = (char *) malloc(MAXLINE);
    char * primes_f = (char *) malloc(MAXLINE);
    char * buf = (char *) malloc(MAXLINE);
    char * xml_head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    // We'll have performance as an element, and perfect_numbers as an element
    append_str(primes_f, xml_head, strlen(xml_head), 0);


    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5666);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr.s_addr);

    printf("connecting...");
    printf("%d", connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)));
    char * xml_content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<req>numbers</req>\n";
    write(sockfd, xml_content, MAXLINE);
    while(read(sockfd, data, MAXLINE) > 0){
        
        //bzero(recvline, MAXLINE);

        //write(sockfd, sendline, strlen(sendline));
        /*
        if(read(sockfd, recvline, MAXLINE) == 0){
            perror("Something broke");
            exit(-1);
        }
        */
        //fputs(sendline, stdout);
        printf("Got input, sending to get_primes.\n");
        printf("data = %s\n", data);
 //       data = "<start>9</start>\n<end>12</end>\n";
        get_primes(primes_f, data);
    }
    printf("\n\nprimes_f=%s", primes_f);
    write(sockfd, primes_f, MAXLINE);
    
}

void get_primes(char * primes_f, char * data) {
    char * line = (char *) malloc(500);
    char * buf;
    char * num = (char *) malloc(15);
    char * n;
    int base = 0;
    int ind = 0;
    int num_ind = 0;
    int start = -1;
    int end = -1;

    printf("data[0]=%c", data[0]);

    while (data[base] != '\0') {
        if (data[base+ind] != '\n') {
            ind++;
        }
        else {
            printf("Entered this loop!\n");
            int start_tag=0;
            for(int i=0; i<ind; i++) {
                if (data[base+i] == '>') {
                    start_tag = 1;
                }
                else if (start_tag) {
                    if (data[base+i] != '<') {
                        num[num_ind] = data[base+i];
                        num_ind++;
                    }
                    else {
                        break;
                    }
                }
            }
            if (start == -1) {
                start = atoi(num);
            }
            else {
                end = atoi(num);
            }
            base+=ind+1;
            ind=0;
            ind = 0;
            num_ind = 0;
            bzero(num, 15);

        }
    }
    if (start > end) {
        int tmp = end;
        end = start;
        start = tmp;
    }
    printf("start=%d\n", start);
    printf("end=%d\n", end);

    char * tag = "";
    ind = 39;
    printf("ind=%d\n", ind);
    for (int i = start; i<=end; i++) {
            tag = "<perfect>";
            append_str(primes_f, tag, 9, ind);
            ind += 9;
            printf("ind=%d\n", ind);
            bzero(num, 15);
            sprintf(num, "%d", i); 
            printf("i = %d, num_digits = %d\n", i, num_digits(i));
            printf("ind=%d\n", ind);
            append_str(primes_f, num, num_digits(i), ind);
            ind += strlen(num);
            printf("ind=%d\n", ind);
            tag = "</perfect>\n";
            append_str(primes_f, tag, 11, ind);
            ind += 11;
            printf("ind=%d\n", ind);
    }
    int i = 0 ;
    //while (i <= 133) {
    printf("primes_f in get_primes=%s", primes_f);
    while (primes_f[i] != '\0') {
        if (primes_f[i] == '\0') {
            printf("^");
        }
        else {
        printf("%c", primes_f[i]);
        }
        i++;
    }
}

int num_digits(int num) {
    // Number won't go above 1000
    int i = 0;
    do {
        num /= 10;
        i++;
    } while(num != 0);
    return i;
}

void append_str(char * str, char * to_append, int size, int start) {
    for (int i=0; i < size; i++) {
        str[start+i] = to_append[i];
    }
}

int is_perfect(int num) {
    int sum = 0;
    for (int i=1; i <= num/2; i++) {
        if ( num % i == 0) {
            sum += i;
        }
        // Small optimization
        if (sum > num) {
            return 0;
        }
    }
    // Is a perfect number
    if (sum == num) {
        return 1;
    }
    return 0;
}
