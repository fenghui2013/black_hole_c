#ifndef _BH_MODULE_H_
#define _BH_MODULE_H_

#include "bh_engine.h"

void        bh_module_create();
void        bh_module_release();
void        bh_module_load(int num, ...);
void        bh_module_set_engine(bh_engine *engine);
void        bh_module_init(int sock_fd);
void        bh_module_recv(int sock_fd, char *data);
void        bh_module_timeout_handler(char *handler_name);
#endif
