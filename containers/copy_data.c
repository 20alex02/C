#include "copy_data.h"
#include "data_source.h"
#include "list.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

void containers_init(int container_count, struct container containers[container_count]) {
    for (int container = 0; container < container_count; ++container) {
        containers[container].name = NULL;
        containers[container].street = NULL;
        containers[container].street_number = UINT_MAX;
        list_init(&containers[container].neighbors, sizeof(struct neighbor));
    }
}

void containers_destroy(int container_count, struct container containers[container_count]) {
    for (int container = 0; container < container_count; ++container) {
        free(containers[container].name);
        free(containers[container].street);
        list_destroy(&containers[container].neighbors);
    }
    free(containers);
}

bool unique_id(int current_container, struct container *containers) {
    for (int container = 0; container < current_container; ++container) {
        if (containers[container].id == containers[current_container].id) {
            return false;
        }
    }
    return true;
}

bool copy_waste_type(enum waste_types *destination, const char *source) {
    if (strcmp(source, "Plastics and Aluminium") == 0) {
        *destination = PLASTICS_AND_ALUMINIUM;
    } else if (strcmp(source, "Paper") == 0) {
        *destination = PAPER;
    } else if (strcmp(source, "Biodegradable waste") == 0) {
        *destination = BIODEGRADABLE_WASTE;
    } else if (strcmp(source, "Clear glass") == 0) {
        *destination = CLEAR_GLASS;
    } else if (strcmp(source, "Colored glass") == 0) {
        *destination = COLORED_GLASS;
    } else if (strcmp(source, "Textile") == 0) {
        *destination = TEXTILE;
    } else {
        return false;
    }
    return true;
}

bool copy_number(unsigned int *destination, const char *source, bool is_optional) {
    if (is_optional && strlen(source) == 0) {
        return true;
    }
    char *end;
    long number = strtol(source, &end, 10);
    if (errno != 0 || number < 0 || number >= UINT_MAX || *source == '\0' || *end != '\0') {
        return false;
    }
    *destination = number;
    return true;
}

bool copy_string(char **destination, const char *source, bool is_optional) {
    if (is_optional && strlen(source) == 0) {
        return true;
    }
    *destination = malloc((strlen(source) + 1) * sizeof(char));
    if (*destination == NULL) {
        return false;
    }
    strcpy(*destination, source);
    return true;
}

bool copy_is_public(bool *destination, const char *source) {
    if (strcmp(source, "Y") == 0) {
        *destination = true;
        return true;
    }
    if (strcmp(source, "N") == 0) {
        *destination = false;
        return true;
    }
    return false;
}

bool copy_decimal_number(double *destination, const char *source) {
    char *decimal_point = strchr(source, '.');
    if (decimal_point != NULL && strlen(decimal_point) > 16) {
        return false;
    }
    char *end;
    *destination = strtod(source, &end);
    if (errno != 0 || *source == '\0' || *end != '\0') {
        return false;
    }
    return true;
}

bool copy_container_data(int container_count, struct container containers[container_count]) {
    for (int container = 0; container < container_count; ++container) {
        if (!copy_number(&containers[container].id, get_container_id(container), false) ||
            !copy_decimal_number(&containers[container].latitude ,get_container_x(container)) ||
            !copy_decimal_number(&containers[container].longitude ,get_container_y(container)) ||
            !copy_waste_type(&containers[container].waste_type, get_container_waste_type(container)) ||
                !copy_number(&containers[container].capacity, get_container_capacity(container), false) ||
                !copy_number(&containers[container].street_number, get_container_number(container), true) ||
                !copy_is_public(&containers[container].is_public, get_container_public(container))) {
            fprintf(stderr, "Invalid container format at line %d\n", container + 1);
            return false;
        }
        if (!copy_string(&containers[container].name, get_container_name(container), true) ||
                !copy_string(&containers[container].street, get_container_street(container), true)) {
            fprintf(stderr, "Malloc failed\n");
            return false;
        }
        if (!unique_id(container, containers)) {
            fprintf(stderr, "Duplicate ID at line %d\n", container + 1);
            return false;
        }
    }
    return true;
}

bool copy_path_data(int path_count, struct path paths[path_count]) {
    for (int path = 0; path < path_count; ++path) {
        if (!copy_number(&paths[path].start_id, get_path_a_id(path), false) ||
                !copy_number(&paths[path].destination_id, get_path_b_id(path), false) ||
                !copy_number(&paths[path].distance, get_path_distance(path), false)) {
            fprintf(stderr, "Invalid path format at line %d\n", path + 1);
            return false;
        }
    }
    return true;
}
