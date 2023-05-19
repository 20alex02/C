#ifndef HW03_PROCESS_DATA_H
#define HW03_PROCESS_DATA_H

#include "global.h"
#include <stdbool.h>
bool set_containers_neighbors(int container_count, struct container containers[container_count],
                              int path_count, struct path paths[path_count]);
void print_container_data(int container_count, struct container containers[container_count], struct options *options);

bool divide_containers_into_sites(int container_count, struct container containers[container_count], struct list *sites);
bool set_sites_neighbors(struct list *sites);
void sites_destroy(struct list *sites);
void print_sites(struct list sites);

bool init_dijkstra(struct list *sites, unsigned int source_id, unsigned int target_id);
void find_shortest_path(struct list *sites, unsigned int target_id, struct list *shortest_path);
void print_shortest_path(struct list *shortest_path);

#endif //HW03_PROCESS_DATA_H
