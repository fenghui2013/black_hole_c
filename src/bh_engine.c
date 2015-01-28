#include <stdlib.h>

#include "bh_event.h"
#include "bh_timer.h"
#include "bh_server.h"
#include "bh_module.h"
#include "bh_engine.h"

struct bh_engine {
    bh_event *event;
    bh_server *server;
    bh_timer *timer;
};

bh_engine *
bh_engine_create(int num, ...) {
    bh_engine *engine = (bh_engine *)malloc(sizeof(bh_engine));

    engine->event = bh_event_create();
    engine->server = bh_server_create();
    engine->timer = bh_timer_create();
    bh_module_create();
    bh_module_load(num, ...);
    bh_module_set_engine(engine);
    return engine;
}

void
bh_engine_start(bh_engine *engine, char *ip, int port) {
    bh_server_listen(engine->event, engine->server, ip, port);
    bh_server_run(engine->event, engine->server, engine->timer);
}

void
bh_engine_release(bh_engine *engine) {
    bh_module_release();
    bh_timer_release(engine->timer);
    bh_server_release(engine->server);
    bh_event_release(engine->event);
    free(engine);
    engine = NULL;
}
