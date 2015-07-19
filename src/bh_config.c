#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bh_config.h"

struct bh_config {
    lua_State *L;
    const char *config_file;
    char *ip;
    int port;
    int threads;
    int lua_vms;
    char *server_type;
    char *lua_modules;
};

bh_config *
bh_config_create() {
    bh_config *config = (bh_config *)malloc(sizeof(bh_config));
    config->L = luaL_newstate();
    luaL_openlibs(config->L);
    config->config_file = NULL;
    config->ip = NULL;
    config->port = 0;
    config->threads = 0;
    config->lua_vms = 0;
    config->server_type = NULL;
    config->lua_modules = NULL;
    
    return config;
}

void
bh_config_release(bh_config *config) {
    free(config->ip);
    free(config->server_type);
    free(config->lua_modules);
    lua_close(config->L);
    free(config);
    config = NULL;
}

void
bh_config_load(bh_config *config, const char *config_file) {
    int res, i;
    const char *temp = NULL;

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
    lua_getglobal(config->L, "server_type");
    lua_getglobal(config->L, "lua_modules");

    config->config_file = config_file;

    temp = (char *)lua_tostring(config->L, 1);
    config->ip = (char *)malloc(strlen(temp)+1);
    memset(config->ip, 0, strlen(temp)+1);
    for (i=0; i<strlen(temp); i++) {
        config->ip[i] = temp[i];
    }


    config->port = luaL_checkint(config->L, 2);
    config->threads = luaL_checkint(config->L, 3);
    config->lua_vms = luaL_checkint(config->L, 4);

    temp = (char *)lua_tostring(config->L, 5);
    config->server_type = (char *)malloc(strlen(temp)+1);
    memset(config->server_type, 0, strlen(temp)+1);
    for (i=0; i<strlen(temp); i++) {
        config->server_type[i] = temp[i];
    }

    temp = (char *)lua_tostring(config->L, 6);
    config->lua_modules = (char *)malloc(strlen(temp)+1);
    memset(config->lua_modules, 0, strlen(temp)+1);
    for (i=0; i<strlen(temp); i++) {
        config->lua_modules[i] = temp[i];
    }

    lua_pop(config->L, 5);
}

const char *
bh_config_get_config_file(bh_config *config) {
    return config->config_file;
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

char *
bh_config_get_server_type(bh_config *config) {
    return config->server_type;
}

char *
bh_config_get_lua_modules(bh_config *config) {
    return config->lua_modules;
}
