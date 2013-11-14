#include <ar.h> // /usr/include/ar.h for reference
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <time.h>

#define S_IRWXUGO S_IRWXU | S_IRWXG | S_IRWXO
#define S_IRWUGO S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#define MAX_LEN 46
#define BLOCKSIZE 1

int uniqify(FILE *, int);
void usage();

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv);
        exit(0);
    }
    int c;
    int num_pipes = 0;
    FILE * fp;
    while ((c = getopt(argc, argv, "hw:p:")) != -1)
    {
        switch(c)
        {
            case 'p': // num pipes
                num_pipes = atoi(optarg);
                break;
            case 'w': // file name
                fp = fopen(optarg, "r"); // open to read
                break;
            case 'h': // Provide usage for the command
                usage(argv);
                exit(0);
            default:
                break;
        }
    }
    uniqify(fp, num_pipes);
}

int uniqify(FILE * fp, int num_pipes) {
    char word[MAX_LEN];
    int fds[num_pipes][2]; // R/W FD for each pipe
    pid_t child_pid[num_pipes];

    for(int i=0; i<num_pipes; i++)
    {
        switch(child_pid[i] = fork())
        {
            case -1:
                printf("Error occured when forking.");
                break;
            case 0:
                printf("This is child process %d.\n", i);
                printf("Pausing process %d.\n", i);
                pid_t out_fd;
                //dup2(fds[i][1], out_fd);
                for (;;_)
                {
                    pause();
                }
                char * buf;
                printf("Signal recieved for unpausing child process %d.\n", i);
                // Do stuff like exec'ing.
                break;
            default:
                printf("This is a parent process, and is recognized.\n");
                pipe(fds[i]);
                break;
        }
    }
    // This checks to see if we're in the parent process.
    FILE * open_pipes[num_pipes];
    if (child_pid[0] > 0)
    {
        for(int i=0; i<num_pipes; i++)
        {
            open_pipes[i] = fdopen(fds[i][1], "w");
        }
        int i = 0;
        while (fgets(word, MAX_LEN, fp))
        {
            printf("Now writing word: %s", word);
            fputs(word, open_pipes[i]);
            i++;
            if(i == num_pipes)
            {
                i = 0;
            }
        }

        // Send out signals to sort pipes
        for(int i=0; i<num_pipes; i++)
        {
            printf("Sending signal for process %d.\n", i);
            signal(SIGCONT, SIG_IGN);
        }


        int pid;
        // Wait for all processes to finish
        while (pid = waitpid(-1, NULL, 0))
        {
            if (pid == ECHILD)
            {
                break;
            }
        }

            }
    return 1;

}

void usage() {
    printf("Usage: Don't fuck up.");
}
