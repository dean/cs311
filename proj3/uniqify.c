#include <ar.h> // /usr/include/ar.h for reference
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#define S_IRWXUGO S_IRWXU | S_IRWXG | S_IRWXO
#define S_IRWUGO S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#define BLOCKSIZE 1

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv);
        exit(0);
    }


    int c = 0;
    int num_
/*
    while ((c = getopt(argc, argv, "vqtdwA:")) != -1)
    {
        switch(c)
        {
                break;
            case 'h': // Provide usage for the command
                usage(argv);
                exit(0);
            default:
                break;
        }
    }
*/
}
