#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define FIRST_BIT 1
#define LAST_BIT 39
#define BYTE 8
#define ENCODED_LENGTH 5
#define DECODED_LENGTH 4

uint64_t get_bit(uint64_t b, int n) {
    return (b & (1l << n));
}

void print_bytes(uint64_t bytes, int byte_count)
{
    int byte;
    for (int i = 0; i < byte_count; i++) {
        byte = 0;
        byte = (int) (byte | (bytes >> ((byte_count - 1 - i) * BYTE)));
        putchar(byte);
    }
}

uint64_t xor (uint64_t encoded, int parity_index) {
    uint64_t result = 0;
    for (int i = FIRST_BIT; i < LAST_BIT; i++) {
        if ((i & parity_index) != 0) {
            if ((get_bit(encoded, LAST_BIT - i)) != 0) {
                result ^= 1;
            }
        }
    }
    return result;
}

uint64_t encode_word(uint32_t decoded)
{
    uint64_t encoded = 0;
    int parity_bit_index = 1;
    int parity_bit_count = 1;
    // copy bits to encoded at the correct indexes
    for (int i = FIRST_BIT; i < LAST_BIT; i++) {
        if (i == parity_bit_index) {
            parity_bit_index <<= 1;
            parity_bit_count++;
            continue;
        }
        encoded |= ((get_bit(decoded, LAST_BIT - BYTE - i + parity_bit_count)) << (BYTE - parity_bit_count));
    }
    // set parity bits
    for (int i = 1; i < LAST_BIT; i <<= 1) {
        encoded |= (xor(encoded, i) << (LAST_BIT - i));
    }
    return encoded;
}

bool encode(void)
{
    uint32_t decoded = 0;
    int input_byte;
    int bytes_loaded = 0;

    while ((input_byte = getchar()) != EOF) {
        bytes_loaded++;
        decoded <<= BYTE;
        decoded |= input_byte;
        if (bytes_loaded % DECODED_LENGTH == 0) {
            print_bytes(encode_word(decoded),ENCODED_LENGTH);
            decoded = 0;
        }
    }

    if (bytes_loaded % DECODED_LENGTH != 0) {
        decoded <<= (BYTE * (DECODED_LENGTH - (bytes_loaded % DECODED_LENGTH)));
        print_bytes(encode_word(decoded),ENCODED_LENGTH);
    }
    return true;
}

uint64_t detect_mistake(uint64_t encoded, int bytes_loaded)
{
    int mistake_index = 0;
    for (int i = FIRST_BIT; i < LAST_BIT; i++) {
        if (get_bit(encoded, LAST_BIT - i) != 0) {
            mistake_index ^= i;
        }
    }
    if (mistake_index == 0) {
        return encoded;
    }
    encoded ^= (1l << (LAST_BIT - mistake_index));
    fprintf(stderr, "One-bit error in byte %d\n", bytes_loaded - (ENCODED_LENGTH - (mistake_index / BYTE)));
    return encoded;
}

uint32_t decode_word(uint64_t encoded)
{
    uint32_t decoded = 0;
    int parity_bit_index = 1;
    int parity_bit_count = 1;

    for (int i = FIRST_BIT; i < LAST_BIT; i++) {
        if (i == parity_bit_index) {
            parity_bit_index <<= 1;
            parity_bit_count++;
            continue;
        }
        decoded |= (get_bit(encoded, LAST_BIT - i) >> (BYTE - parity_bit_count));
    }
    return decoded;
}

bool decode(void)
{
    uint64_t encoded = 0;
    int input_byte;
    int bytes_loaded = 0;

    while ((input_byte = getchar()) != EOF) {
        bytes_loaded++;
        encoded <<= BYTE;
        encoded |= input_byte;
        if (bytes_loaded % ENCODED_LENGTH == 0) {
            encoded = detect_mistake(encoded, bytes_loaded);
            print_bytes(decode_word(encoded), DECODED_LENGTH);
            encoded = 0;
        }
    }

    if (bytes_loaded % ENCODED_LENGTH != 0) {
        fprintf(stderr, "Wrong code word\n");
        return false;
    }
    return true;
}

/*************************************
 * DO NOT MODIFY THE FUNCTION BELLOW *
 *************************************/
int main(int argc, char *argv[])
{
    if (argc > 2) {
        fprintf(stderr, "Usage: %s (-e|-d)\n", argv[0]);
        return EXIT_FAILURE;
    }
    bool is_success = false;
    if (argc == 1 || (strcmp(argv[1], "-e") == 0)) {
        is_success = encode();
    } else if (strcmp(argv[1], "-d") == 0) {
        is_success = decode();
    } else {
        fprintf(stderr, "Unknown argument: %s\n", argv[1]);
    }
    return is_success ? EXIT_SUCCESS : EXIT_FAILURE;
}
