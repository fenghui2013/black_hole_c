#ifndef _BH_ENGINE_H_
#define _BH_ENGINE_H_

typedef struct bh_engine bh_engine;
typedef struct bh_lua_module bh_lua_module;
typedef struct bh_event bh_event;
typedef struct bh_server bh_server;
typedef struct bh_timer bh_timer;
typedef struct bh_thread_pool bh_thread_pool;

struct bh_engine {
    bh_thread_pool *thread_pool;
    bh_lua_module *lua_module;
    bh_event *event;
    bh_server *server;
    bh_timer *timer;
};

//bh_engine * bh_engine_create(int num, ...);
bh_engine * bh_engine_create(char *ip, int port, int max_threads, int queue_size, int lua_vm_count);
void        bh_engine_start(bh_engine *engine);
void        bh_engine_release(bh_engine *engine);

#endif
