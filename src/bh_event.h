#ifndef _BH_EVENT_H_
#define _BH_EVENT_H_

#include <stdint.h>

typedef struct bh_event_node bh_event_node;
struct bh_event_node {
    int fd;
    uint32_t read;
    uint32_t write;
};

typedef struct bh_event bh_event;
struct bh_event {
    int event_fd;
    int max_events;
    bh_event_node *events;
};

//static bh_event *  bh_event_create();
//static void        bh_event_release(bh_event *event);
//static int         bh_event_add(bh_event *event, int sock_fd);
//static void        bh_event_del(bh_event *event, int sock_fd);
//static int         bh_event_write(bh_event *event, int sock_fd, int enable);
//static int         bh_event_poll(bh_event *event, int max, int timeout);

#ifdef __linux__
#include "bh_epoll.h"
#endif

#endif
