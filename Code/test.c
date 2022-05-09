#include <stdlib.h>
#include <stdio.h>
#include "bit_array.h"

int main() {
    int result;
    BIT_ARRAY * bitarr = bit_array_create(10000);
    // bit_array_print(bitarr, stdout);
    // fputc('\n', stdout);

    bit_array_set_bits(bitarr, 4, 77,102,154,2003);
    // bit_array_print(bitarr, stdout);
    // fputc('\n', stdout);

    printf("The number of bits set is %llu\n", bit_array_num_bits_set(bitarr));
    bit_array_find_first_set_bit(bitarr, &result);
    printf("The first bit that is set is locatated in %d\n", (int) result);

    bit_index_t result2;
    bit_array_find_next_set_bit(bitarr, 1512, &result2);
    printf("The first bit that is set after offset %d is locatated in %llu\n", (int) result+1, result2);

    bit_index_t result3;
    bit_array_find_last_set_bit(bitarr, &result3);
    printf("The last bit that is set is locatated in %d\n", (int) result3);

    return 0;
}