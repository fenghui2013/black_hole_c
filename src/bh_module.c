#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>

#include "bh_module.h"

struct bh_module {
    lua_State *L;
};

bh_module *
bh_lua_module_create() {
    bh_module *module = (bh_module *)malloc(sizeof(bh_module));

    module->L = luaL_newstate();
    luaL_openlibs(module->L);
    
    return module;
}

void
bh_lua_module_load(bh_module *module, char *mod_name) {
    luaL_dofile(module->L, mod_name);
}

static void
_init(lua_State *L, int sock_fd) {
    lua_getglobal(L, "init");
    lua_pushinteger(L, sock_fd);
    lua_call(L, 1, 0);
}

void
bh_lua_module_init(bh_module *module, int sock_fd) {
    _init(module->L, sock_fd);
}

static void
_recv(lua_State *L, int sock_fd, char *data) {
    lua_getglobal(L, "recv");
    lua_pushinteger(L, sock_fd);
    lua_pushstring(L, data);
    lua_call(L, 2, 0);
}

void
bh_lua_module_recv(bh_module *module, int sock_fd, char *data) {
    _recv(module->L, sock_fd, data);
}
