#include <stdlib.h>

#include "bh_engine.h"
#include "bh_event.h"
#include "bh_timer.h"
#include "bh_module.h"
#include "bh_server.h"

//struct bh_engine {
//    bh_module *module;
//    bh_event *event;
//    bh_server *server;
//    bh_timer *timer;
//};

bh_engine *
bh_engine_create(char *ip, int port) {
    bh_engine *engine = (bh_engine *)malloc(sizeof(bh_engine));

    engine->module = bh_module_create();
    engine->event = bh_event_create();
    engine->server = bh_server_create();
    engine->timer = bh_timer_create();
    //bh_module_load(num, ...);
    bh_module_load(engine->module, "./lualib/bh.lua");
    bh_module_call(engine->module);
    bh_module_set_module(engine->module);
    bh_module_set_engine(engine->module, engine);
    bh_module_set_event(engine->module, engine->event);
    bh_module_set_server(engine->module, engine->server);
    bh_module_set_timer(engine->module, engine->timer);
    bh_server_listen(engine->event, engine->server, ip, port);
    return engine;
}

void
bh_engine_start(bh_engine *engine) {
    bh_server_run(engine->module, engine->event, engine->server, engine->timer);
}

void
bh_engine_release(bh_engine *engine) {
    bh_timer_release(engine->timer);
    bh_server_release(engine->server);
    bh_event_release(engine->event);
    bh_module_release(engine->module);
    free(engine);
    engine = NULL;
}
