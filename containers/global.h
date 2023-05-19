#ifndef HW03_GLOBAL_H
#define HW03_GLOBAL_H
#include <stdbool.h>
#include "list.h"

enum program_options {
    OPT_T,
    OPT_C,
    OPT_P,
    OPT_S,
    OPT_G,
    OPTIONS
};
enum waste_types {
    PLASTICS_AND_ALUMINIUM,
    PAPER,
    BIODEGRADABLE_WASTE,
    CLEAR_GLASS,
    COLORED_GLASS,
    TEXTILE,
    WASTE_TYPES
};
struct options {
    bool options[OPTIONS], is_public;
    enum waste_types waste_types[WASTE_TYPES];
    unsigned int first, second;
};
struct container {
    unsigned int id, capacity, street_number;
    char *name, *street;
    double longitude, latitude;
    enum waste_types waste_type;
    bool is_public;
    struct list neighbors;
    struct site *site;
};
struct neighbor {
    void *neighbor;
    unsigned int id, distance;
};
struct path {
    unsigned int start_id, destination_id, distance;
};
struct site {
    unsigned int id, distance_from_source;
    double latitude, longitude;
    enum waste_types waste_types[WASTE_TYPES];
    struct list containers;
    struct list neighbors;
    struct site *previous_site;
};

#endif //HW03_GLOBAL_H
