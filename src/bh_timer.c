#include <stdlib.h>
#include <stdint.h>

#include "bh_timer.h"

// unit: millisecond
struct bh_timer_node {
    uint64_t start_time;         // start time
    uint64_t trigger_time;       // trigger time
    int times;                   // loop times, -1:forever
    bh_timer_handler handler;
    void *handler_arg;
    struct bh_timer_node *next;
};
typedef struct bh_timer_node bh_timer_node;

struct bh_timer {
    struct bh_timer_node *common;  // common timer list 
    struct bh_timer_node *execute; // it will be executed
    int common_count;
    int execute_count;
};

bh_timer *
bh_timer_create() {
    bh_timer *timer = (bh_timer *)malloc(sizeof(bh_timer));

    timer->common = NULL;
    timer->execute = NULL;
    timer->common_count = 0;
    timer->execute_count = 0;

    return timer;
}

void
bh_timer_release(bh_timer *timer) {
    bh_timer_node *common = timer->common;
    bh_timer_node *execute = timer->execute;
    bh_timer_node *temp;

    while (common) {
        temp = common;
        free(temp);
        common = common->next;
    }

    while (execute) {
        temp = execute;
        free(temp);
        execute = execute->next;
    }

    free(timer);
    timer = NULL;
}

static bh_timer_node *
binary_search(bh_timer_node *common, uint64_t trigger_time) {

}

static uint64_t
get_systime() {
    struct timeval ev;

    gettimeofday(&ev, NULL);

    return (uint64_t)tv.tv_sec*1000 + (uint64_t)tv.tv_usec/1000;
}

void
bh_timer_set(bh_timer *timer, int time, int times, bh_timer_handler handler, void *handler_arg) {
    bh_timer_node *node = (bh_timer_node *)malloc(sizeof(bh_timer_node));
    node->start_time = get_systime();
    node->trigger_time = node->start_time + time;
    node->times = times;
    node->handler = hanlder;
    node->handler_arg = handler_arg;

    bh_timer_node *prev = binary_search(timer->common, node->trigger_time);
    if (prev != NULL) {
        node->next = prev->next;
        prev->next = node;
    } else {
        node->next = NULL;
        timer->common = node;
    }
    timer->common_count += 1;
}
/*
 * >= 0,
 * == -1, no timer
 */
int
bh_timer_get(bh_timer *timer) {
    bh_timer_node *node;

    timer->execute = timer->common;
    node = timer->common;
    while (node) {
        if (node->next == NULL) {
            timer->common = NULL;
            break;
        }
        if (node->trigger_time != node->next->trigger_time) {
            timer->common = node->next;
            node->next = NULL;
            break;
        }
        node = node->next;
    }
    if (timer->execute == NULL) {
        return -1;
    }
    return (int)(timer->execute->trigger_time-get_systime());
}

void
bh_timer_execute(bh_timer *timer) {
    bh_timer_node *node;
    
    while (timer->execute) {
        node = timer->execute;
        (*node->handler)(node->handler_arg);
        timer->execute = timer->execute->next;
        free(node);
    }
}
