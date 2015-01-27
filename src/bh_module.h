#ifndef _BH_MODULE_H_
#define _BH_MODULE_H_

#include "bh_engine.h"

typedef struct bh_module bh_module;

bh_module * bh_module_create();
void        bh_module_release(bh_module *module);
void        bh_module_load(bh_module *module, int num, ...);
void        bh_module_init(bh_module *module, bh_engine *engine, int sock_fd);
void        bh_module_recv(bh_module *module, int sock_fd, char *data);
#endif
