// #desc: Testy nanečisto
#define CUT
#include "libs/cut.h"

#include "libs/mainwrap.h"

#include <stdlib.h>

TEST(nanecisto_encode_zeroes)
{
    const unsigned char input[] = {
        0x00,
        0x00,
        0x00,
        0x00,
    };
    const unsigned char output[] = {
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };

    INPUT_BYTES(input);
    CHECK(app_main("-e") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(nanecisto_encode_assignment_example)
{
    const unsigned char input[] = { 0x00, 0x01, 0x02, 0x03 };
    const unsigned char output[] = { 0x20, 0x80, 0x04, 0x08, 0x06 };
    INPUT_BYTES(input);
    CHECK(app_main("-e") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(nanecisto_decode_zeroes)
{
    const unsigned char input[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
    const unsigned char output[] = { 0x00, 0x00, 0x00, 0x00 };

    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}

TEST(nanecisto_decode_assignment_example)
{
    const unsigned char input[] = { 0x20, 0x80, 0x04, 0x08, 0x06 };
    const unsigned char output[] = { 0x00, 0x01, 0x02, 0x03 };
    INPUT_BYTES(input);
    CHECK(app_main("-d") == EXIT_SUCCESS);
    CHECK_FILE(stderr, "");
    CHECK_BINARY_FILE(stdout, output);
}
