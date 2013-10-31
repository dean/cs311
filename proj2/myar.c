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
void contents(char *, int);

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
                contents(argv[optind], 0);
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

void fill_space(char * arr, int size)
{
    int flag = 0;
    for(int i=0; i< size; i++)
    {
        if(arr[i] == '\0' || flag == 1)
        {
            flag = 1;
            arr[i] = ' ';
        }
    }
}

void make_header(struct ar_hdr * header, char * fname, time_t time, uid_t uid, gid_t gid, mode_t PERMS, long filesize)
{

    memcpy(header->ar_name, fname, 16);
    fill_space(header->ar_name, 16);
    snprintf(header->ar_date, 12, "%ld", time); // get current time
    fill_space(header->ar_date, 12);
    snprintf(header->ar_uid, 6, "%d", uid);
    fill_space(header->ar_uid, 6);
    snprintf(header->ar_gid, 6, "%d", gid);
    fill_space(header->ar_gid, 6);
    // Figure out more elegant way for this later...
    snprintf(header->ar_mode, 8, "%o", PERMS);
    fill_space(header->ar_mode, 8);
    // Need to do some calculations for this first.
    snprintf(header->ar_size, 10, "%ld", filesize);
    fill_space(header->ar_size, 10);
    memcpy(header->ar_fmag, ARFMAG, 2);
}

// -q Quickly append all files to archive
void append(char **f_names, int num_files, int verbose)
{
    // We rewrite the file even if it exists.
    int ar_fd = open(f_names[0], O_RDWR | O_CREAT, S_IRWXUGO);
    if (ar_fd < 0)
    {
        printf("Opening of archive was unsuccessful...");
        exit(0);
    }

    // Need to calculate this out
    int contents;
    char buf[BLOCKSIZE];

    // Write archive identifier
    int n_read;
    if (n_read = read(ar_fd, buf, BLOCKSIZE) == 0)
    {
        lseek(ar_fd, 0, SEEK_SET);
        write(ar_fd, ARMAG, SARMAG);
    }
    else
    {
        lseek(ar_fd, 0, SEEK_END);
    }

    printf("numfiles: %d", num_files);
    for(int i=1; i<num_files; i++)
    {
        int fd = open(f_names[i], O_RDONLY);
        struct stat sb;
        fstat(fd, &sb);
        struct ar_hdr header;
        make_header(&header, f_names[i], sb.st_mtime, sb.st_uid, sb.st_gid, sb.st_mode, sb.st_size);

        printf("writing header...");
        // Write header
        write(ar_fd, &header, sizeof(header));

        // Seek past header
        printf("not supposed to write yet");

        int num_written = 0;
        int num_read;
        printf("started writing...");
        while((num_read = read(fd, buf, BLOCKSIZE)) > 0)
        {
            printf("buf=%s\n", buf);
            num_written = write(ar_fd, buf, BLOCKSIZE);
            printf("buf=%s\n", buf);
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

void extract_string(char * src, int start_index, int count, char * dest)
{
    for(int i = start_index, j = 0; i < start_index + count; i++, j++)
    {
        if (src[i] != ' ')
        {
            dest[j] = src[i];
        }
    }
}

// -t Print a concise table of contents
void contents(char *ar_fname, int verbose)
{
    int ar_fd = open(ar_fname, O_RDWR, S_IRWXUGO);
    lseek(ar_fd, 8, SEEK_SET); // skip ar declaration.
    char header[60];
    int num_read;
    while((num_read = read(ar_fd, header, 60)) > 0)
    {

        int i = 0;
        char name[16];
        char size[10]; // Needed for lseek
        extract_string(header, 0, 16, name);
        /*
        for(; i<16; i++)
        {
            if (header[i] != ' ')
            {
                name[i] = header[i];
            }
        }
        */
        if (verbose != 1)
        {
            char date[12];
            extract_string(header, 16, 12, date);
            char uid[6];
            extract_string(header, 28, 6, uid);
            char gid[6];
            extract_string(header, 34, 6, gid);
            char mode[8];
            extract_string(header, 40, 8, mode);
            extract_string(header, 48, 10, size);
            for(int j=3; j<6; j++)
            {
                char tmp[1];
                tmp[0] = mode[j];
                int mode_num = atoi(tmp);
                printf("%c", mode_num == 4 || mode_num == 6 || mode_num == 7 ? 'r' : '-');
                printf("%c", mode_num == 2 || mode_num == 3 || mode_num == 6 || mode_num == 7 ? 'w' : '-');
                printf("%c", mode_num == 1 || mode_num == 3 || mode_num == 5 || mode_num == 7 ? 'x' : '-');
            }
            printf(" ");
            printf("%d/%d\t", atoi(uid), atoi(gid));
            printf("%ld ", atol(size));
            time_t t = (time_t) atol(date);
            struct tm *temp;
            temp = localtime(&t);
            char c[100];
            size_t a;
            a = strftime(c, sizeof(c), "%b %d %M:%S %Y ", temp);
            printf("%s", c);
/*
            //long filesize = atol(size);
            //lseek(ar_fd, filesize, SEEK_CUR);
            if (filesize % 2 == 1)
            {
                lseek(ar_fd, 1, SEEK_CUR);
            }
  */      }
        printf("%s", name);

        lseek(ar_fd, sizeof(header) + atol(size), SEEK_CUR);
        printf("\n");
    }
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
