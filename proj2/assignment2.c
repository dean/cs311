#include <ar.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

void usage(char *[]);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage(argv);
        exit(0);
    }


}

void usage(char *argv[])
{
    printf("Usage: %s key afile name ...", argv[0]);
}
