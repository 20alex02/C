#include "list.h"
#include <string.h>


void list_init(struct list *list, size_t element_size)
{
    if (list == NULL || element_size <= 0) {
        return;
    }
    list->head = NULL;
    list->tail = NULL;
    list->elem_size = element_size;
}

bool create_new_node(struct list *list, const void *data, struct node **new_node) {
    *new_node = (struct node *) malloc(sizeof(struct node));
    if (new_node == NULL) {
        return false;
    }
    (*new_node)->data = malloc(list->elem_size);
    if ((*new_node)->data == NULL) {
        free(*new_node);
        return false;
    }
    memcpy((*new_node)->data, data, list->elem_size);
    return true;
}

bool list_push_back(struct list *list, const void *data)
{
    if (list == NULL || data == NULL) {
        return false;
    }
    struct node *new_node;
    if (!create_new_node(list, data, &new_node)) {
        return false;
    }
    new_node->next = NULL;
    if (list_is_empty(list)) {
        new_node->prev = NULL;
        list->head = new_node;
        list->tail = new_node;
    } else {
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    }
    return true;
}

bool list_push_front(struct list *list, const void *data)
{
    if (list == NULL || data == NULL) {
        return false;
    }
    struct node *new_node;
    if (!create_new_node(list, data, &new_node)) {
        return false;
    }
    new_node->prev = NULL;
    if (list_is_empty(list)) {
        new_node->next = NULL;
        list->head = new_node;
        list->tail = new_node;
    } else {
        new_node->next = list->head;
        list->head->prev = new_node;
        list->head = new_node;
    }
    return true;
}

bool list_insert_after_node(struct list *list, struct node *node, const void *data)
{
    if (list == NULL || data == NULL) {
        return false;
    }
    if (node == NULL || list_is_empty(list)) {
        return list_push_back(list, data);
    }
    struct node *new_node;
    if (!create_new_node(list, data, &new_node)) {
        return false;
    }
    new_node->prev = node;
    new_node->next = node->next;
    if (node == list->tail) {
        list->tail = new_node;
    } else {
        node->next->prev = new_node;
    }
    node->next = new_node;
    return true;
}

size_t list_size(const struct list *list)
{
    if (list == NULL) {
        return 0;
    }
    size_t size = 0;
    struct node *node = list->head;
    while (node != NULL) {
        size++;
        node = node->next;
    }
    return size;
}

bool list_is_empty(const struct list *list)
{
    if (list == NULL) {
        return false;
    }
    return list->head == NULL;
}

bool list_pop_front(struct list *list, void *data)
{
    if (list == NULL || list_is_empty(list)) {
        return false;
    }
    if (data != NULL) {
        memcpy(data, list->head->data, sizeof(list->elem_size));
    }
    struct node *head = list->head;
    if (list_size(list) == 1) {
        list->tail = NULL;
        list->head = NULL;
        list->elem_size = 0;
    } else {
        list->head = list->head->next;
        list->head->prev = NULL;
    }
    free(head->data);
    free(head);
    return true;
}

bool list_pop_back(struct list *list, void *data)
{
    if (list == NULL || list_is_empty(list)) {
        return false;
    }
    if (data != NULL) {
        memcpy(data, list->tail->data, sizeof(list->elem_size));
    }
    struct node *tail = list->tail;
    if (list_size(list) == 1) {
        list->tail = NULL;
        list->head = NULL;
        list->elem_size = 0;
    } else {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
    }
    free(tail->data);
    free(tail);
    return true;
}

bool list_remove_node(struct list *list, struct node *node_to_be_removed, void *data) {
    if (list == NULL || list_is_empty(list) || node_to_be_removed == NULL) {
        return false;
    }
    if (data != NULL) {
        memcpy(data, node_to_be_removed->data, sizeof(list->elem_size));
    }
    struct node *node = list->head;
    while (node != NULL) {
        if (node == node_to_be_removed) {
            if (node == list->head) {
                list_pop_front(list, NULL);
            } else if (node == list->tail) {
                list_pop_back(list, NULL);
            } else {
                node->prev->next = node->next;
                node->next->prev = node->prev;
                free(node->data);
                free(node);
            }
            return true;
        }
        node = node->next;
    }
    return false;
}

void list_destroy(struct list *list)
{
    if (list == NULL) {
        return;
    }
    struct node *node = list->head;
    struct node *prev_node;
    while (node != NULL) {
        prev_node = node;
        node = node->next;
        free(prev_node->data);
        free(prev_node);
    }
    list->head = NULL;
    list->tail = NULL;
    list->elem_size = 0;
}
