#ifndef _BH_MODULE_H_
#define _BH_MODULE_H_

#include "bh_event.h"
#include "bh_server.h"
#include "bh_timer.h"
#include "bh_engine.h"

typedef struct bh_module bh_module;

void        bh_module_create();
void        bh_module_release();
//void        bh_module_load(int num, ...);
void        bh_module_load(const char *mod_name);
void        bh_module_init(int sock_fd);
void        bh_module_recv(int sock_fd, char *data);

void        bh_module_timeout_handler(char *handler_name);
void        bh_module_set_engine(bh_engine *engine);
void        bh_module_set_event(bh_event *event);
void        bh_module_set_server(bh_server *server);
void        bh_module_set_timer(bh_timer *timer);
#endif
