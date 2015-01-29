#ifndef _BH_SERVER_H_
#define _BH_SERVER_H_

#include "bh_event.h"
#include "bh_buffer.h"
#include "bh_socket.h"
#include "bh_engine.h"
#include "bh_timer.h"

typedef struct bh_server bh_server;

bh_server * bh_server_create();
void        bh_server_release(bh_server *server);
void        bh_server_listen(bh_event *event, bh_server *server, char *ip, int port);
void        bh_server_client_accept(bh_event *event, bh_server *server);
void        bh_server_client_connect(bh_event *event, bh_server *server, char *ip, int port);
void        bh_server_client_close(bh_event *event, bh_server *server, int sock_fd);
int         bh_server_read(bh_server *server, int sock_fd);
int         bh_server_write(bh_server *server, int sock_fd);
void        bh_server_run(bh_event *event, bh_server *server, bh_timer *timer);

void        up_to_down(bh_server *server, int sock_fd, char *data, int len);
void        down_to_up(bh_server *server, int sock_fd);
#endif
