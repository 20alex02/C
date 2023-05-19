#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

bool queue_init(struct queue *queue, size_t elem_size) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->elem_size = elem_size;
    return true;
}

struct node *create_new_node(size_t elem_size, const void *data) {
    struct node *new_node = malloc(sizeof(struct node));
    if (new_node == NULL) {
        return NULL;
    }
    new_node->data = malloc(elem_size);
    if (new_node->data == NULL) {
        free(new_node);
        return NULL;
    }
    memcpy(new_node->data, data, elem_size);
    new_node->next = NULL;
    return new_node;
}


bool queue_size_one(const struct queue *queue) {
    return queue->head != NULL && queue->head == queue->tail;
}

bool queue_is_empty(const struct queue *queue) {
    return queue->head == NULL;
}

void queue_connect_node(struct queue *queue, struct node *node) {
    if (queue_is_empty(queue)) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }
}

bool enqueue(struct queue *queue, const void *data) {
    struct node *node;
    if ((node = create_new_node(queue->elem_size, data)) == NULL) {
        return false;
    }
    queue_connect_node(queue, node);
    return true;
}

bool enqueue_posix(struct queue *queue, const void *data, pthread_mutex_t *mutex) {
    struct node *node;
    if ((node = create_new_node(queue->elem_size, data)) == NULL) {
        return false;
    }
    pthread_mutex_lock(mutex);
    queue_connect_node(queue, node);
//    printf("%lu inserted: %d\n", pthread_self(), *(int *) data);
    pthread_mutex_unlock(mutex);
    return true;
}

bool dequeue(struct queue *queue, void *data) {
    if (queue_is_empty(queue)) {
        return false;
    }
    if (data != NULL) {
        memcpy(data, queue->head->data, queue->elem_size);
    }
    struct node *head = queue->head;
    if (queue_size_one(queue)) {
        queue->tail = NULL;
        queue->head = NULL;
    } else {
        queue->head = queue->head->next;
    }
    free(head->data);
    free(head);
    return true;
}

bool dequeue_posix(struct queue *queue, void *data, pthread_mutex_t *mutex) {
    bool rv;
//    int popped;
    pthread_mutex_lock(mutex);
    rv = dequeue(queue, &data);
//    if (rv) {
//        printf("%lu popped: %d\n", pthread_self(), popped);
//    } else {
//        printf("%lu empty\n", pthread_self());
//    }
    pthread_mutex_unlock(mutex);
    return rv;
}

void queue_destroy(struct queue *queue) {
    struct node *node = queue->head;
    struct node *prev_node;
    while (node != NULL) {
        prev_node = node;
        node = node->next;
        free(prev_node->data);
        free(prev_node);
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->elem_size = 0;
}

/*********************************************************
 *                      LOCK FREE                        *
 *********************************************************/

bool queue_init_lock_free(struct queue *queue, size_t elem_size) {
    struct node *initial_node = malloc(sizeof(struct node));
    if (initial_node == NULL) {
        return false;
    }
    initial_node->next = NULL;
    initial_node->data = NULL;
    queue->head = initial_node;
    queue->tail = initial_node;
    queue->elem_size = elem_size;
    return true;
}

bool queue_is_empty_lock_free(struct queue *queue) {
    return queue->head->next == NULL;
}

bool enqueue_lock_free(struct queue *queue, void *data) {
    struct node *node;
    if ((node = create_new_node(queue->elem_size, data)) == NULL) {
        return false;
    }
    struct node *tail;
    do {
        tail = queue->tail;
    } while (!__sync_val_compare_and_swap(&tail->next, NULL, node));
    __sync_val_compare_and_swap(&queue->tail, tail, node);
//    printf("%lu inserted: %d\n", pthread_self(), *(int*)data);
    return true;
}


bool dequeue_lock_free(struct queue* queue, void *data) {
    struct node* head;
    struct node* tail;
    void *dequeued_data;
    do {
        head = queue->head;
        if (head->next == NULL) {
//            printf("%lu popped: none\n", pthread_self());
            return false;
        }
        dequeued_data = head->next->data;
    } while (!__sync_val_compare_and_swap(&queue->head, head, head->next));
//    printf("%lu popped: %d\n", pthread_self(), *(int*) dequeued_data);
    if (data != NULL) {
        memcpy(data, dequeued_data, queue->elem_size);
    }
    free(dequeued_data);
    free(head);
    return true;
}

void queue_destroy_lock_free(struct queue *queue) {
    struct node *node = queue->head->next;
    free(queue->head);
    struct node *prev_node;
    while (node != NULL) {
        prev_node = node;
        node = node->next;
        free(prev_node->data);
        free(prev_node);
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->elem_size = 0;
}
