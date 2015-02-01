#ifndef _BH_MODULE_H_
#define _BH_MODULE_H_

typedef struct bh_module bh_module;
typedef struct bh_engine bh_engine;
typedef struct bh_event bh_event;
typedef struct bh_server bh_server;
typedef struct bh_timer bh_timer;

bh_module *        bh_module_create();
void               bh_module_release(bh_module *module);
//void               bh_module_load(int num, ...);
void               bh_module_load(bh_module *module, const char *mod_name);
void               bh_module_init(bh_module *module, int sock_fd);
void               bh_module_recv(bh_module *module, int sock_fd, char *data, int len);
                   
void               bh_module_timeout_handler(bh_module *module, char *handler_name);
void               bh_module_set_module(bh_module *module);
void               bh_module_set_engine(bh_module *module, bh_engine *engine);
void               bh_module_set_event(bh_module *module, bh_event *event);
void               bh_module_set_server(bh_module *module, bh_server *server);
void               bh_module_set_timer(bh_module *module, bh_timer *timer);
#endif
