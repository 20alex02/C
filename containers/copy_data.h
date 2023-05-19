#ifndef HW03_COPY_DATA_H
#define HW03_COPY_DATA_H
#include <stdbool.h>

#include "global.h"

void containers_init(int container_count, struct container containers[container_count]);
void containers_destroy(int container_count, struct container containers[container_count]);
bool copy_container_data(int container_count, struct container containers[container_count]);
bool copy_path_data(int path_count, struct path paths[path_count]);

#endif //HW03_COPY_DATA_H
