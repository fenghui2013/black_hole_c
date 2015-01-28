#include <stdlib.h>
#include <stdint.h>

#include "bh_module.h"
#include "bh_timer.h"

// unit: millisecond
struct bh_timer_node {
    uint64_t start_time;         // start time
    uint64_t trigger_time;       // trigger time
    int time;                    // timeout
    int times;                   // loop times, -1:forever
    char *handler_name;
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
_search(bh_timer_node *common, uint64_t trigger_time) {
    bh_timer_node *node = common, *prev = NULL;
    while (node) {
        if (trigger_time < node->trigger_time) {
            break;
        }
        prev = node;
        node = node->next;
    }
    return prev;
}

static uint64_t
_get_systime() {
    struct timeval ev;

    gettimeofday(&ev, NULL);

    return (uint64_t)tv.tv_sec*1000 + (uint64_t)tv.tv_usec/1000;
}

void
_bh_timer_set(bh_timer *timer, bh_timer_node *node) {
    bh_timer_node *prev = _search(timer->common, node->trigger_time);
    if (prev != NULL) {
        node->next = prev->next;
        prev->next = node;
    } else {
        node->next = timer->common;
        timer->common = node;
    }
    timer->common_count += 1;
}

void
bh_timer_set(bh_timer *timer, int time, int times, char *handler_name) {
    bh_timer_node *node = (bh_timer_node *)malloc(sizeof(bh_timer_node));
    node->start_time = _get_systime();
    node->trigger_time = node->start_time + time;
    node->time = time;
    node->times = times;
    node->handler_name = handler_name;

    _bh_timer_set(timer, node);
}

/*
 * >= 0, timeout time
 * == -1, no timer
 */
int
bh_timer_get(bh_timer *timer) {
    bh_timer_node *node;

    if (timer->execute != NULL) {
        return (int)(timer->execute->trigger_time-_get_systime());
    }
    timer->execute = timer->common;
    node = timer->common;
    while (node) {
        timer->execute_count += 1;
        timer->common_count -= 1;
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
    if (timer->execute_count == 0) {
        return -1;
    }
    return (int)(timer->execute->trigger_time-_get_systime());
}

void
bh_timer_execute(bh_timer *timer) {
    bh_timer_node *node;
    uint64_t current_time = _get_systime();
    
    while (timer->execute) {
        if (current_time < timer->execute->trigger_time) {
            break;
        }
        node = timer->execute;
        timer->execute = timer->execute->next;
        timer->execute_count -= 1;
        bh_module_timeout_handler(node->handler_name);
        if (node->times>0 || node->times==-1) {
            node->trigger_time = _get_systime() + node->time;
            node->times = (node->times==-1) ? node->times : (node->times-1);
            _bh_timer_set(timer, node);
        } else {
            free(node);
        }
    }
}
