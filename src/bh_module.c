#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "bh_event.h"
#include "bh_timer.h"
#include "bh_engine.h"
#include "bh_server.h"
#include "bh_module.h"

typedef struct bh_lua_state bh_lua_state;
struct bh_lua_state {
    int sock_fd;
    lua_State *L;
    bh_lua_state *prev;
    bh_lua_state *next;
};

struct bh_module {
    lua_State *L;
    bh_lua_state *first;
    bh_lua_state *last;
    pthread_mutex_t lua_lock;
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
    int res;

    bh_module *module = (bh_module *)malloc(sizeof(bh_module));
    module->L = luaL_newstate();
    luaL_openlibs(module->L);
    module->first = NULL;
    module->last = NULL;
    res = pthread_mutex_init(&(module->lua_lock), NULL);
    if (res != 0) {
        printf("pthread_mutex_init failed\n");
        exit(0);
    }
    return module;
}

void
bh_module_release(bh_module *module) {
    lua_close(module->L);
    pthread_mutex_destroy(&(module->lua_lock));
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
    luaL_loadfile(module->L, mod_name);
    printf("load module: %s\n", mod_name);
}

void
bh_module_call(bh_module *module) {
    int res;
    res = lua_pcall(module->L, 0, LUA_MULTRET, 0);
    printf("call res: %d\n", res);
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
_set_thread_pool(lua_State *L, bh_thread_pool *thread_pool) {
    lua_getglobal(L, "set_thread_pool");
    lua_pushlightuserdata(L, (void *)thread_pool);
    lua_call(L, 1, 0);
}

void
bh_module_set_thread_pool(bh_module *module, bh_thread_pool *thread_pool) {
    _set_thread_pool(module->L, thread_pool);
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

static bh_lua_state *
_find(bh_lua_state *lua_states, int sock_fd) {
    bh_lua_state *temp_lua_state = lua_states;

    while (temp_lua_state) {
        if (temp_lua_state->sock_fd == sock_fd) {
            break;
        }
        temp_lua_state = temp_lua_state->next;
    }
    return temp_lua_state;
}

static int
_recv(lua_State *L, int sock_fd, char *data, int len, char *type) {
    lua_getglobal(L, "recv");
    lua_pushinteger(L, sock_fd);
    lua_pushlstring(L, data, len);
    lua_pushinteger(L, len);
    lua_pushstring(L, type);
    //lua_call(L, 4, 0);
    return lua_resume(L, NULL, 4);
}

void
bh_module_recv(bh_module *module, int sock_fd, char *data, int len, char *type) {
    int res;
    bh_lua_state *temp_lua_state = NULL;
    lua_State *L = NULL;

    pthread_mutex_lock(&(module->lua_lock));
    temp_lua_state = _find(module->first, sock_fd);
    if (temp_lua_state == NULL) {
        L = lua_newthread(module->L);
        temp_lua_state = (bh_lua_state *)malloc(sizeof(bh_lua_state));
        temp_lua_state->L = L;
        temp_lua_state->sock_fd = sock_fd;
        temp_lua_state->prev = NULL;
        temp_lua_state->next = NULL;
        if (module->first == NULL) {
            module->first = temp_lua_state;
            module->last = temp_lua_state;
        } else {
            module->last->next = temp_lua_state;
            temp_lua_state->prev = module->last;
            module->last = temp_lua_state;
        }
    }
    L = temp_lua_state->L;
    pthread_mutex_unlock(&(module->lua_lock));

    res = _recv(L, sock_fd, data, len, type);
    
    pthread_mutex_lock(&(module->lua_lock));
    if (res == LUA_OK) {
        temp_lua_state = _find(module->first, sock_fd);
        if (temp_lua_state == NULL) {
            pthread_mutex_unlock(&(module->lua_lock));
            return;
        }
        if (temp_lua_state->prev==NULL && temp_lua_state->next==NULL) {
            module->first = NULL;
            module->last = NULL;
        } else if (temp_lua_state->prev==NULL && temp_lua_state->next!=NULL) {
            module->first = temp_lua_state->next;
            temp_lua_state->next->prev = NULL;
        } else if (temp_lua_state->prev!=NULL && temp_lua_state->next==NULL) {
            module->last = temp_lua_state->prev;
            temp_lua_state->prev->next = NULL;
        } else {
            temp_lua_state->prev->next = temp_lua_state->next;
            temp_lua_state->next->prev = temp_lua_state->prev;
        }
        free(temp_lua_state);
        temp_lua_state = NULL;
    } else if (res == LUA_YIELD) {
    } else {
        printf("lua error: %d\n", res);
    }
    pthread_mutex_unlock(&(module->lua_lock));
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
