#ifndef _BH_ENGINE_H_
#define _BH_ENGINE_H_

typedef struct bh_engine bh_engine;

bh_engine * bh_engine_create(char *ip, int port, int num, ...);
void        bh_engine_start(bh_engine *engine);
void        bh_engine_stop(bh_engine *engine);

#endif
