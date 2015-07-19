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

    engine = bh_engine_create("./examples/config.lua");
    bh_engine_start(engine);

    return 0;
}
