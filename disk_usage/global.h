#ifndef HW05_GLOBAL_H
#define HW05_GLOBAL_H

#include <stdbool.h>
#include <stdint.h>
#define FILE_NAME_LENGTH 256
#define BLOCK_SIZE 512
enum options {
    OPT_A,
    OPT_S,
    OPT_P,
    OPT_D,
    OPTIONS_COUNT,
};

struct tree_node {
    struct tree_node *parent;
    int children_count;
    int capacity;
    struct tree_node *children;
    bool is_last_child;

    bool error;
    uint64_t size;
    char name[FILE_NAME_LENGTH];

};

struct tree_prefix {
    char *prefix;
    int capacity;
    int length;
};

#endif //HW05_GLOBAL_H
