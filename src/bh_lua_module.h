#ifndef _BH_MODULE_H_
#define _BH_MODULE_H_

typedef struct bh_lua_module bh_lua_module;
typedef struct bh_engine bh_engine;
typedef struct bh_event bh_event;
typedef struct bh_server bh_server;
typedef struct bh_timer bh_timer;
typedef struct bh_thread_pool bh_thread_pool;

bh_lua_module * bh_lua_module_create(int lua_vm_count);
void            bh_lua_module_release(bh_lua_module *lua_module);
//void            bh_module_load(int num, ...);
void            bh_lua_module_load(bh_lua_module *lua_module, const char *mod_name);
int             bh_lua_module_recv(bh_lua_module *lua_module, int sock_fd, char *data, int len, char *type);
                
void            bh_lua_module_timeout_handler(bh_lua_module *lua_module, char *handler_name);
void            bh_lua_module_set_lua_module(bh_lua_module *lua_module);
void            bh_lua_module_set_engine(bh_lua_module *lua_module, bh_engine *engine);
void            bh_lua_module_set_event(bh_lua_module *lua_module, bh_event *event);
void            bh_lua_module_set_server(bh_lua_module *lua_module, bh_server *server);
void            bh_lua_module_set_timer(bh_lua_module *lua_module, bh_timer *timer);

void            bh_lua_module_http_server_run(bh_lua_module *lua_module);
#endif
