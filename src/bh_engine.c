#include <stdlib.h>

#include "bh_buffer.h"
#include "bh_event.h"
#include "bh_timer.h"
#include "bh_socket.h"
#include "bh_module.h"
#include "bh_engine.h"

typedef struct client_node client_node;
struct client_node {
    int sock_fd;  // client socket fd
    char *ip;
    int port;
    bh_buffer *recv_buffer;
    bh_buffer *send_buffer;
    client_node *next;
};

typedef struct client_list client_list;
struct client_list {
    client_node *first;
    client_node *last;
    int client_count;
};

struct bh_engine {
    int event_fd;
    bh_event *events_buffer;
    int max_event;
    int sock_fd; // listen socket fd
    char *ip;
    int port;
    client_list *list;
};

bh_engine *
bh_engine_create() {
    bh_engine *engine = (bh_engine *)malloc(sizeof(bh_engine));

    engine->event_fd = bh_event_create();
    engine->max_event = 256;
    engine->events_buffer = (bh_event *)malloc(256*sizeof(bh_event));
    engine->sock_fd = bh_socket_create();
    engine->list = (client_list *)malloc(sizeof(client_list));
    engine->list->first = NULL;
    engine->list->last = NULL;
    engine->list->client_count = 0;

    return engine;
}

static void
_init_server(bh_engine *engine) {
    bh_socket_bind(engine->sock_fd, engine->ip, engine->port);
    bh_socket_nonblocking(engine->sock_fd);
    bh_socket_listen(engine->sock_fd, 1024);
    bh_event_add(engine->event_fd, engine->sock_fd);
}

static void
_open_client(bh_engine *engine) {
    client_node *client = (client_node *)malloc(sizeof(client_node));

    client->sock_fd = bh_socket_accept(engine->sock_fd, &client->ip, &client->port);
    client->recv_buffer = bh_buffer_create(1024, 8*1024);
    client->send_buffer = bh_buffer_create(1024, 8*1024);
    client->next = NULL;
    if (engine->list->first == NULL) {
        engine->list->first = client;
        engine->list->last = client;
        engine->list->client_count += 1;
    } else {
        engine->list->last->next = client;
        engine->list->last = client;
        engine->list->client_count += 1;
    }
    bh_event_add(engine->event_fd, client->sock_fd);
}

static client_node *
_find_prev(bh_engine *engine, int sock_fd) {
    client_node *node = engine->list->first;
    client_node *prev = NULL;

    while (node) {
        if (node->sock_fd == sock_fd) break;
        prev = node;
        node = node->next;
    }
    return prev;
}

static void
_close_client(bh_engine *engine, int sock_fd) {
    client_node *prev = _find_prev(engine, sock_fd), *temp;

    if (prev == NULL) {
        free(engine->list->first);
        engine->list->first = NULL;
        engine->list->last = NULL;
        engine->list->client_count -= 1;
    } else if (prev->next->next == NULL) {
        temp = prev->next;
        prev->next = temp->next;
        free(temp);
        engine->list->last = prev;
    } else if (prev->next == NULL) {
        temp = prev->next;
        prev
    }
}

static client_node *
_find(bh_engine *engine, int sock_fd) {
    client_node *node = engine->list->first;
    
    while (node) {
        if (node->sock_fd == sock_fd) break;
        node = node->next;
    }

    return node;
}

/*
 * 1, socket normal
 * 0, socket close
 * -1, socket error
 */
static int 
_read(bh_engine *engine, int sock_fd) {
    client_node *node = _find(engine, sock_fd);
    char *buffer;
    int size;
    int res;

    while (1) {
        size = bh_buffer_get_write(node->recv_buffer, buffer);
        res = bh_socket_recv(sock_fd, &buffer, size);
        switch (res) {
            case -2:
                // again
                continue;
            case -1:
                // error close
                return -1;
            case 0:
                // normal close
                return 0;
            default:
                bh_buffer_set_write(node->recv_buffer, res);
                break;
        }
    }
    return 1;
}

static void
_write(bh_engine *engine, int sock_fd) {
}

void
bh_engine_start(bh_engine *engine) {
    int events_num = 0, i, res;

    _init_server(engine);
    while (1) {
        events_num = bh_event_poll(engine->event_fd, engine->events_buffer, engine->max_event);
        for (i=0; i<events_num; i++) {
            if (engine->event_buffer[i].fd == engine->sock_fd) {
                // accept new client
                _open_client(engine);
            } else {
                // read or write
                if (engine->events_buffer[i].read) {
                    res = _read(engine, engine->events_buffer[i].fd);
                    if (res == 0 || res == -1) {
                        _close_client(engine, engine->events_buffer[i].fd);
                    }
                }

                if (engine->events_buffer[i].write) {
                    _write(engine, engine->events_buffer[i].fd);
                }
            }
        }
    }
}

void
bh_engine_stop(bh_engine *engine) {
}
