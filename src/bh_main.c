#include "bh_lua_module.h"
#include "bh_engine.h"

int
main() {
    bh_engine *engine;

    engine = bh_engine_create("127.0.0.1", 8000, 3, 512, 9);
    bh_lua_module_load(engine->lua_module, "./test/test_handler.lua");
    //bh_module_load("./test/test_server.lua");
    //bh_module_load(engine->module, "./service_lua/redis.lua");
    //bh_module_load(engine->module, "./test/test_redis.lua");
    bh_engine_start(engine);
    return 0;
}
