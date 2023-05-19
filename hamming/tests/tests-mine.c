/*
 * You can write your own tests in this file
 */

#define CUT
#include "libs/cut.h"

#include "libs/mainwrap.h"

#include <stdlib.h>

TEST(decode_wrong_code_word)
{
    const unsigned char input[] = { 0x20, 0x80, 0x04, 0x08 };
    //    const unsigned char output[] = {};
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_FAILURE);
    CHECK_FILE(stderr, "Wrong code word\n");
    //    CHECK_BINARY_FILE(stdout, output);
}

TEST(decode_assignment_example_and_wrong_code_word)
{
    const unsigned char input[] = { 0x20, 0x80, 0x04, 0x08, 0x06, 0x04 };
    const unsigned char output[] = { 0x00, 0x01, 0x02, 0x03 };
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_FAILURE);
    CHECK_FILE(stderr, "Wrong code word\n");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(decode_5_bytes)
{
    const unsigned char input[] = { 0x62, 0x99, 0x03, 0x30, 0x84 };
    const unsigned char output[] = { 0x23, 0x20, 0xcc, 0x02 };
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(decode_10_bytes)
{
    const unsigned char input[] = { 0x08, 0x33, 0x0c, 0x14, 0x60, 0x62, 0x36, 0x88, 0x6a, 0xa0 };
    const unsigned char output[] = { 0x06, 0x63, 0x05, 0x30, 0x26, 0xc2, 0x1a, 0x90 };
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(decode_to_ff_only)
{
    const unsigned char input[] = { 0x17, 0xff, 0xff, 0xff, 0x7e };
    const unsigned char output[] = { 0xff, 0xff, 0xff, 0xff };
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(encode_2_bytes)
{
    const unsigned char input[] = { 0xb5 };
    const unsigned char output[] = { 0x1b, 0x28, 0x00, 0x00, 0x00 };

    INPUT_BYTES(input);
    CHECK(app_main("-e") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(encode_3_bytes)
{
    const unsigned char input[] = { 0x01, 0x02, 0x03 };
    const unsigned char output[] = { 0x40, 0x88, 0x88, 0x0c, 0x00 };

    INPUT_BYTES(input);
    CHECK(app_main("-e") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(encode_4_bytes)
{
    const unsigned char input[] = { 0x0e, 0x20, 0xea, 0x11 };
    const unsigned char output[] = { 0x08, 0xf1, 0x83, 0xa8, 0x22 };

    INPUT_BYTES(input);
    CHECK(app_main("-e") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(encode_5_bytes)
{
    const unsigned char input[] = { 0x97, 0x3c, 0x88, 0x20, 0x61 };
    const unsigned char output[] = { 0x71, 0xb9, 0xf2, 0x20, 0xc0, 0x6e, 0x88, 0x00, 0x00, 0x00 };

    INPUT_BYTES(input);
    CHECK(app_main("-e") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(encode_8_bytes)
{
    const unsigned char input[] = { 0x06, 0x34, 0xaa, 0x3d, 0xde, 0x66, 0xa0, 0x25 };
    const unsigned char output[] = { 0x28, 0x31, 0x52, 0xa8, 0xfa, 0x35, 0x73, 0x1a, 0x80, 0xca };

    INPUT_BYTES(input);
    CHECK(app_main("-e") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(encode_ff_only)
{
    const unsigned char input[] = { 0xff, 0xff, 0xff, 0xff };
    const unsigned char output[] = { 0x17, 0xff, 0xff, 0xff, 0x7e };

    INPUT_BYTES(input);
    CHECK(app_main("-e") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(decode_bonus)
{
    const unsigned char input[] = { 0x30, 0x80, 0x04, 0x08, 0x06 };
    const unsigned char output[] = { 0x00, 0x01, 0x02, 0x03 };
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "One-bit error in byte 0\n");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(decode_bonus_long)
{
    const unsigned char input[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x21, 0x48, 0x40, 0xe2 };
    const unsigned char output[] = { 0x00, 0x00, 0x00, 0x00, 0xc4, 0x32, 0x14, 0x31 };
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "One-bit error in byte 8\n");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(decode_bonus_parity_bit)
{
    const unsigned char input[] = { 0x5a, 0x82, 0x58, 0x88, 0x88 };
    const unsigned char output[] = { 0xa0, 0x56, 0x22, 0x04 };
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "One-bit error in byte 2\n");
    CHECK_BINARY_FILE(stdout, output);
}
