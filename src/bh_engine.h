#ifndef _BH_ENGINE_H_
#define _BH_ENGINE_H_

typedef struct bh_engine bh_engine;
typedef struct bh_lua_module bh_lua_module;
typedef struct bh_event bh_event;
typedef struct bh_server bh_server;
typedef struct bh_timer bh_timer;
typedef struct bh_thread_pool bh_thread_pool;
typedef struct bh_config bh_config;

struct bh_engine {
    bh_thread_pool *thread_pool;
    bh_lua_module *lua_module;
    bh_event *event;
    bh_server *server;
    bh_timer *timer;
    bh_config *config;
};

//bh_engine * bh_engine_create(int num, ...);
bh_engine * bh_engine_create(const char *config_file);
void        bh_engine_start(bh_engine *engine);
void        bh_engine_release(bh_engine *engine);

#endif
