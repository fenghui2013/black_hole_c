#ifndef _BH_MODULE_H_
#define _BH_MODULE_H_

typedef struct bh_module bh_module;

bh_module * bh_lua_module_create();
void        bh_lua_module_load(bh_module *module, char *mod_name);
void        bh_lua_module_init(bh_module *module, int sock_fd);
void        bh_lua_module_recv(bh_module *module, int sock_fd, char *data);

#endif
