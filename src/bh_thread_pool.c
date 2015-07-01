#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bh_thread_pool.h"

typedef struct bh_thread_task bh_thread_task;
struct bh_thread_task {
    int task_id;
    int is_running;
    bh_task task;
    void *task_arg;
    bh_task_arg_handler handler;
    bh_thread_task *next;
};

typedef struct bh_running_thread bh_running_thread;
struct bh_running_thread {
    pthread_t thread_id;
    int sock_fd;
};

struct bh_thread_pool {
    pthread_mutex_t queue_ready_lock;
    pthread_cond_t queue_ready;
    bh_thread_task *queue;
    bh_thread_task *queue_current_read;
    bh_thread_task *queue_current_write;
    int current_task_count;
    int current_free_count;
    int shutdown;
    pthread_t *threadid;
    int max_thread_num;
    int current_thread_num;
    bh_running_thread *running_threads;
};

static bh_thread_pool *pool = NULL;

#define POOL_EXPANSION_THRESHOLD 1024

static void
_bh_running_thread_add(pthread_t thread_id, int sock_fd) {
    bh_running_thread *temp_thread = (bh_running_thread *)&(pool->running_threads[pool->current_thread_num]);
    temp_thread->thread_id = thread_id;
    temp_thread->sock_fd = sock_fd;
    pool->current_thread_num += 1;
}

static void
_bh_running_thread_change(int i, pthread_t thread_id, int sock_fd) {
    int j = 0;;
    bh_running_thread *temp_thread = NULL;
    if (i == -1) {
        for (j=0; j<pool->current_thread_num; j++) {
            temp_thread = (bh_running_thread*)&(pool->running_threads[j]);
            if (pthread_equal(temp_thread->thread_id, thread_id)) {
                temp_thread->sock_fd = sock_fd;
                return;
            }
        }
    } else {
        temp_thread = (bh_running_thread *)&(pool->running_threads[i]);
        temp_thread->thread_id = thread_id;
        temp_thread->sock_fd = sock_fd;
    }
}

/*
 * check if the thread can run the task
 * 0, no
 * 1, yes
 */
static int
_bh_running_thread_check(pthread_t thread_id, int sock_fd) {
    assert(sock_fd!=0);
    int i, j = -1;
    bh_running_thread *temp_thread;

    for (i=0; i<pool->current_thread_num; i++) {
        temp_thread = (bh_running_thread *)&(pool->running_threads[i]);
        if (sock_fd == temp_thread->sock_fd) {
            return 0;
        }
        if (pthread_equal(thread_id, temp_thread->thread_id)) {
            j = i;
        }
    }
    _bh_running_thread_change(j, thread_id, sock_fd);
    return 1;
}

static const struct timespec
_get_timeout(int sec, int nsec) {
    struct timeval now;
    struct timespec timeout;

    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + sec;
    timeout.tv_nsec = now.tv_usec*1000 + nsec;

    return timeout;
}

void *
_task_killer(void *arg) {
    pthread_t thread_id = pthread_self();
    bh_thread_task *task = NULL;

    pthread_mutex_lock(&(pool->queue_ready_lock));
    _bh_running_thread_add(thread_id, 0);
    pthread_mutex_unlock(&(pool->queue_ready_lock));

    while (1) {
        pthread_mutex_lock(&(pool->queue_ready_lock));
        while ((pool->current_task_count==0 || (pool->current_task_count!=0 && !_bh_running_thread_check(thread_id, pool->queue_current_read->task_id)))
                && !pool->shutdown) {
            const struct timespec timeout = _get_timeout(0, 1000000*100);
            //const struct timespec timeout = _get_timeout(1, 0);
            pthread_cond_timedwait(&(pool->queue_ready), &(pool->queue_ready_lock), &timeout);
        }

        if (pool->shutdown) {
            pthread_mutex_unlock(&(pool->queue_ready_lock));
            pthread_exit(NULL);
        }

        task = pool->queue_current_read;
        task->is_running = 1;
        pool->queue_current_read = pool->queue_current_read->next;
        pool->current_task_count -= 1;
        pthread_mutex_unlock(&(pool->queue_ready_lock));

        (*(task->task))(task->task_arg);
        (*(task->handler))(task->task_arg);

        pthread_mutex_lock(&(pool->queue_ready_lock));
        task->is_running = 0;
        pool->current_free_count += 1;
        _bh_running_thread_change(-1, thread_id, 0);
        pthread_mutex_unlock(&(pool->queue_ready_lock));
    }
}

static bh_thread_task *
_bh_thread_task_init() {
    bh_thread_task *temp_task = NULL;
    temp_task = (bh_thread_task *)malloc(sizeof(bh_thread_task));
    temp_task->task_id = 0;
    temp_task->is_running = 0;
    temp_task->task = NULL;
    temp_task->task_arg = NULL;
    temp_task->handler = NULL;
    temp_task->next = NULL;
    return temp_task;
}

static bh_thread_task *
_bh_thread_pool_queue_init(int queue_size) {
    int i;
    bh_thread_task *queue = NULL, *temp_queue = NULL, *temp_task = NULL;

    queue = _bh_thread_task_init();
    temp_queue = queue;

    for (i=1; i<queue_size; i++) {
        temp_task = _bh_thread_task_init();
        temp_queue->next = temp_task;
        temp_queue = temp_queue->next;
    }
    temp_queue->next = queue;
    return queue;
}

static void
_bh_thread_pool_queue_expansion(bh_thread_task *current_write, bh_thread_task *current_write_next) {
    int i;
    bh_thread_task *temp_queue = current_write, *temp_task = NULL;

    for (i=0; i < POOL_EXPANSION_THRESHOLD; i++) {
        temp_task = _bh_thread_task_init();
        temp_queue->next = temp_task;
        temp_queue = temp_queue->next;
    }

    temp_queue->next = current_write_next;

    pool->current_free_count += POOL_EXPANSION_THRESHOLD;
}

bh_thread_pool *
bh_thread_pool_create(int max_threads, int queue_size) {
    int i, res;

    pool = (bh_thread_pool *)malloc(sizeof(bh_thread_pool));

    res = pthread_mutex_init(&(pool->queue_ready_lock), NULL);
    if (res != 0) {
        printf("pthread_mutex_init failed\n");
        exit(0);
    }

    res = pthread_cond_init(&(pool->queue_ready), NULL);
    if (res != 0) {
        printf("pthread_cond_init failed\n");
        exit(0);
    }

    pool->queue = _bh_thread_pool_queue_init(queue_size);
    pool->queue_current_read = pool->queue;
    pool->queue_current_write = pool->queue;
    pool->current_task_count = 0;
    pool->current_free_count = queue_size;
    pool->shutdown = 0;
    pool->max_thread_num = max_threads;
    pool->current_thread_num = 0;
    pool->threadid = (pthread_t *)malloc(max_threads*sizeof(pthread_t));
    memset(pool->threadid, 0, max_threads*sizeof(pthread_t));
    pool->running_threads = (bh_running_thread *)malloc(max_threads*sizeof(bh_running_thread));
    memset(pool->running_threads, 0, max_threads*sizeof(bh_running_thread));

    for (i=0; i<max_threads; i++) {
        pthread_create((pthread_t *)&(pool->threadid[i]), NULL, _task_killer, NULL);
    }

    return pool;
}

void
bh_thread_pool_add_task(bh_thread_pool *thread_pool, bh_task task, void *task_arg, int task_id, bh_task_arg_handler handler) {
    bh_thread_task *temp_task = NULL;

    pthread_mutex_lock(&(thread_pool->queue_ready_lock));

    if (thread_pool->current_free_count==1 || thread_pool->queue_current_write->next->is_running) {
        _bh_thread_pool_queue_expansion(thread_pool->queue_current_write, thread_pool->queue_current_write->next);
    }
    temp_task = thread_pool->queue_current_write;
    temp_task->task_id = task_id;
    temp_task->is_running = 0;
    temp_task->task = task;
    temp_task->task_arg = task_arg;
    temp_task->handler = handler;
    thread_pool->queue_current_write = thread_pool->queue_current_write->next;
    thread_pool->current_task_count += 1;
    thread_pool->current_free_count -= 1;
    pthread_cond_signal(&(thread_pool->queue_ready));
    pthread_mutex_unlock(&(thread_pool->queue_ready_lock));
}

void
bh_thread_pool_release(bh_thread_pool *thread_pool) {
    int i;
    bh_thread_task *temp_task;
    
    if (thread_pool->shutdown) return;
    while (pool->current_task_count != 0) {
        sleep(1);
    }
    thread_pool->shutdown = 1;

    pthread_cond_broadcast(&(thread_pool->queue_ready));
    for (i=0; i<thread_pool->max_thread_num; i++) {
        pthread_join(thread_pool->threadid[i], NULL);
    }

    free(thread_pool->threadid);
    free(thread_pool->running_threads);

    for (i=0; i<pool->current_free_count; i++) {
        temp_task = pool->queue;
        pool->queue = pool->queue->next;
        free(temp_task);
    }

    pthread_mutex_destroy(&(thread_pool->queue_ready_lock));
    pthread_cond_destroy(&(thread_pool->queue_ready));

    free(thread_pool);
    thread_pool = NULL;
}
