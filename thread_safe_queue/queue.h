#ifndef HW01_QUEUE_H
#define HW01_QUEUE_H

#include <string.h>
#include <pthread.h>
#include <stdbool.h>

struct node {
    struct node *next;
    void *data;
};

/**
 * if used as lock free, it is initialized with dummy node,
 * head and tail are set to this node
 */
struct queue {
    struct node *head;
    struct node *tail;
    size_t elem_size;
};

/**
 * head points to the last dequeued element, head->next is the actual head
 * is empty when head->next == NULL
 */

bool queue_init(struct queue *queue, size_t elem_size);
bool queue_is_empty(const struct queue *queue);
bool enqueue(struct queue *queue, const void *data);
bool dequeue(struct queue *queue, void *data);
void queue_destroy(struct queue *queue);

bool queue_init_lock_free(struct queue *queue, size_t elem_size);
bool queue_is_empty_lock_free(struct queue *queue);
bool enqueue_lock_free(struct queue *queue, void *data);
bool dequeue_lock_free(struct queue* queue, void *data);
void queue_destroy_lock_free(struct queue *queue);

bool dequeue_posix(struct queue *queue, void *data, pthread_mutex_t *mutex);
bool enqueue_posix(struct queue *queue, const void *data, pthread_mutex_t *mutex);

#endif //HW01_QUEUE_H
