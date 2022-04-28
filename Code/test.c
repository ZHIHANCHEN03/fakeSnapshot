#include <limits.h>    /* for CHAR_BIT */
#include <stdint.h>   /* for uint32_t */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint32_t word_t;
enum { BITS_PER_WORD = sizeof(word_t) * CHAR_BIT };
#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

void set_bit(word_t *words, int n) { 
    words[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

void clear_bit(word_t *words, int n) {
    words[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n)); 
}

int get_bit(word_t *words, int n) {
    word_t bit = words[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit != 0; 
}

int main() {
    word_t * words = malloc(32);
    *words = 123321;
    word_t * words2 = malloc(32);
    *words2 = 123321;
    set_bit(words, 1);
    set_bit(words2, 1);

    printf("Get Bit => %d\n", get_bit(words, 1));
}