#ifndef _BH_EPOLL_H_
#define _BH_EPOLL_H_

#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>

bh_event *
bh_event_create() {
    bh_event *event = (bh_event *)malloc(sizeof(bh_event));
    event->event_fd = epoll_create(1024);
    event->max_events = 1024;
    event->events = (bh_event_node *)malloc(1024*sizeof(bh_event_node));
    return event;
}

void
bh_event_release(bh_event *event) {
    free(event->events);
    close(event->event_fd);
    free(event);
}

int
bh_event_add(bh_event *event, int sock_fd) {
    struct epoll_event ev;
    ev.events = EPOLLET | EPOLLIN;
    ev.data.fd = sock_fd;
    if (epoll_ctl(event->event_fd, EPOLL_CTL_ADD, sock_fd, &ev) == -1) {
        return 0;
    }
    return 1;
}

void
bh_event_del(bh_event *event, int sock_fd) {
    epoll_ctl(event->event_fd, EPOLL_CTL_DEL, sock_fd, NULL);
}

int
bh_event_write(bh_event *event, int sock_fd, int enable) {
    struct epoll_event ev;
    ev.events = EPOLLET | EPOLLIN | (enable ? EPOLLOUT : 0);
    ev.data.fd = sock_fd;
    if (epoll_ctl(event->event_fd, EPOLL_CTL_MOD, sock_fd, &ev) == -1) {
        return 0;
    }
    return 1;
}

int
bh_event_poll(bh_evnet *event, int max, int timeout) {
    struct epoll_event ev[max];
    int i;
    uint32_t flag;
    int n = epoll_wait(event->event_fd, ev, max, timeout);
    for (i=0; i<n; i++) {
        event->events[i].fd = ev[i].data.fd;
        flag = ev[i].events;
        event->events[i].write = (flag&EPOLLOUT) ? 1 : 0;
        event->events[i].read = (flag&EPOLLIN) ? 1 : 0;
    }
    return n;
}
#endif
