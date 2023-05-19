#include "process_data.h"
#include "list.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/*********************************************************
 *                   PRINT AND FILTER                    *
 *********************************************************/

bool add_to_sorted_list(struct list *neighbors, void *neighbor, unsigned int neighbor_id, unsigned int distance) {
    struct neighbor new_neighbor = {.neighbor = neighbor, .distance = distance, .id = neighbor_id};
    if (list_is_empty(neighbors) || new_neighbor.id < ((struct neighbor *) neighbors->head->data)->id) {
        return list_push_front(neighbors, &new_neighbor);
    }
    struct node *node = neighbors->head;
    while (node->next != NULL && new_neighbor.id >= ((struct neighbor *) node->next->data)->id) {
        node = node->next;
    }
    if (new_neighbor.id == ((struct neighbor *) node->data)->id) {
        return true;
    }
    return list_insert_after_node(neighbors, node, &new_neighbor);
}

bool set_containers_neighbors(int container_count, struct container containers[container_count],
        int path_count, struct path paths[path_count]) {
    struct container *start_container;
    struct container *destination_container;
    for (int path = 0; path < path_count; ++path) {
        start_container = NULL;
        destination_container = NULL;
        for (int container = 0; container < container_count; ++container) {
            if (containers[container].id == paths[path].start_id) {
                start_container = &containers[container];
            }
            if (containers[container].id == paths[path].destination_id) {
                destination_container = &containers[container];
            }
            if (start_container != NULL && destination_container != NULL) {
                if (!add_to_sorted_list(&start_container->neighbors, destination_container, destination_container->id, paths[path].distance) ||
                        !add_to_sorted_list(&destination_container->neighbors, start_container, start_container->id, paths[path].distance)) {
                    fprintf(stderr, "Malloc failed\n");
                    return false;
                }
                break;
            }
        }
        if (start_container == NULL || destination_container == NULL) {
            fprintf(stderr, "Paths file contains id which is not in containers file\n");
            return false;
        }
    }
    return true;
}

bool filter_capacity(struct container container, unsigned int min_capacity, unsigned int max_capacity) {
    return container.capacity >= min_capacity && container.capacity <= max_capacity;
}

bool filter_is_public(struct container container, bool is_public) {
    return is_public == container.is_public;
}

bool filter_waste_type(struct container container, const enum waste_types waste_types[WASTE_TYPES]) {
    for (int waste_type = 0; waste_type < WASTE_TYPES; ++waste_type) {
        if (waste_types[waste_type] == container.waste_type) {
            return true;
        }
    }
    return false;
}

char *get_waste_type(enum waste_types waste_type) {
    switch (waste_type) {
        case PLASTICS_AND_ALUMINIUM:
            return "Plastics and Aluminium";
        case PAPER:
            return "Paper";
        case BIODEGRADABLE_WASTE:
            return "Biodegradable waste";
        case CLEAR_GLASS:
            return "Clear glass";
        case COLORED_GLASS:
            return "Colored glass";
        case TEXTILE:
            return "Textile";
        default:
            return NULL;
    }
}

void print_container_data(int container_count, struct container containers[container_count], struct options *options) {
    struct node *node;
    for (int container = 0; container < container_count; ++container) {
        if ((!options->options[OPT_T] || filter_waste_type(containers[container], options->waste_types)) &&
                (!options->options[OPT_C] || filter_capacity(containers[container], options->first, options->second)) &&
                (!options->options[OPT_P] || filter_is_public(containers[container], options->is_public))) {
            printf("ID: %u, Type: %s, Capacity: %u, Address:", containers[container].id, get_waste_type(containers[container].waste_type), containers[container].capacity);
            if (containers[container].street != NULL) {
                printf(" %s", containers[container].street);
            }
            if (containers[container].street_number != UINT_MAX) {
                printf(" %u", containers[container].street_number);
            }
            printf(", Neighbors:");
            node = containers[container].neighbors.head;
            while (node != NULL) {
                printf(" %d", ((struct container *)((struct neighbor *) node->data)->neighbor)->id);
                node = node->next;
            }
            printf("\n");
        }
    }
}

/*********************************************************
 *                        SITES                          *
 *********************************************************/

bool equal_coordinates(double first, double second) {
    return fabs(first - second) <= 0.00000000000001;
}

void sites_destroy(struct list *sites) {
    struct node *node = sites->head;
    struct site *site;
    while (node != NULL) {
        site = node->data;
        list_destroy(&site->containers);
        list_destroy(&site->neighbors);
        node = node->next;
    }
    list_destroy(sites);
}

bool create_new_site(struct site *site, unsigned int id, struct container *container) {
    site->id = id;
    memset(site->waste_types, WASTE_TYPES, WASTE_TYPES * sizeof(enum waste_types));
    site->waste_types[container->waste_type] = container->waste_type;
    site->latitude = container->latitude;
    site->longitude = container->longitude;
    list_init(&site->containers, sizeof(struct container *));
    list_init(&site->neighbors, sizeof(struct neighbor));
    return list_push_back(&site->containers, &container);
}

bool connect_site_and_container(struct container *container, struct list *sites) {
    struct node *node = sites->head;
    struct site *site;
    while (node != NULL) {
        site = node->data;
        if (equal_coordinates(site->latitude, container->latitude) &&
            equal_coordinates(site->longitude, container->longitude)) {
            site->waste_types[container->waste_type] = container->waste_type;
            container->site = site;
            return list_push_back(&site->containers, &container);
        }
        node = node->next;
    }
    struct site new_site;
    unsigned int id = sites->head == NULL ? 1 : site->id + 1;
    if (!create_new_site(&new_site, id, container) ||
            !list_push_back(sites, &new_site)) {
        return false;
    }
    container->site = sites->tail->data;
    return true;
}

bool divide_containers_into_sites(int container_count, struct container containers[container_count], struct list *sites) {
    for (int container = 0; container < container_count; ++container) {
        if (!connect_site_and_container(&containers[container], sites)) {
            fprintf(stderr, "Malloc failed\n");
            return false;
        }
    }
    return true;
}

bool set_sites_neighbors(struct list *sites) {
    struct node *site_node, *container_node, *neighbor_node;
    struct site *site;
    struct container *container;
    struct neighbor *neighbor;
    site_node = sites->head;
    while (site_node != NULL) {
        site = site_node->data;
        container_node = site->containers.head;
        while (container_node != NULL) {
            neighbor_node = (*((struct container **)container_node->data))->neighbors.head;
            while (neighbor_node != NULL) {
                neighbor = neighbor_node->data;
                container = neighbor->neighbor;
                if (!add_to_sorted_list(&site->neighbors, container->site, container->site->id, neighbor->distance)) {
                    fprintf(stderr, "Malloc failed\n");
                    return false;
                }
                neighbor_node = neighbor_node->next;
            }
            container_node = container_node->next;
        }
        site_node = site_node->next;
    }
    return true;
}

char encode_waste_types(enum waste_types waste_type) {
    switch (waste_type) {
        case PLASTICS_AND_ALUMINIUM:
            return 'A';
        case PAPER:
            return 'P';
        case BIODEGRADABLE_WASTE:
            return 'B';
        case CLEAR_GLASS:
            return 'G';
        case COLORED_GLASS:
            return 'C';
        case TEXTILE:
            return 'T';
        default:
            return '\0';
    }
}

void print_waste_types(enum waste_types waste_types[WASTE_TYPES]) {
    char encoded_waste_type;
    for (int waste_type = 0; waste_type < WASTE_TYPES; ++waste_type) {
        encoded_waste_type = encode_waste_types(waste_types[waste_type]);
        if (encoded_waste_type != '\0') {
            printf("%c", encoded_waste_type);
        }
    }
    printf(";");
}

void print_neighbor_sites(struct site *site) {
    struct node *node = site->neighbors.head;
    while (node != NULL) {
        if (node != site->neighbors.head) {
            printf(",");
        }
        printf("%d", ((struct neighbor *)node->data)->id);
        node = node->next;
    }
    printf("\n");
}

void print_sites(struct list sites) {
    struct node *node = sites.head;
    struct site *site;
    while (node != NULL) {
        site = node->data;
        printf("%d;", site->id);
        print_waste_types(site->waste_types);
        print_neighbor_sites(site);
        node = node->next;
    }
}

/*********************************************************
 *                   SHORTEST PATH                       *
 *********************************************************/

void find_closest_site_node(struct list *sites, struct node **closest_site_node) {
    struct node *node = sites->head;
    struct site *site;
    *closest_site_node = sites->head;
    while (node != NULL) {
        site = node->data;
        if (site->distance_from_source < ((struct site *)(*closest_site_node)->data)->distance_from_source) {
            *closest_site_node = node;
        }
        node = node->next;
    }
}

void add_to_shortest_path(struct list *sites, struct list *shortest_path, struct node *closest_site_node) {
    if (list_is_empty(sites)) {
        return;
    }
    if (sites->head == sites->tail) {
        sites->head = NULL;
        sites->tail = NULL;
    } else if (closest_site_node == sites->head) {
        sites->head = closest_site_node->next;
        sites->head->prev = NULL;
    } else if (closest_site_node == sites->tail) {
        sites->tail = closest_site_node->prev;
        sites->tail->next = NULL;
    } else {
        closest_site_node->prev->next = closest_site_node->next;
        closest_site_node->next->prev = closest_site_node->prev;
    }

    if (list_is_empty(shortest_path)) {
        shortest_path->head = closest_site_node;
        shortest_path->tail = closest_site_node;
        closest_site_node->prev = NULL;
    } else {
        shortest_path->tail->next = closest_site_node;
        closest_site_node->prev = shortest_path->tail;
        shortest_path->tail = closest_site_node;
    }
    closest_site_node->next = NULL;
}

bool init_dijkstra(struct list *sites, unsigned int source_id, unsigned int target_id) {
    struct site *site;
    struct node *node = sites->head;
    bool found_source = false;
    bool found_target = false;
    while (node != NULL) {
        site = node->data;
        if (site->id == source_id) {
            found_source = true;
            site->distance_from_source = 0;
        } else {
            site->distance_from_source = UINT_MAX;
            if (site->id == target_id) {
                found_target = true;
            }
        }
        site->previous_site = NULL;
        node = node->next;
    }
    return found_source && found_target;
}

void find_shortest_path(struct list *sites, unsigned int target_id, struct list *shortest_path) {
    struct site *site;
    struct node *node, *closest_site_node;
    struct neighbor *neighbor;
    struct site *neighbor_site;
    unsigned int alt;
    while (!list_is_empty(sites)) {
        find_closest_site_node(sites, &closest_site_node);
        add_to_shortest_path(sites, shortest_path, closest_site_node);
        site = closest_site_node->data;
        if (site->id == target_id) {
            return;
        }
        node = site->neighbors.head;
        while (node != NULL) {
            neighbor = node->data;
            neighbor_site = neighbor->neighbor;
            alt = site->distance_from_source + neighbor->distance;
            if (alt < neighbor_site->distance_from_source) {
                neighbor_site->distance_from_source = alt;
                neighbor_site->previous_site = shortest_path->tail->data;
            }
            node = node->next;
        }
    }
}

void print_shortest_path(struct list *shortest_path) {
    if (list_is_empty(shortest_path)) {
        printf("No path between specified sites\n");
        return;
    }
    struct node *node = shortest_path->head;
    struct site *site;
    while (node != NULL) {
        site = node->data;
        printf("%d", site->id);
        if (node != shortest_path->tail) {
            printf("-");
        }
        node = node->next;
    }
    printf(" %d\n", ((struct site *)shortest_path->tail->data)->distance_from_source);
}
