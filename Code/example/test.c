#include <limits.h>    /* for CHAR_BIT */
#include <stdint.h>   /* for uint32_t */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint32_t realBlockStatus_t;
enum { BITS_PER_WORD = sizeof(realBlockStatus_t) * CHAR_BIT };
#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

void set_bit(realBlockStatus_t *realBlockStatus, int n) { 
    realBlockStatus[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

void clear_bit(realBlockStatus_t *realBlockStatus, int n) {
    realBlockStatus[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n)); 
}

int get_bit(realBlockStatus_t *realBlockStatus, int n) {
    realBlockStatus_t bit = realBlockStatus[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit != 0; 
}

int main () {
   FILE *fp;
   char c[] = "this is tutorialspoint";
   char c2[] = "It is a test";
   char buffer[100];

   /* Open file for both reading and writing */
   fp = fopen("file.txt", "w+");

   /* Write data to the file */
   fwrite(c, strlen(c) + 1, 1, fp);

   /* Seek to the beginning of the file */
   fseek(fp, 0, SEEK_SET);

    fwrite(c2, strlen(c2), 1, fp);

    fseek(fp, 0, SEEK_SET);

   /* Read and display data */
   fread(buffer, strlen(c)+1, 1, fp);
   printf("%s\n", buffer);
   fclose(fp);
   
   return(0);
}