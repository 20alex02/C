#include "global.h"
#include "copy_data.h"
#include "data_source.h"
#include "process_data.h"
#include "program_arguments.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    struct options options;
    memset(options.options, false, OPTIONS * sizeof(bool));
    memset(options.waste_types, WASTE_TYPES, WASTE_TYPES * sizeof(enum waste_types));

    if (!parse_options(argc, argv, &options)) {
        fprintf(stderr, "Usage: %s [-t APBGCT] [-c volume_min-volume_max] [-p Y|N] | [-s] | [-g id_start,id_destination] <CONTAINERS_FILE> <PATHS_FILE>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!init_data_source(argv[argc - 2], argv[argc - 1])) {
        fprintf(stderr, "init_data_source failed\n");
        return EXIT_FAILURE;
    }

    int path_count = 0;
    while (get_path_a_id(path_count) != NULL) {
        path_count++;
    }
    struct path *paths = path_count == 0 ? NULL : malloc(path_count * sizeof(struct path));
    if (paths == NULL) {
        path_count = 0;
    }
    if (!copy_path_data(path_count, paths)) {
        free(paths);
        destroy_data_source();
        return EXIT_FAILURE;
    }

    int container_count = 0;
    while (get_container_id(container_count) != NULL) {
        container_count++;
    }
    struct container *containers = container_count == 0 ? NULL : malloc(container_count * sizeof(struct container));
    if (containers == NULL) {
        container_count = 0;
    }
    containers_init(container_count, containers);
    if (!copy_container_data(container_count, containers)) {
        containers_destroy(container_count, containers);
        free(paths);
        destroy_data_source();
        return EXIT_FAILURE;
    }
    destroy_data_source();

    if (!set_containers_neighbors(container_count, containers, path_count, paths)) {
        containers_destroy(container_count, containers);
        free(paths);
        return EXIT_FAILURE;
    }

    struct list sites;
    list_init(&sites, sizeof(struct site));
    if (options.options[OPT_S] || options.options[OPT_G]) {
        if (!divide_containers_into_sites(container_count, containers, &sites) ||
                !set_sites_neighbors(&sites)) {
            containers_destroy(container_count, containers);
            free(paths);
            sites_destroy(&sites);
            return EXIT_FAILURE;
        }
    }
    if (options.options[OPT_S]) {
        print_sites(sites);
    } else if (options.options[OPT_G]){
        struct list shortest_path;
        list_init(&shortest_path, sizeof(struct site));
        if (!init_dijkstra(&sites, options.first, options.second)) {
            fprintf(stderr, "Invalid site ID for shortest path\n");
            containers_destroy(container_count, containers);
            free(paths);
            sites_destroy(&sites);
            return EXIT_FAILURE;
        }
        find_shortest_path(&sites, options.second, &shortest_path);
        print_shortest_path(&shortest_path);
        sites_destroy(&shortest_path);
    } else {
        print_container_data(container_count, containers, &options);
    }

    containers_destroy(container_count, containers);
    free(paths);
    sites_destroy(&sites);
    return EXIT_SUCCESS;
}
