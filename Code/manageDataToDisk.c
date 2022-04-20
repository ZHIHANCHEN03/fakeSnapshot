#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h> 
#include <uuid/uuid.h>
#include <string.h>

#define WRITE "WRITE"
#define DELETE "DELETE"
#define BREAK "BREAK"
#define PARALLEL_INPUT_SIZE 100

int main() {
    while ( 1 ) {
        char writeOrDeleteInput[5];
        printf("Write Or Delete or Break: \n");
        scanf("%s", writeOrDeleteInput);

        char writeOrDelete[strlen(writeOrDeleteInput)];
        for (int i = 0; i < strlen(writeOrDeleteInput); i++) {
            writeOrDelete[i] = toupper(writeOrDeleteInput[i]);
        }

        if (strcmp(writeOrDelete, BREAK) == 0) {
            break;
        } else if (strcmp(writeOrDelete, WRITE) == 0) {
            char parallelOrNot[5];
            char getLine[PARALLEL_INPUT_SIZE];
            printf("Parallel Writing: Y or N \n");
            scanf("%s", parallelOrNot);
            if ( strcmp(parallelOrNot, "Y") == 0 || strcmp(parallelOrNot, "YES") == 0 ) {
                printf("Enter Your Input for Simulate Input (Format: First Write Size, Second Write Size,...) ==> \n");
                scanf(" %[^\n]", getLine);
                printf("Input: %s\n", getLine);
            } else if ( strcmp(parallelOrNot, "N") == 0 || strcmp(parallelOrNot, "NO") == 0 ) {
                printf("Enter Your Input for Simulate Input (Format: Write Size) ==> \n");
                scanf(" %s", getLine);
                printf("Input: %s\n", getLine);
            } else {
                printf("Invalid Input!\n");
            }
        } else if (strcmp(writeOrDelete, DELETE) == 0) {
            char parallelOrNot[5];
            char getLine[PARALLEL_INPUT_SIZE];
            printf("Parallel Delete: Y or N \n");
            scanf("%s", parallelOrNot);
            if ( strcmp(parallelOrNot, "Y") == 0 || strcmp(parallelOrNot, "YES") == 0 ) {
                printf("Enter Your Input for Simulate Delete (Format: First Delete Blcok Name, Second Delete Blcok Name,...) ==> \n");
                scanf(" %[^\n]", getLine);
                printf("Input: %s\n", getLine);
            } else if ( strcmp(parallelOrNot, "N") == 0 || strcmp(parallelOrNot, "NO") == 0 ) {
                printf("Enter Your Input for Simulate Delete (Format: Delete Blcok Name) ==> \n");
                scanf(" %s", getLine);
                printf("Input: %s\n", getLine);
            } else {
                printf("Invalid Input!\n");
            }
        }
    }
}

// char * covertToUppercase(char * inputString) {
//     char returnString[strlen(inputString)];
//     for (int i = 0; i < strlen(inputString); i++) {
//         returnString[i] = toupper(inputString[i]);
//     }
//     return returnString;
// }