#ifndef HW05_LOAD_DATA_H
#define HW05_LOAD_DATA_H

#include "global.h"
#include <stdbool.h>
#include <sys/stat.h>
bool load_data(const char *path, struct tree_node *node, int options[OPTIONS_COUNT]);
void init_tree_node(struct tree_node *node, struct tree_node *parent,
                    struct stat *st, const char *file_name, int options[OPTIONS_COUNT]);
void destroy_tree(struct tree_node *node);

#endif //HW05_LOAD_DATA_H
