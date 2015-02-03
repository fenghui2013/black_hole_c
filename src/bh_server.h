#ifndef _BH_SERVER_H_
#define _BH_SERVER_H_


typedef struct bh_server bh_server;
typedef struct bh_event bh_event;
typedef struct bh_module bh_module;
typedef struct bh_timer bh_timer;

bh_server * bh_server_create();
void        bh_server_release(bh_server *server);
void        bh_server_listen(bh_event *event, bh_server *server, char *ip, int port);
void        bh_server_client_accept(bh_module *module, bh_event *event, bh_server *server, char *type);
int         bh_server_client_connect(bh_module *module, bh_event *event, bh_server *server, const char *ip, int port, char *type);
void        bh_server_client_close(bh_module *module, bh_event *event, bh_server *server, int sock_fd);
int         bh_server_read(bh_server *server, int sock_fd);
int         bh_server_write(bh_server *server, int sock_fd);
void        bh_server_run(bh_module *module, bh_event *event, bh_server *server, bh_timer *timer);

void        up_to_down(bh_event *event, bh_server *server, int sock_fd, char *data, int len);
void        down_to_up(bh_module *module, bh_server *server, int sock_fd);
#endif
