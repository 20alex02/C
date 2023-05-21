#include "global.h"
#include "load_data.h"
#include "print_data.h"
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int parse_int(const char *string) {
    char *end;
    long number = strtol(string, &end, 10);
    if (errno != 0 || number < 0 || number > UINT_MAX || *string == '\0' || *end != '\0') {
        return -1;
    }
    return (int) number;
}

bool parse_args(int argc, char **argv, int options[OPTIONS_COUNT], int *opt_d_arg) {
    int opt;
    while((opt = getopt(argc, argv, "aspd:")) != -1) {
        switch(opt) {
            case 'a':
                options[OPT_A]++;
                break;
            case 's':
                options[OPT_S]++;
                break;
            case 'p':
                options[OPT_P]++;
                break;
            case 'd':
                options[OPT_D]++;
                *opt_d_arg = parse_int(optarg);
                if (*opt_d_arg == -1) {
                    return false;
                }
                break;
            default:
                return false;
        }
    }

    for (int option = 0; option < OPTIONS_COUNT; ++option) {
        if (options[option] > 1) {
            return false;
        }
    }
    return optind + 1 == argc;
}

void update_parents(struct tree_node *node) {
    for (int child = 0; child < node->children_count; ++child) {
        node->children[child].parent = node;
        update_parents(&node->children[child]);
    }
}

int main(int argc, char **argv) {
    int options[OPTIONS_COUNT] = {0};
    int opt_d_arg = -1;
    if (!parse_args(argc, argv, options, &opt_d_arg)) {
        fprintf(stderr, "Usage: %s [-asp] [-d NUMBER] <PATH>\n", argv[0]);
        return EXIT_FAILURE;
    }
    options[OPT_D] = opt_d_arg;

    char *path = argv[argc - 1];
    struct stat file_info;
    if (lstat(path, &file_info) == -1) {
        perror(path);
        return EXIT_FAILURE;
    }
    if (!S_ISREG(file_info.st_mode) && !S_ISDIR(file_info.st_mode) && !S_ISLNK(file_info.st_mode)) {
        return EXIT_SUCCESS;
    }
    struct tree_node root;
    init_tree_node(&root, NULL, &file_info, basename(path), options);
    if (S_ISDIR(file_info.st_mode) && !load_data(path, &root, options)) {
        destroy_tree(&root);
        return EXIT_FAILURE;
    }
    update_parents(&root);
    char *tree_prefix = malloc(sizeof(char) * 20);
    if (tree_prefix == NULL) {
        fprintf(stderr, "allocation failed\n");
        destroy_tree(&root);
        return EXIT_FAILURE;
    }
    if (!print_tree(&root, options, 0, &root, &tree_prefix, 20, path)) {
        free(tree_prefix);
        destroy_tree(&root);
        return EXIT_FAILURE;
    }
    free(tree_prefix);
    destroy_tree(&root);
    return EXIT_SUCCESS;
}
