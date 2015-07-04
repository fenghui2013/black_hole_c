#ifndef _BH_SERVER_H_
#define _BH_SERVER_H_

typedef struct bh_thread_pool bh_thread_pool;
typedef struct bh_server bh_server;
typedef struct bh_event bh_event;
typedef struct bh_lua_module bh_lua_module;
typedef struct bh_timer bh_timer;

bh_server *  bh_server_create(bh_event *event, char *ip, int port);
void         bh_server_run(bh_thread_pool *thread_pool, bh_lua_module *lua_module, bh_event *event, bh_server *server, bh_timer *timer);
void         bh_server_release(bh_server *server);

enum {
    ACCEPT = 1,
    CONNECT = 2,
    READ = 3,
    WRITE = 4,
    UP_TO_DOWN = 5,
    CLOSE = 6
};

typedef void (*bh_server_accept_task)(bh_event *event, bh_server *server, char *type);
//typedef int  (*bh_server_connect_task)(bh_event *event, bh_server *server, const char *ip, int port, char *type);
typedef int  (*bh_server_read_task)(bh_server *server, int sock_fd);
typedef int  (*bh_server_write_task)(bh_server *server, int sock_fd);
typedef void (*bh_down_to_up_task)(bh_lua_module *lua_module, bh_server *server, int sock_fd);
//typedef void (*bh_up_to_down_task)(bh_event *event, bh_server *server, int sock_fd, char *data, int len, int data_ref);
typedef void (*bh_server_close_task)(bh_lua_module *lua_module, bh_event *event, bh_server *server, int sock_fd);

typedef struct bh_task_arg bh_task_arg;

typedef struct bh_accept_task_arg bh_accept_task_arg;
//typedef struct bh_connect_task_arg bh_connect_task_arg;
typedef struct bh_read_task_arg bh_read_task_arg;
typedef struct bh_write_task_arg bh_write_task_arg;
//typedef struct bh_up_to_down_task_arg bh_up_to_down_task_arg;
//typedef struct bh_close_task_arg bh_close_task_arg;

bh_task_arg *            bh_task_generator(int task_type, void *task_arg);
void                     bh_task_executer(void *task_arg);
void                     bh_task_terminator(void *task_arg);

bh_accept_task_arg *     bh_accept_task_generator(bh_event *event, bh_server *server, char *type);
void                     bh_accept_task_executer(bh_accept_task_arg *accept_task_arg);
void                     bh_accept_task_terminator(bh_accept_task_arg *accept_task_arg);
//bh_connect_task_arg *    bh_connect_task_generator(bh_event *event, bh_server *server, const char *ip, int port, char *type);
//void                     bh_connect_task_executer(bh_connect_task_arg *connect_task_arg);
//void                     bh_connect_task_terminator(bh_connect_task_arg *connect_task_arg);
bh_read_task_arg *       bh_read_task_generator(bh_lua_module *lua_module, bh_event *event, bh_server *server, int sock_fd);
void                     bh_read_task_executer(bh_read_task_arg *read_task_arg);
void                     bh_read_task_terminator(bh_read_task_arg *read_task_arg);
bh_write_task_arg *      bh_write_task_generator(bh_lua_module *lua_module, bh_event *event, bh_server *server, int sock_fd);
void                     bh_write_task_executer(bh_write_task_arg *write_task_arg);
void                     bh_write_task_terminator(bh_write_task_arg *write_task_arg);
//bh_up_to_down_task_arg * bh_up_to_down_task_generator(bh_event *event, bh_server *server, int sock_fd, char *data, int len, int data_ref);
//void                     bh_up_to_down_task_executer(bh_up_to_down_task_arg *up_to_down_task_arg);
//void                     bh_up_to_down_task_terminator(bh_up_to_down_task_arg *up_to_down_task_arg);
//bh_close_task_arg *      bh_close_task_generator(bh_lua_module *lua_module, bh_event *event, bh_server *server, int sock_fd);
//void                     bh_close_task_executer(bh_close_task_arg *close_task_arg);
//void                     bh_close_task_terminator(bh_close_task_arg *close_task_arg);


void         bh_server_client_accept(bh_event *event, bh_server *server, char *type);
int          bh_server_client_connect(bh_event *event, bh_server *server, const char *ip, int port, char *type);
void         bh_server_client_close(bh_lua_module *lua_module, bh_event *event, bh_server *server, int sock_fd);
int          bh_server_read(bh_server *server, int sock_fd);
int          bh_server_write(bh_server *server, int sock_fd);

void         up_to_down(bh_event *event, bh_server *server, int sock_fd, char *data, int len);
void         down_to_up(bh_lua_module *lua_module, bh_server *server, int sock_fd);
#endif
