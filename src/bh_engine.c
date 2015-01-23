#include "bh_buffer.h"
#include "bh_engine.h"
#include "bh_event.h"
#include "bh_timer.h"
#include "bh_socket.h"

typedef struct client_node client_node;
struct client_node {
    int sock_fd;
    char *ip;
    int port;
    bh_buffer *recv_buffer;
    bh_buffer *send_buffer;
    client_node *next;
};

typedef struct client_list client_list;
struct client_list {
    client_node *clients;
    int client_count;
};

struct bh_engine {
    int sock_fd;
};
