#include <stdlib.h>
#include <string.h>
#include "capture.h"

int main(int argc, char **argv)
{
    if (argc != 5) {
        fprintf(stderr, "invalid argc, expected 5\n");
        return EXIT_FAILURE;
    }

    uint8_t from_mask[4];
    uint8_t from_mask_len;
    if (sscanf(argv[2], "%hhu.%hhu.%hhu.%hhu/%hhu", &from_mask[0], &from_mask[1], &from_mask[2], &from_mask[3], &from_mask_len) != 5) {
        fprintf(stderr, "loading <from+mask> failed\n");
        return EXIT_FAILURE;
    }
    if (from_mask_len > 32) {
        fprintf(stderr, "invalid arg <from+mask>, expected: bits <= 32\n");
        return EXIT_FAILURE;
    }

    uint8_t to_mask[4];
    uint8_t to_mask_len;
    if (sscanf(argv[3], "%hhu.%hhu.%hhu.%hhu/%hhu", &to_mask[0], &to_mask[1], &to_mask[2], &to_mask[3], &to_mask_len) != 5) {
        fprintf(stderr, "loading <to+mask> failed\n");
        return EXIT_FAILURE;
    }
    if (to_mask_len > 32) {
        fprintf(stderr, "invalid arg <to+mask>, expected: bits <= 32\n");
        return EXIT_FAILURE;
    }

    if ((strcmp(argv[4], "flowstats") != 0) && (strcmp(argv[4], "longestflow") != 0)) {
        fprintf(stderr, "invalid arg <statistic>, expected: <flowstats> or <longestflow>\n");
        return EXIT_FAILURE;
    }

    struct capture_t capture[1];
    if (load_capture(capture, argv[1]) != 0) {
        fprintf(stderr, "load capture failed\n");
        return EXIT_FAILURE;
    }

    struct capture_t filtered1[1];
    if (filter_from_mask(capture, filtered1, from_mask, from_mask_len) != 0) {
        destroy_capture(capture);
        fprintf(stderr, "filter from mask failed\n");
        return EXIT_FAILURE;
    }

    struct capture_t filtered2[1];
    if (filter_to_mask(filtered1, filtered2, to_mask, to_mask_len) != 0) {
        destroy_capture(capture);
        destroy_capture(filtered1);
        fprintf(stderr, "filter to mask failed\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[4], "flowstats") == 0) {
        if (print_flow_stats(filtered2) != 0) {
            destroy_capture(capture);
            destroy_capture(filtered1);
            destroy_capture(filtered2);
            return EXIT_FAILURE;
        }
    } else {
        if (print_longest_flow(filtered2) != 0) {
            destroy_capture(capture);
            destroy_capture(filtered1);
            destroy_capture(filtered2);
            return EXIT_FAILURE;
        }
    }
    destroy_capture(capture);
    destroy_capture(filtered1);
    destroy_capture(filtered2);
    return EXIT_SUCCESS;
}
