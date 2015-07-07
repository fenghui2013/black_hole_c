#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "bh_lua_module.h"
#include "bh_engine.h"
#include "bh_config.h"

int
main() {
    bh_engine *engine;
    bh_config *config = NULL;
    char *lua_modules = NULL;
    char *d = ";";
    char *p = NULL;

    config = bh_config_create();
    bh_config_load(config, "./examples/config");

    engine = bh_engine_create(bh_config_get_ip(config), bh_config_get_port(config),
            bh_config_get_threads(config), 512, bh_config_get_lua_vms(config));
    lua_modules = bh_config_get_lua_modules(config);
    
    p = strtok(lua_modules, d);
    while (p) {
        bh_lua_module_load(engine->lua_module, p);
        p = strtok(NULL, d);
    }
    bh_config_release(config);

    bh_engine_start(engine);

    return 0;
}
