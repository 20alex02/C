#include "queue.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define THREADS 10 // 20 threads in total
#define OPERATIONS_PER_THREAD 10000

pthread_mutex_t mutex;

void *enqueue_routine_lock_free(void *arg) {
    struct queue *queue = (struct queue *) arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        enqueue_lock_free(queue, &i);
    }
    return NULL;
}

void *dequeue_routine_lock_free(void *arg) {
    struct queue *queue = (struct queue *) arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        dequeue_lock_free(queue, &i);
    }
    return NULL;
}

void *enqueue_routine_posix(void *arg) {
    struct queue *queue = (struct queue *) arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        enqueue_posix(queue, &i, &mutex);
    }
    return NULL;
}

void *dequeue_routine_posix(void *arg) {
    struct queue *queue = (struct queue *) arg;
    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        dequeue_posix(queue, NULL, &mutex);
    }
    return NULL;
}

long test(void *enqueue_routine(void *arg),
          void *dequeue_routine(void *arg),
          bool init(struct queue *queue, size_t elem_size),
          void destroy(struct queue *queue)) {
    long rv = -1;
    clock_t start, end;
    struct queue queue;
    init(&queue, sizeof(int));
    pthread_t producers[THREADS];
    pthread_t consumers[THREADS];
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "mutex init failed\n");
        goto error;
    }

    start = clock();
    for (int thread = 0; thread < THREADS; ++thread) {
        if (pthread_create(&producers[thread], NULL, enqueue_routine, &queue) != 0 ||
                pthread_create(&consumers[thread], NULL, dequeue_routine, &queue) != 0) {
            fprintf(stderr, "thread create failed\n");
            goto error;
        }
    }
    for (int thread = 0; thread < THREADS; ++thread) {
        if (pthread_join(producers[thread], NULL) != 0 ||
                pthread_join(consumers[thread], NULL) != 0) {
            fprintf(stderr, "thread join failed\n");
            goto error;
        }
    }
    end = clock();
    if (pthread_mutex_destroy(&mutex) != 0) {
        fprintf(stderr, "mutex init failed\n");
        goto error;
    }
    rv = end - start;
error:
    destroy(&queue);
    return rv;
}


int main() {
    printf("525178\n");
    long posix_time = test(enqueue_routine_posix,
                           dequeue_routine_posix,
                           queue_init,
                           queue_destroy);
    long lock_free_time = test(enqueue_routine_lock_free,
                               dequeue_routine_lock_free,
                               queue_init_lock_free,
                               queue_destroy_lock_free);
    if (posix_time == -1 || lock_free_time == -1) {
        return EXIT_FAILURE;
    }
    long result = lock_free_time * 100 / posix_time;
    printf("%ld\n", result);
    return EXIT_SUCCESS;
}
