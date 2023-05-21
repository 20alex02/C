#ifndef HW05_PRINT_DATA_H
#define HW05_PRINT_DATA_H

#include "global.h"
#include <stdbool.h>
bool print_tree(struct tree_node *node, int options[OPTIONS_COUNT], int depth,
        struct tree_node *root, char **tree_prefix, int capacity, char *root_path);
#endif //HW05_PRINT_DATA_H
