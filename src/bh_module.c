#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdarg.h>

#include "bh_module.h"

struct bh_module {
    lua_State *L;
};

bh_module *
bh_module_create() {
    bh_module *module = (bh_module *)malloc(sizeof(bh_module));

    module->L = luaL_newstate();
    luaL_openlibs(module->L);
    
    return module;
}

void
bh_module_release(bh_module *module) {
    lua_close(module->L);
    free(module);
    module = NULL;
}

void
bh_module_load(bh_module *module, int num, ...) {
    va_list arg_ptr;
    char *mod_name;
    int i;

    va_start(arg_ptr, num);
    for (i=0; i<num; i++) {
        mod_name = va_arg(arg_ptr, char *);
        luaL_dofile(module->L, mod_name);
    }
    va_end(arg_ptr);
}

static void
_init(lua_State *L, bh_engine *engine, int sock_fd) {
    lua_getglobal(L, "init");
    lua_pushlightuserdata(L, engine);
    lua_pushinteger(L, sock_fd);
    lua_call(L, 1, 0);
}

void
bh_module_init(bh_module *module, bh_engine *engine, int sock_fd) {
    _init(module->L, engine, sock_fd);
}

static void
_recv(lua_State *L, int sock_fd, char *data) {
    lua_getglobal(L, "recv");
    lua_pushinteger(L, sock_fd);
    lua_pushstring(L, data);
    lua_call(L, 2, 0);
}

void
bh_module_recv(bh_module *module, int sock_fd, char *data) {
    _recv(module->L, engine, sock_fd, data);
}
