#include "load_data.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void destroy_tree(struct tree_node *node) {
    if (node == NULL || node->children == NULL) {
        return;
    }
    for (int child = 0; child < node->children_count; ++child) {
        destroy_tree(&node->children[child]);
    }
    free(node->children);
}

void init_tree_node(struct tree_node *node, struct tree_node *parent,
        struct stat *st, const char *file_name, int options[OPTIONS_COUNT]) {
    node->parent = parent;
    node->children_count = 0;
    node->capacity = 0;
    node->children = NULL;
    node->is_last_child = false;
    node->error = false;
    node->size = options[OPT_A]? st->st_size : st->st_blocks * BLOCK_SIZE;
    strncpy(node->name, file_name, FILE_NAME_LENGTH);
}

bool alloc_children(struct tree_node *node) {
    node->capacity = node->capacity == 0 ? 20 : node->capacity * 2;
    struct tree_node *tmp = realloc(node->children, node->capacity * sizeof(struct tree_node));
    if (tmp == NULL) {
        fprintf(stderr, "allocation failed\n");
        return false;
    }
    node->children = tmp;
    return true;
}

int filter(const struct dirent *file) {
    return strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0;
}

int compare_alphabetically(const void *p1, const void *p2) {
    struct tree_node *node1 = (struct tree_node *) p1;
    struct tree_node *node2 = (struct tree_node *) p2;
    char node1_name[FILE_NAME_LENGTH] = {0};
    char node2_name[FILE_NAME_LENGTH] = {0};
    strcpy(node1_name, node1->name);
    strcpy(node2_name, node2->name);
    for (int i = 0; i < FILE_NAME_LENGTH; ++i) {
        node1_name[i] = tolower(node1_name[i]);
        node2_name[i] = tolower(node2_name[i]);
    }
    int diff = strcmp(node1_name, node2_name);
    return diff ? diff : strcmp(node1->name, node2->name);
}

int compare_size(const void *p1, const void *p2) {
    struct tree_node *node1 = (struct tree_node *) p1;
    struct tree_node *node2 = (struct tree_node *) p2;
    int diff = node2->size - node1->size;
    return diff == 0 ? compare_alphabetically(p1, p2) : diff;
}

bool load_data(const char *path, struct tree_node *node, int options[OPTIONS_COUNT]) {
    DIR *dir = NULL;
    if ((dir = opendir(path)) == NULL) {
        perror(path);
        node->error = true;
        if (node->parent != NULL) {
            node->parent->error = true;
        }
        return true;
    }

    char full_path[PATH_MAX];
    struct dirent *file = NULL;
    struct stat info;
    errno = 0;
    while ((file = readdir(dir)) != NULL) {
        if (!filter(file)) {
            continue;
        }
        snprintf(full_path, PATH_MAX - 1, "%s/%s", path, file->d_name);
        if (lstat(full_path, &info) == -1) {
            perror(full_path);
            continue;
        }
        if (!S_ISREG(info.st_mode) && !S_ISDIR(info.st_mode) && !S_ISLNK(info.st_mode)) {
            continue;
        }
        if (node->children_count >= node->capacity && !alloc_children(node)) {
            closedir(dir);
            return false;
        }
        node->children_count++;
        init_tree_node(&node->children[node->children_count - 1], node, &info, file->d_name, options);
        if (S_ISDIR(info.st_mode)) {
            if (!load_data(full_path, &node->children[node->children_count - 1], options)) {
                closedir(dir);
                return false;
            }
            if (node->error && node->parent != NULL) {
                node->parent->error = true;
            }
        }
        node->size += node->children[node->children_count - 1].size;
    }
    /*
    if (errno != 0) {
        perror(path);
        node->error = true;
        if (node->parent != NULL) {
            node->parent->error = true;
        }
    }
     */
    if (node->children_count > 0) {
        qsort(node->children, node->children_count, sizeof(struct tree_node), options[OPT_S] ? compare_size : compare_alphabetically);
        node->children[node->children_count - 1].is_last_child = true;
    }
    closedir(dir);
    return true;
}
