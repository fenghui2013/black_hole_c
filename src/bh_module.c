#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdarg.h>

#include "bh_module.h"

struct bh_module {
    lua_State *L;
};

bh_module *module = NULL;

void
bh_module_create() {
    if (module == NULL) {
        *module = (bh_module *)malloc(sizeof(bh_module));
        module->L = luaL_newstate();
        luaL_openlibs(module->L);
    }
    
}

void
bh_module_release() {
    lua_close(module->L);
    free(module);
    module = NULL;
}

void
bh_module_load(int num, ...) {
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
_set_engine(lua_State *L, bh_engine *engine) {
    lua_getglobal(L, "set_engine");
    lua_pushlightuserdata(L, (void *)engine);
    lua_call(L, 1, 0);
}

void
bh_module_set_engine(bh_engine *engine) {
    _set_engine(module->L, engine);
}

static void
_init(lua_State *L, int sock_fd) {
    lua_getglobal(L, "init");
    lua_pushinteger(L, sock_fd);
    lua_call(L, 1, 0);
}

void
bh_module_init(int sock_fd) {
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
bh_module_recv(int sock_fd, char *data) {
    _recv(module->L, engine, sock_fd, data);
}

_timeout_handler(lua_State *L, char *handler_name) {
    lua_getglobal(L, "timeout_handler");
    lua_pushstring(L, handler_name);
    lua_call(L, 1, 0);
}

void
bh_module_timeout_handler(char *handler_name) {
    _timeout_handler(module->L, handler_name);
}
