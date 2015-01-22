#ifndef _BH_EVENT_H_
#define _BH_EVENT_H_

#include <stdint.h>

typedef struct bh_event {
    void * s;
    uint32_t read;
    uint32_t write;
} bh_event;

int  bh_event_create();
void bh_event_release(int event_fd);
int  bh_event_add(int event_fd, int sock_fd, void *ud);
void bh_event_del(int event_fd, int sock_fd);
int  bh_event_write(int event_fd, int sock_fd, void *ud, int enable);
int  bh_event_poll(int event_fd, bh_event *e, int max);

#ifdef __linux__
#include "bh_epoll.h"
#endif

#endif
