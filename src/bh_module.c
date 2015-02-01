#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>

#include "bh_event.h"
#include "bh_timer.h"
#include "bh_engine.h"
#include "bh_server.h"
#include "bh_module.h"

struct bh_module {
    lua_State *L;
};

//bh_module *_module = NULL;

bh_module *
bh_module_create() {
    //if (_module == NULL) {
    //    _module = (bh_module *)malloc(sizeof(bh_module));
    //    _module->L = luaL_newstate();
    //    luaL_openlibs(_module->L);
    //}
    //return _module;
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

//void
//bh_module_load(int num, ...) {
//    va_list arg_ptr;
//    char *mod_name;
//    int i;
//
//    va_start(arg_ptr, num);
//    for (i=0; i<num; i++) {
//        mod_name = va_arg(arg_ptr, char *);
//        luaL_dofile(module->L, mod_name);
//    }
//    va_end(arg_ptr);
//}

void
bh_module_load(bh_module *module, const char *mod_name) {
    int res;
    //luaL_dofile(module->L, mod_name);
    luaL_loadfile(module->L, mod_name);
    res = lua_pcall(module->L, 0, LUA_MULTRET, 0);
    printf("load module: %s, res: %d\n", mod_name, res);
}

static void
_set_module(lua_State *L, bh_module *module) {
    lua_getglobal(L, "set_module");
    lua_pushlightuserdata(L, (void *)module);
    lua_call(L, 1, 0);
}

void
bh_module_set_module(bh_module *module) {
    _set_module(module->L, module);
}

static void
_set_engine(lua_State *L, bh_engine *engine) {
    lua_getglobal(L, "set_engine");
    lua_pushlightuserdata(L, (void *)engine);
    lua_call(L, 1, 0);
}

void
bh_module_set_engine(bh_module *module, bh_engine *engine) {
    _set_engine(module->L, engine);
}

static void
_set_event(lua_State *L, bh_event *event) {
    lua_getglobal(L, "set_event");
    lua_pushlightuserdata(L, (void *)event);
    lua_call(L, 1, 0);
}

void
bh_module_set_event(bh_module *module, bh_event *event) {
    _set_event(module->L, event);
}

static void
_set_server(lua_State *L, bh_server *server) {
    lua_getglobal(L, "set_server");
    lua_pushlightuserdata(L, (void *)server);
    lua_call(L, 1, 0);
}

void
bh_module_set_server(bh_module *module, bh_server *server) {
    _set_server(module->L, server);
}

static void
_set_timer(lua_State *L, bh_timer *timer) {
    lua_getglobal(L, "set_timer");
    lua_pushlightuserdata(L, (void *)timer);
    lua_call(L, 1, 0);
}

void
bh_module_set_timer(bh_module *module, bh_timer *timer) {
    _set_timer(module->L, timer);
}

static void
_bh_module_init(lua_State *L, int sock_fd) {
    lua_getglobal(L, "init");
    lua_pushinteger(L, sock_fd);
    lua_call(L, 1, 0);
}

void
bh_module_init(bh_module *module, int sock_fd) {
    _bh_module_init(module->L, sock_fd);
}

static void
_recv(lua_State *L, int sock_fd, char *data, int len) {
    lua_getglobal(L, "recv");
    lua_pushinteger(L, sock_fd);
    lua_pushlstring(L, data, len);
    lua_pushinteger(L, len);
    lua_call(L, 3, 0);
}

void
bh_module_recv(bh_module *module, int sock_fd, char *data, int len) {
    _recv(module->L, sock_fd, data, len);
}

void
_timeout_handler(lua_State *L, char *handler_name) {
    lua_getglobal(L, "timeout_handler");
    lua_pushstring(L, handler_name);
    lua_call(L, 1, 0);
}

void
bh_module_timeout_handler(bh_module *module, char *handler_name) {
    _timeout_handler(module->L, handler_name);
}
