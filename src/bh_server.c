#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "bh_thread_pool.h"
#include "bh_module.h"
#include "bh_event.h"
#include "bh_buffer.h"
#include "bh_socket.h"
#include "bh_engine.h"
#include "bh_timer.h"
#include "bh_server.h"

typedef struct bh_client bh_client;
struct bh_client {
    int sock_fd;  // client socket fd
    char *ip;
    int port;
    char *type;
    bh_buffer *recv_buffer;
    bh_buffer *send_buffer;
    bh_client *prev;
    bh_client *next;
};

typedef struct bh_clients bh_clients;
struct bh_clients {
    bh_client *first;
    bh_client *last;
    int clients_count;
};

struct bh_server {
    int sock_fd;
    char *ip;
    int port;
    bh_clients *clients;
    pthread_mutex_t clients_lock;
};

struct bh_accept_task_arg {
    bh_server_accept_task server_accept_task;
    bh_module *module;
    bh_event *event;
    bh_server *server;
    char *type;
};

bh_accept_task_arg *
bh_accept_task_generator(bh_module *module, bh_event *event, bh_server *server, char *type) {
    bh_accept_task_arg *accept_task_arg = (bh_accept_task_arg *)malloc(sizeof(bh_accept_task_arg));

    accept_task_arg->module = module;
    accept_task_arg->event = event;
    accept_task_arg->server = server;
    accept_task_arg->type = type;
    accept_task_arg->server_accept_task = bh_server_client_accept;

    return accept_task_arg;
}

void
bh_accept_task_executer(bh_accept_task_arg *accept_task_arg) {
    (*(accept_task_arg->server_accept_task))(accept_task_arg->module, accept_task_arg->event, accept_task_arg->server, accept_task_arg->type);
}

void
bh_accept_task_terminator(bh_accept_task_arg *accept_task_arg) {
    free(accept_task_arg);
    accept_task_arg = NULL;
}

//struct bh_connect_task_arg {
//    bh_server_connect_task server_connect_task;
//    bh_module *module;
//    bh_event *event;
//    bh_server *server;
//    char *ip;
//    int port;
//    char *type;
//};
//
//bh_connect_task_arg *
//bh_connect_task_generator(bh_module *module, bh_event *event, bh_server *server, const char *ip, int port, char *type) {
//    bh_connect_task_arg *connect_task_arg = (bh_connect_task_arg *)malloc(sizeof(bh_connect_task_arg));
//
//    connect_task_arg->module = module;
//    connect_task_arg->event = event;
//    connect_task_arg->server = server;
//    connect_task_arg->ip = ip;
//    connect_task_arg->port = port;
//    connect_task_arg->type = type;
//    connect_task_arg->server_connect_task = bh_server_client_connect;
//
//    return connect_task_arg;
//}
//
//void
//bh_connect_task_executer(bh_connect_task_arg *connect_task_arg) {
//    (*(connect_task_arg->server_connect_task))(connect_task_arg->module, connect_task_arg->event, connect_task_arg->server, connect_task_arg->type);
//}
//
//void
//bh_connect_task_terminator(bh_connect_task_arg *connect_task_arg) {
//    free(connect_task_arg);
//    connect_task_arg = NULL;
//}

struct bh_read_task_arg {
    bh_server_read_task server_read_task;
    bh_down_to_up_task down_to_up_task;
    bh_server_close_task server_close_task;
    bh_server *server;
    int sock_fd;
    bh_module *module;
    bh_event *event;
};

bh_read_task_arg *
bh_read_task_generator(bh_module *module, bh_event *event, bh_server *server, int sock_fd) {
    bh_read_task_arg *read_task_arg = (bh_read_task_arg *)malloc(sizeof(bh_read_task_arg));

    read_task_arg->module = module;
    read_task_arg->event = event;
    read_task_arg->server = server;
    read_task_arg->sock_fd = sock_fd;
    read_task_arg->server_read_task = bh_server_read;
    read_task_arg->down_to_up_task = down_to_up;
    read_task_arg->server_close_task = bh_server_client_close;

    return read_task_arg;
}

void
bh_read_task_executer(bh_read_task_arg *read_task_arg) {
    int res;

    res = (*(read_task_arg->server_read_task))(read_task_arg->server, read_task_arg->sock_fd);
    if (res == 1) {
        (*(read_task_arg->down_to_up_task))(read_task_arg->module, read_task_arg->server, read_task_arg->sock_fd);
    } else if (res == 0 || res == -1) {
        (*(read_task_arg->server_close_task))(read_task_arg->module, read_task_arg->event, read_task_arg->server, read_task_arg->sock_fd);
    }
}

void
bh_read_task_terminator(bh_read_task_arg *read_task_arg) {
    free(read_task_arg);
    read_task_arg = NULL;
}

struct bh_write_task_arg {
    bh_server_write_task server_write_task;
    bh_event_write_task event_write_task;
    bh_server_close_task server_close_task;
    bh_server *server;
    int sock_fd;
    bh_module *module;
    bh_event *event;
};

bh_write_task_arg *
bh_write_task_generator(bh_module *module, bh_event *event, bh_server *server, int sock_fd) {
    bh_write_task_arg *write_task_arg = (bh_write_task_arg *)malloc(sizeof(bh_write_task_arg));

    write_task_arg->module = module;
    write_task_arg->event = event;
    write_task_arg->server = server;
    write_task_arg->sock_fd = sock_fd;
    write_task_arg->server_write_task = bh_server_write;
    write_task_arg->event_write_task = bh_event_write;
    write_task_arg->server_close_task = bh_server_client_close;

    return write_task_arg;
}

void
bh_write_task_executer(bh_write_task_arg *write_task_arg) {
    int res;

    res = (*(write_task_arg->server_write_task))(write_task_arg->server, write_task_arg->sock_fd);
    if (res == 0) {
        (*(write_task_arg->event_write_task))(write_task_arg->event, write_task_arg->sock_fd, 0);
    } else if (res == -1){
        (*(write_task_arg->server_close_task))(write_task_arg->module, write_task_arg->event, write_task_arg->server, write_task_arg->sock_fd);
    }
}

void
bh_write_task_terminator(bh_write_task_arg *write_task_arg) {
    free(write_task_arg);
    write_task_arg = NULL;
}

struct bh_up_to_down_task_arg {
    bh_up_to_down_task up_to_down_task;
    bh_event *event;
    bh_server *server;
    int sock_fd;
    char *data;
    int len;
};

bh_up_to_down_task_arg *
bh_up_to_down_task_generator(bh_event *event, bh_server *server, int sock_fd, char *data, int len) {
    bh_up_to_down_task_arg *up_to_down_task_arg = (bh_up_to_down_task_arg *)malloc(sizeof(bh_up_to_down_task_arg));

    up_to_down_task_arg->event = event;
    up_to_down_task_arg->server = server;
    up_to_down_task_arg->sock_fd = sock_fd;
    up_to_down_task_arg->data = data;
    up_to_down_task_arg->len = len;
    up_to_down_task_arg->up_to_down_task = up_to_down;

    return up_to_down_task_arg;
}

void
bh_up_to_down_task_executer(bh_up_to_down_task_arg *up_to_down_task_arg) {
    (*(up_to_down_task_arg->up_to_down_task))(up_to_down_task_arg->event, up_to_down_task_arg->server,
            up_to_down_task_arg->sock_fd, up_to_down_task_arg->data, up_to_down_task_arg->len);
}

void
bh_up_to_down_task_terminator(bh_up_to_down_task_arg *up_to_down_task_arg) {
    free(up_to_down_task_arg);
    up_to_down_task_arg = NULL;
}

struct bh_close_task_arg {
    bh_server_close_task server_close_task;
    bh_module *module;
    bh_event *event;
    bh_server *server;
    int sock_fd;
};

bh_close_task_arg *
bh_close_task_generator(bh_module *module, bh_event *event, bh_server *server, int sock_fd) {
    bh_close_task_arg *close_task_arg = (bh_close_task_arg *)malloc(sizeof(bh_close_task_arg));

    close_task_arg->module = module;
    close_task_arg->event = event;
    close_task_arg->server = server;
    close_task_arg->sock_fd = sock_fd;
    close_task_arg->server_close_task = bh_server_client_close;

    return close_task_arg;
}

void
bh_close_task_executer(bh_close_task_arg *close_task_arg) {
    (*(close_task_arg->server_close_task))(close_task_arg->module, close_task_arg->event,
            close_task_arg->server, close_task_arg->sock_fd);
}

void
bh_close_task_terminator(bh_close_task_arg *close_task_arg) {
    free(close_task_arg);
    close_task_arg = NULL;
}

struct bh_task_arg {
    int task_type;
    void *task_arg;
};

bh_task_arg *
bh_task_generator(int task_type, void *task_arg) {
    bh_task_arg *arg = (bh_task_arg *)malloc(sizeof(bh_task_arg));

    arg->task_type = task_type;
    arg->task_arg = task_arg;

    return arg;
}

void
bh_task_executer(void *task_arg) {
    bh_task_arg *arg = (bh_task_arg *)task_arg;

    switch (arg->task_type) {
        case ACCEPT:
            bh_accept_task_executer((bh_accept_task_arg *)(arg->task_arg));
            break;
        case CONNECT:
            //bh_connect_task_executer((bh_connect_task_arg *)(arg->task_arg));
            break;
        case READ:
            bh_read_task_executer((bh_read_task_arg *)(arg->task_arg));
            break;
        case WRITE:
            bh_write_task_executer((bh_write_task_arg *)(arg->task_arg));
            break;
        case UP_TO_DOWN:
            bh_up_to_down_task_executer((bh_up_to_down_task_arg *)(arg->task_arg));
            break;
        case CLOSE:
            bh_close_task_executer((bh_close_task_arg *)(arg->task_arg));
            break;
        default:
            break;
    }
}

void
bh_task_terminator(void *task_arg) {
    bh_task_arg *arg = (bh_task_arg *)task_arg;

    switch (arg->task_type) {
        case ACCEPT:
            bh_accept_task_terminator((bh_accept_task_arg *)(arg->task_arg));
            break;
        case CONNECT:
            //bh_connect_task_terminator((bh_connect_task_arg *)(arg->task_arg));
            break;
        case READ:
            bh_read_task_terminator((bh_read_task_arg *)(arg->task_arg));
            break;
        case WRITE:
            bh_write_task_terminator((bh_write_task_arg *)(arg->task_arg));
            break;
        case UP_TO_DOWN:
            bh_up_to_down_task_terminator((bh_up_to_down_task_arg *)(arg->task_arg));
            break;
        case CLOSE:
            bh_close_task_terminator((bh_close_task_arg *)(arg->task_arg));
            break;
        default:
            break;
    }
    free(arg);
    arg = NULL;
}

static bh_server *server = NULL;

void
_bh_server_listen(bh_event *event, char *ip, int port) {
    server->sock_fd = bh_socket_create();
    server->ip = ip;
    server->port = port;
    printf("sock_fd: %d\n", server->sock_fd);
    printf("ip: %s\n", server->ip);
    printf("port: %d\n", server->port);
    bh_socket_nonblocking(server->sock_fd);
    bh_socket_bind(server->sock_fd, server->ip, server->port);
    bh_socket_listen(server->sock_fd, 1024);
    bh_event_add(event, server->sock_fd);
}

bh_server *
bh_server_create(bh_event *event, char *ip, int port) {
    int res;

    server = (bh_server *)malloc(sizeof(bh_server));
    bh_clients *clients = (bh_clients *)malloc(sizeof(bh_clients));
    clients->first = NULL;
    clients->last = NULL;
    clients->clients_count = 0;
    server->clients = clients;
    server->sock_fd = 0;
    server->ip = "";
    server->port = 0;
    res = pthread_mutex_init(&(server->clients_lock), NULL);
    if (res != 0) {
        printf("pthread_mutex_init failed\n");
        exit(0);
    }

    _bh_server_listen(event, ip, port);

    return server;
}

void
bh_server_release(bh_server *server) {
    free(server->clients);
    pthread_mutex_destroy(&(server->clients_lock));
    if (server->sock_fd != 0) {
        bh_socket_close(server->sock_fd);
    }
    free(server);
    server = NULL;
}

void
bh_server_client_accept(bh_module *module, bh_event *event, bh_server *server, char *type) {
    bh_client *client = (bh_client *)malloc(sizeof(bh_client));

    client->sock_fd = bh_socket_accept(server->sock_fd, &client->ip, &client->port);
    printf("new client ip: %s, port: %d, fd:%d\n", client->ip, client->port, client->sock_fd);
    bh_socket_nonblocking(client->sock_fd);
    client->recv_buffer = bh_buffer_create(1024, 8*1024);
    client->send_buffer = bh_buffer_create(1024, 8*1024);
    client->next = NULL;
    client->prev = NULL;
    client->type = type;

    pthread_mutex_lock(&(server->clients_lock));
    if (server->clients->first == NULL) {
        server->clients->first = client;
        server->clients->last = client;
    } else {
        server->clients->last->next = client;
        client->prev = server->clients->last;
        server->clients->last = client;
    }
    server->clients->clients_count += 1;
    pthread_mutex_unlock(&(server->clients_lock));

    bh_module_init(module, client->sock_fd);
    bh_event_add(event, client->sock_fd);
}

int
bh_server_client_connect(bh_module *module, bh_event *event, bh_server *server, const char *ip, int port, char *type) {
    bh_client *client = (bh_client *)malloc(sizeof(bh_client));

    client->sock_fd = bh_socket_create();
    client->ip = (char *)ip;
    client->port = port;
    bh_socket_nonblocking(client->sock_fd);
    bh_socket_connect(client->sock_fd, client->ip, client->port);
    client->recv_buffer = bh_buffer_create(1024, 8*1024);
    client->send_buffer = bh_buffer_create(1024, 8*1024);
    client->next = NULL;
    client->prev = NULL;
    client->type = type;

    pthread_mutex_lock(&(server->clients_lock));
    if (server->clients->first == NULL) {
        server->clients->first = client;
        server->clients->last = client;
    } else {
        server->clients->last->next = client;
        client->prev = server->clients->last;
        server->clients->last = client;
    }
    server->clients->clients_count += 1;
    pthread_mutex_unlock(&(server->clients_lock));

    bh_module_init(module, client->sock_fd);
    bh_event_add(event, client->sock_fd);

    return client->sock_fd;
}

static bh_client *
_find(bh_server *server, int sock_fd) {
    bh_client *client = server->clients->first;

    while (client) {
        if (client->sock_fd == sock_fd) break;
        client = client->next;
    }

    return client;
}

void
bh_server_client_close(bh_module *module, bh_event *event, bh_server *server, int sock_fd) {
    bh_client *client = NULL;

    pthread_mutex_lock(&(server->clients_lock));
    client = _find(server, sock_fd);
    pthread_mutex_unlock(&(server->clients_lock));

    if (client == NULL) return;
    printf("close client ip: %s, port: %d\n", client->ip, client->port);

    pthread_mutex_lock(&(server->clients_lock));
    if (client->prev==NULL && client->next==NULL) {
        server->clients->first = NULL;
        server->clients->last = NULL;
    } else if (client->prev==NULL && client->next!=NULL) {
        server->clients->first = client->next;
        client->next->prev = NULL;
    } else if (client->prev!=NULL && client->next==NULL) {
        server->clients->last = client->prev;
        client->prev->next = NULL;
    } else {
        client->prev->next = client->next;
        client->next->prev = client->prev;
    }
    server->clients->clients_count -= 1;
    pthread_mutex_unlock(&(server->clients_lock));

    bh_module_recv(module, sock_fd, "", 0, client->type);
    bh_event_del(event, sock_fd);
    bh_socket_close(client->sock_fd);
    bh_buffer_release(client->recv_buffer);
    bh_buffer_release(client->send_buffer);
    free(client);
    client = NULL;
}

/*
 * 1, socket normal
 * 0, socket close
 * -1, socket error
 */
int
bh_server_read(bh_server *server, int sock_fd) {
    bh_client *client = NULL;
    char *buffer = NULL;
    int res, size;

    pthread_mutex_lock(&(server->clients_lock));
    client = _find(server, sock_fd);
    pthread_mutex_unlock(&(server->clients_lock));

    if (client == NULL) return -1;
    while (1) {
        size = bh_buffer_get_write(client->recv_buffer, &buffer);
        res = bh_socket_recv(sock_fd, &buffer, size);
        if (res == -2) break;     // again
        if (res == -1) return -1; // error close
        if (res == 0) return 0;   // normal close
        bh_buffer_set_write(client->recv_buffer, res);
    }
    return 1;
}

/*
 * 0, data wirte done
 * 1, again
 * -1, error
 */
int
bh_server_write(bh_server *server, int sock_fd) {
    bh_client *client = NULL;
    char *buffer = NULL;
    int res, size;

    pthread_mutex_lock(&(server->clients_lock));
    client = _find(server, sock_fd);
    pthread_mutex_unlock(&(server->clients_lock));

    if (client == NULL) return -1;
    while (1) {
        size = bh_buffer_get_read(client->send_buffer, &buffer);
        if (size == 0) return 0;
        res = bh_socket_send(sock_fd, buffer, size);
        if (res == -2) break;
        if (res == -1) return -1;
        bh_buffer_set_read(client->send_buffer, res);
    }
    return 1;
}

void
up_to_down(bh_event *event, bh_server *server, int sock_fd, char *data, int len) {
    bh_client *client = NULL;
    char *buffer = NULL;
    int size, i, j = 0;

    pthread_mutex_lock(&(server->clients_lock));
    client = _find(server, sock_fd);
    pthread_mutex_unlock(&(server->clients_lock));

    if (client == NULL) return;
    while (1) {
        size = bh_buffer_get_write(client->send_buffer, &buffer);
        for (i=0; i<size && j<len; i++, j++) {
            buffer[i] = data[j];
        }
        bh_buffer_set_write(client->send_buffer, i);
        if (j == len) break;
    }
    bh_event_write(event, sock_fd, 1);
}

void
down_to_up(bh_module *module, bh_server *server, int sock_fd) {
    bh_client *client = NULL;
    char *buffer = NULL;
    int size;

    pthread_mutex_lock(&(server->clients_lock));
    client = _find(server, sock_fd);
    pthread_mutex_unlock(&(server->clients_lock));

    if (client == NULL) return;
    while (1) {
        size = bh_buffer_get_read(client->recv_buffer, &buffer);
        if (size == 0) break;
        bh_module_recv(module, sock_fd, buffer, size, client->type);
        bh_buffer_set_read(client->recv_buffer, size);
    }
}

void
bh_server_run(bh_thread_pool *thread_pool, bh_module *module, bh_event *event, bh_server *server, bh_timer *timer) {
    int events_num = 0, i, timeout;
    
    while (1) {
        timeout = bh_timer_get(timer);
        events_num = bh_event_poll(event, event->max_events, timeout);
        for (i=0; i<events_num; i++) {
            if (event->events[i].fd == server->sock_fd) {
                // accept new client
                bh_accept_task_arg *accept_task_arg = bh_accept_task_generator(module, event, server, "normal");
                bh_thread_pool_add_task(thread_pool, bh_task_executer, bh_task_generator(ACCEPT, (void *)accept_task_arg), -1, bh_task_terminator);
            } else {
                if (event->events[i].read) {
                    bh_read_task_arg *read_task_arg = bh_read_task_generator(module, event, server, event->events[i].fd);
                    bh_thread_pool_add_task(thread_pool, bh_task_executer, bh_task_generator(READ, (void *)read_task_arg), event->events[i].fd, bh_task_terminator);
                }

                if (event->events[i].write) {
                    bh_write_task_arg *write_task_arg = bh_write_task_generator(module, event, server, event->events[i].fd);
                    bh_thread_pool_add_task(thread_pool, bh_task_executer, bh_task_generator(WRITE, (void *)write_task_arg), event->events[i].fd, bh_task_terminator);
                }
            }
        }
        bh_timer_execute(module, timer);
    }
}
