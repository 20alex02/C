#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdbool.h>

struct node {
    struct node *next;
    struct node *prev;
    void *data;
};

struct list {
    struct node *head;
    struct node *tail;
    size_t elem_size;
};

void list_init(struct list *list, size_t element_size);

bool list_push_back(struct list *list, const void *data);

bool list_push_front(struct list *list, const void *data);

bool list_insert_after_node(struct list *list, struct node *node, const void *data);

size_t list_size(const struct list *list);

bool list_is_empty(const struct list *list);

bool list_pop_front(struct list *list, void *data);

bool list_pop_back(struct list *list, void *data);

bool list_remove_node(struct list *list, struct node *node_to_be_removed, void *data);

void list_destroy(struct list *list);

#endif // LIST_H
