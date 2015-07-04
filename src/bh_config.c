#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>

#include "bh_config.h"

struct bh_config {
    lua_State *L;
    char *ip;
    int port;
    int threads;
    int lua_vms;
};

bh_config *
bh_config_create() {
    bh_config *config = (bh_config *)malloc(sizeof(bh_config));
    config->L = luaL_newstate();
    luaL_openlibs(config->L);
    config->ip = NULL;
    config->port = 0;
    config->threads = 0;
    config->lua_vms = 0;
    
    return config;
}

void
bh_config_release(bh_config *config) {
    lua_close(config->L);
    free(config);
    config = NULL;
}

void
bh_config_load(bh_config *config, const char *config_file) {
    int res;

    luaL_loadfile(config->L, config_file);
    res = lua_pcall(config->L, 0, LUA_MULTRET, 0);
    if (res != 0) {
        printf("bh_config_load failed == res: %d\n", res);
        exit(0);
    }
    lua_getglobal(config->L, "ip");
    lua_getglobal(config->L, "port");
    lua_getglobal(config->L, "threads");
    lua_getglobal(config->L, "lua_vms");

    config->ip = (char *)lua_tostring(config->L, 1);
    config->port = luaL_checkint(config->L, 2);
    config->threads = luaL_checkint(config->L, 3);
    config->lua_vms = luaL_checkint(config->L, 4);
    lua_pop(config->L, 4);
}

char *
bh_config_get_ip(bh_config *config) {
    return config->ip;
}

int
bh_config_get_port(bh_config *config) {
    return config->port;
}

int
bh_config_get_threads(bh_config *config) {
    return config->threads;
}

int
bh_config_get_lua_vms(bh_config *config) {
    return config->lua_vms;
}
