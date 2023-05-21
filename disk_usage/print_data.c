#include "print_data.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PREFIX_LENGTH 4

float get_percentage(uint64_t node_size, uint64_t root_size) {
    return floorf(node_size * 100 / (float) root_size * 10) / 10;
}

float get_units(uint64_t node_size, char unit_string[4]) {
    int unit = 50;
    while (unit > 0) {
        if (node_size >> unit > 0) {
            break;
        }
        unit -= 10;
    }
    switch (unit) {
        case 50:
            sprintf(unit_string, "PiB");
            break;
        case 40:
            sprintf(unit_string, "TiB");
            break;
        case 30:
            sprintf(unit_string, "GiB");
            break;
        case 20:
            sprintf(unit_string, "MiB");
            break;
        case 10:
            sprintf(unit_string, "KiB");
            break;
        default:
            sprintf(unit_string, "B  ");
            break;
    }
    return floorf(node_size / (float) (1ull << unit) * 10) / 10;
}

bool update_tree_prefix(char **prefix, int *capacity, int depth, bool parent_is_last) {
    if (depth < 2) {
        **prefix = '\0';
        return true;
    }
    if ((depth - 2) * PREFIX_LENGTH + PREFIX_LENGTH + 1 > *capacity) {
        *capacity *= 2;
        char *tmp = realloc(*prefix, sizeof(char) * (*capacity));
        if (tmp == NULL) {
            fprintf(stderr, "allocation failed\n");
            return false;
        }
        *prefix = tmp;
    }
    strcpy(*prefix + (depth - 2) * 4, parent_is_last ? "    " : "|   ");
    return true;
}

bool print_tree(struct tree_node *node, int options[OPTIONS_COUNT], int depth,
        struct tree_node *root, char **tree_prefix, int capacity, char *root_path) {
    char unit[4];
    if (options[OPT_D] != -1 && depth > options[OPT_D]) {
        return true;
    }
    if (root->error) {
        printf("%s", node->error ? "? " : "  ");
    }
    if (options[OPT_P]) {
        printf("%5.1f%% ", get_percentage(node->size, root->size));
    } else {
        printf("%6.1f %s ", get_units(node->size, unit), unit);
    }
    if (node->parent != NULL) {
        if (!update_tree_prefix(tree_prefix, &capacity, depth, node->parent->is_last_child)) {
            return false;
        }
        printf("%s%s ", *tree_prefix, node->is_last_child ? "\\--" : "|--");
        printf("%s\n", node->name);
    } else {
        printf("%s\n", root_path);
    }

    for (int child = 0; child < node->children_count; ++child) {
        print_tree(&node->children[child], options, depth + 1, root, tree_prefix, capacity, root_path);
    }
    return true;
}
