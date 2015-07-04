#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "bh_lua_module.h"
#include "bh_engine.h"
#include "bh_config.h"

int
main() {
    bh_engine *engine;

    engine = bh_engine_create("127.0.0.1", 8000, 3, 512, 9);
    bh_lua_module_load(engine->lua_module, "./test/test_handler.lua");
    bh_engine_start(engine);
    //bh_config *config = NULL;

    //config = bh_config_create();
    //bh_config_load(config, "./examples/config");

    //printf("ip: %s\n", bh_config_get_ip(config));
    //printf("port: %d\n", bh_config_get_port(config));
    //printf("threads: %d\n", bh_config_get_threads(config));
    //printf("lua_vms: %d\n", bh_config_get_lua_vms(config));

    //bh_config_release(config);
    return 0;
}
