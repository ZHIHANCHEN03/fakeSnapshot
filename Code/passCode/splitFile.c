#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <uuid/uuid.h>
#include <string.h>

#define DATA_SIZE 4000

int main() {
    char * filename = "test.bin";
    int fd1 = open( filename, O_RDONLY );
    uuid_t binuuid;
    int fileCount = 0;

    printf( "fd is %d\n", fd1 );

    if ( fd1 == -1 )
    {
        perror( "open() failed" );
        return EXIT_FAILURE;       /* exit( EXIT_FAILURE ); */
    }

    char buffer[DATA_SIZE];
    while ( 1 ) {
        int rc = read( fd1, buffer, 4000 );
        if (rc == 0) {
            break;
        }
        // printf( "read() returned %d -- read \"%s\"\n", rc, buffer );

        fileCount++;

        uuid_generate_random(binuuid);
        char * uuid = malloc(37);
        uuid_unparse(binuuid, uuid);

        char tmpname[50];
        strcpy(tmpname, "./data/");
        strcat(tmpname, uuid);

        int fd2 = open( tmpname, O_WRONLY | O_APPEND | O_CREAT, 0644 );
        if ( fd2 == -1 )
        {
            perror( "create failed" );
            return EXIT_FAILURE;       /* exit( EXIT_FAILURE ); */
        }
        write( fd2, buffer, DATA_SIZE );
        printf("Write to the file %s with %d count(s)\n", tmpname, fileCount);

        free(uuid);
    }

    return EXIT_SUCCESS;
}