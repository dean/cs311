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

void make_header(struct ar_hdr * header, char * fname, time_t time, uid_t uid, gid_t gid, mode_t PERMS, long filesize)
{
    memcpy(header->ar_name, fname, 16);
    snprintf(header->ar_date, 12, "%ld", time); // get current time
    snprintf(header->ar_uid, 6, "%d", uid);
    snprintf(header->ar_gid, 6, "%d", gid);
    // Figure out more elegant way for this later...
    snprintf(header->ar_mode, 8, "%o", PERMS);
    // Need to do some calculations for this first.
    snprintf(header->ar_size, 10, "%ld", filesize);
    memcpy(header->ar_fmag, ARFMAG, 2);
}

// -q Quickly append all files to archive
void append(char **f_names, int num_files, int verbose)
{
    // We rewrite the file even if it exists.
    int ar_fd = open(f_names[0], O_WRONLY | O_CREAT, S_IRWXUGO);
    if (ar_fd < 0)
    {
        printf("Opening of archive was unsuccessful...");
        exit(0);
    }

    // Need to calculate this out
    long filesize = 0;
    struct ar_hdr ar_header;
    struct ar_hdr header;
    int contents;

    // Total up file size
    for(int i=1; i<num_files; i++)
    {
        int fd = open(f_names[i], O_RDONLY);
        struct stat sb;
        fstat(fd, &sb);

        // Write header
        filesize += sizeof(header);
        filesize += sb.st_size;

        // Account for odd filesize
        int odd = sb.st_size % 2;

        if (odd == 1)
        {
            filesize += 1;
        }

    }

    // Initial archive header
    make_header(&ar_header, f_names[0], time(NULL), getuid(), getgid(), S_IRWUGO, filesize);
    write(ar_fd, &ar_header, sizeof(ar_header));

    for(int i=1; i<num_files; i++)
    {
        int fd = open(f_names[i], O_RDONLY);
        struct stat sb;
        fstat(fd, &sb);
        make_header(&header, f_names[i], sb.st_mtime, sb.st_uid, sb.st_gid, sb.st_mode, sb.st_size);

        // Write header
        write(ar_fd, &header, sizeof(header));

        int num_written = 0;
        int num_read = 0;
        char * buf;
        // random comment
        while((num_read = read(fd, buf, BLOCKSIZE)) > 0)
        {
            num_written = write(ar_fd, buf, BLOCKSIZE);
            if (num_written != num_read)
            {
                printf("Num written not equal to num_read on file %s!", f_names[i]);
                exit(0);
            }
        }

        // Account for even offset
        int odd = sb.st_size % 2;

        if (odd == 1)
        {
            write(ar_fd, "\n", 1);
        }

    }

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
