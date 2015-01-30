#include "bh_module.h"
#include "bh_engine.h"

int
main() {
    bh_engine *engine;

    engine = bh_engine_create();
    bh_module_load("./test/test_handler.lua");
    //bh_module_load("./test/test_server.lua");
    bh_engine_start(engine, "127.0.0.1", 8000);
    return 0;
}
