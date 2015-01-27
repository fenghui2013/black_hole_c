#ifndef _BH_EPOLL_H_
#define _BH_EPOLL_H_

#include <sys/epoll.h>

int
bh_event_create() {
    return epoll_create(1024);
}

void
bh_event_release(int event_fd) {
    close(event_fd);
}

int
bh_event_add(int event_fd, int sock_fd) {
    struct epoll_event ev;
    ev.events = EPOLLET | EPOLLIN;
    ev.data.fd = sock_fd;
    if (epoll_ctl(event_fd, EPOLL_CTL_ADD, sock_fd, &ev) == -1) {
        return 0;
    }
    return 1;
}

void
bh_event_del(int event_fd, int sock_fd) {
    epoll_ctl(event_fd, EPOLL_CTL_DEL, sock_fd, NULL);
}

int
bh_event_write(int event_fd, int sock_fd, int enable) {
    struct epoll_event ev;
    ev.events = EPOLLET | EPOLLIN | (enable ? EPOLLOUT : 0);
    ev.data.fd = sock_fd;
    if (epoll_ctl(event_fd, EPOLL_CTL_ADD, sock_fd, &ev) == -1) {
        return 0;
    }
    return 1;
}

int
bh_event_poll(int event_fd, bh_event *e, int max, int timeout) {
    struct epoll_event ev[max];
    int i;
    uint32_t flag;
    int n = epoll_wait(event_fd, ev, max, timeout);
    for (i=0; i<n; i++) {
        e[i].fd = ev[i].data.fd;
        flag = ev[i].events;
        e[i].write = (flag&EPOLLOUT) ? 1 : 0;
        e[i].read = (flag&EPOLLIN) ? 1 : 0;
    }
    return n;
}
#endif
