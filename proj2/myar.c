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

void usage(char *[]);
void append(char **, int, int);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv);
        exit(0);
    }


    int c = 0;
    int verbose = 0;
    int num_files = 0; // num files for an option
    char **f_names ; // [0] is the archive name, the following are files to put in the archive

    while ((c = getopt(argc, argv, "vqtdwA:")) != -1)
    {
        switch(c)
        {
            case 'v': // If and only if other options are chosen, print verbose output
                verbose = 1;
                break;
            case 'q': // Quickly append files to archive
                // append files

                // filenames must be 16 char or less.

                // 16 character placeholders.
                f_names = (char**) malloc(16 * (argc-optind));
                num_files = argc - optind;
                for (int i=0; i<num_files; i++)
                {
                    if (strlen(argv[optind]) > 16) // A char 1 byte, so 16 is what we're looking for.
                    {
                        printf("The provided filename \"%s\" is more than 16 "
                               "characters!", optarg);
                        usage(argv);
                        exit(0);
                    }
                    f_names[i] = argv[optind];
                    optind++;
                }
                append(f_names, num_files, verbose);
                break;
            case 't': // Print a concise table of contents

                break;
            case 'x': // Extract named files

                // 16 character placeholders.
                f_names = (char**) malloc(16 * (argc-optind));
                num_files = argc - optind;
                for (int i=0; i<num_files; i++)
                {
                    if (strlen(argv[optind]) > 16) // A char 1 byte, so 16 is what we're looking for.
                    {
                        printf("The provided filename \"%s\" is more than 16 "
                               "characters!", optarg);
                        usage(argv);
                        exit(0);
                    }
                    f_names[i] = argv[optind];
                    optind++;
                }

                break;
            case 'd': // Delete files from named archive

                // 16 character placeholders.
                f_names = (char**) malloc(16 * (argc-optind));
                num_files = argc - optind;
                for (int i=0; i<num_files; i++)
                {
                    if (strlen(argv[optind]) > 16) // A char 1 byte, so 16 is what we're looking for.
                    {
                        printf("The provided filename \"%s\" is more than 16 "
                               "characters!", optarg);
                        usage(argv);
                        exit(0);
                    }
                    f_names[i] = argv[optind];
                    optind++;
                }

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


}

// Provide usage for the function.
void usage(char *argv[])
{
    printf("Usage: %s key afile name ...", argv[0]);
}

// -q Quickly append all files to archive
void append(char **f_names, int num_files, int verbose)
{
    // We rewrite the file even if it exists.
    int ar_fd = open(f_names[0], O_WRONLY | O_CREAT, S_IRWXUGO);
    if (ar_fd < 0)
    {
        printf("Opening of archive was not successful...");
        exit(0);
    }

    printf("sizeof(arhdr): %d", sizeof(struct ar_hdr));
    struct ar_hdr ar_header;
    long file_size = 0;
    memcpy(ar_header.ar_name, f_names[0], 16);
    snprintf(ar_header.ar_date, 12, "%ld", time(NULL)); // get current time
    snprintf(ar_header.ar_uid, 6, "%d", getuid());
    snprintf(ar_header.ar_gid, 6, "%d", getgid());
    // Figure out more elegant way for this later...
    //ar_header.ar_mode = "0666";
    // Need to do some calculations for this first.
    snprintf(ar_header.ar_size, 10, "%ld", file_size);
    memcpy(ar_header.ar_fmag, ARFMAG, 2);

    printf("sizeof(arhdr): %d", sizeof(ar_header));
}

// -t Print a concise table of contents
void contents(char *ar_fname, int verbose)
{
    int ar_fd = open(ar_fname, O_RDONLY, S_IRWXUGO);

}

//  -x Extract named files
void extract(char *ar_fname, int verbose)
{
    int ar_fd = open(ar_fname, O_RDONLY, S_IRWXUGO);

}

// -d Delete files from named archive
void delete(char **f_names, int verbose)
{
    int ar_fd = open(f_names[0], O_RDWR, S_IRWXUGO);

}

// -A Quickly append all files in the current directory
void append_all(char *ar_fname, int verbose)
{
    int ar_fd = open(ar_fname, O_RDONLY, S_IRWXUGO);

}
