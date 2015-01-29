#include "bh_engine.h"

int
main() {
    bh_engine *engine;

    engine = bh_engine_create();
    bh_engine_start(engine, "127.0.0.1", 8000);
    return 0;
}
