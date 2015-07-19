#include <stdlib.h>
#include <string.h>

#include "bh_engine.h"
#include "bh_event.h"
#include "bh_timer.h"
#include "bh_lua_module.h"
#include "bh_server.h"
#include "bh_thread_pool.h"
#include "bh_config.h"

//struct bh_engine {
//    bh_module *module;
//    bh_event *event;
//    bh_server *server;
//    bh_timer *timer;
//};

bh_engine *
bh_engine_create(const char *config_file) {
    char *d = ";";
    char *p = NULL;
    bh_engine *engine = (bh_engine *)malloc(sizeof(bh_engine));

    engine->config = bh_config_create();
    bh_config_load(engine->config, config_file);
    engine->thread_pool = bh_thread_pool_create(bh_config_get_threads(engine->config), 512);
    engine->lua_module = bh_lua_module_create(bh_config_get_lua_vms(engine->config));
    engine->event = bh_event_create();
    engine->server = bh_server_create(engine->event, bh_config_get_ip(engine->config), bh_config_get_port(engine->config));
    engine->timer = bh_timer_create();
    //bh_module_load(num, ...);
    bh_lua_module_load(engine->lua_module, "./lualib/bh.lua");
    bh_lua_module_load(engine->lua_module, bh_config_get_config_file(engine->config));
    p = strtok(bh_config_get_lua_modules(engine->config), d);
    while (p) {
        bh_lua_module_load(engine->lua_module, p);
        p = strtok(NULL, d);
    }
    bh_lua_module_set_lua_module(engine->lua_module);
    bh_lua_module_set_engine(engine->lua_module, engine);
    bh_lua_module_set_event(engine->lua_module, engine->event);
    bh_lua_module_set_server(engine->lua_module, engine->server);
    bh_lua_module_set_timer(engine->lua_module, engine->timer);

    return engine;
}

void
bh_engine_start(bh_engine *engine) {
    if (!strcmp(bh_config_get_server_type(engine->config), "http")) {
        bh_lua_module_http_server_run(engine->lua_module);
    }
    bh_server_run(engine->thread_pool, engine->lua_module, engine->event, engine->server, engine->timer);
}

void
bh_engine_release(bh_engine *engine) {
    bh_thread_pool_release(engine->thread_pool);
    bh_timer_release(engine->timer);
    bh_server_release(engine->server);
    bh_event_release(engine->event);
    bh_lua_module_release(engine->lua_module);
    bh_config_release(engine->config);
    free(engine);
    engine = NULL;
}
