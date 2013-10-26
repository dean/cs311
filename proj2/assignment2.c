#include <ar.h> // /usr/include/ar.h for reference
#include <ctype.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>

#define S_IRWXUGO S_IRWXU | S_IRWXG | S_IRWXO

void usage(char *[]);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv);
        exit(0);
    }


    int c = 0;
    bool verbose = false;
    char *
    char key;

    while ((c = getopt(argc, argv, "vqtdwA:")) != -1)
    {
        switch(c)
        {
            case 'v': // If and only if other options are chosen, print verbose output
                verbose = true;
                break;
            case 'q': // Quickly append files to archive
                // append files
                key = 'q';
                break;
            case 't': // Print a concise table of contents

                break;
            case 'x': // Extract named files

                break;
            case 'd': // Delete files from named archive

                break;
            case 'w': // Extra Credit: For a given timout add all modified files to a given archive
                // Use timestamp to check this
                break;
            case 'A': // Quickly append all regular files in a given directory.

                break;
            case 'h': // Provide usage for the command
                usage(argv);
                exit(0);
        }
    }

    int ar_fd = open(argv[1], O_WRONLY | O_CREAT, S_IRWXUGO);



}

// Provide usage for the function.
void usage(char *argv[])
{
    printf("Usage: %s key afile name ...", argv[0]);
}

// -q Quickly append all files to archive
void append(char *ar_fname, char **f_names, int num_files, bool verbose)
{
    break;
}

// -t Print a concise table of contents
void contents(char *ar_fname, bool verbose)
{
    break;
}

//  -x Extract named files
void extract(char *ar_fname, char **f_names, bool verbose)
{
    break;
}

// -d Delete files from named archive
void delete(char *ar_fname, char **f_names, bool verbose)
{
    break;
}

// -w Extra Credit: For a given timout add all modified files to a given archive
// Use timestamp to check this
void append_modified(char *ar_fname, char **f_names, int timeout, bool verbose)
{
    break;
}

// -A Quickly append all files in the current directory
void append_all(char *ar_fname, bool verbose)
{
    break;
}
