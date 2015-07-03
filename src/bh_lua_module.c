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
#include "bh_lua_module.h"

typedef struct bh_lua_vm bh_lua_vm;
struct bh_lua_vm {
    lua_State *vm;
    int vm_id;
    pthread_mutex_t vm_lock;
};

typedef struct bh_lua_state bh_lua_state;
struct bh_lua_state {
    int vm_id;
    lua_State *state;
    int sock_fd;
    bh_lua_state *prev;
    bh_lua_state *next;
};

struct bh_lua_module {
    bh_lua_vm *lua_vm;
    int current_vm;
    int vm_count;
    bh_lua_state *first;
    bh_lua_state *last;
    pthread_mutex_t lua_state_lock;
};

//bh_module *_module = NULL;

bh_lua_module *
bh_lua_module_create(int lua_vm_count) {
    int i, res;
    bh_lua_vm *temp_lua_vm = NULL;
    bh_lua_module *lua_module = (bh_lua_module *)malloc(sizeof(bh_lua_module));
    lua_module->first = NULL;
    lua_module->last = NULL;
    res = pthread_mutex_init(&(lua_module->lua_state_lock), NULL);
    if (res != 0) {
        printf("pthread_mutex_init failed\n");
        exit(0);
    }
    lua_module->lua_vm = (bh_lua_vm *)malloc(lua_vm_count*sizeof(bh_lua_vm));

    for (i=0; i<lua_vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        temp_lua_vm->vm = luaL_newstate();
        luaL_openlibs(temp_lua_vm->vm);
        temp_lua_vm->vm_id = i;
        res = pthread_mutex_init(&(temp_lua_vm->vm_lock), NULL);
        if (res != 0) {
            printf("pthread_mutex_init failed\n");
            exit(0);
        }
    }
    lua_module->current_vm = 0;
    lua_module->vm_count = lua_vm_count;

    return lua_module;
}

void
bh_lua_module_release(bh_lua_module *lua_module) {
    int i;
    bh_lua_vm *temp_lua_vm = NULL;

    for (i=0; i<lua_module->vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        lua_close(temp_lua_vm->vm);
        pthread_mutex_destroy(&(temp_lua_vm->vm_lock));
    }
    pthread_mutex_destroy(&(lua_module->lua_state_lock));
    free(lua_module->lua_vm);
    free(lua_module);
    lua_module = NULL;
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
bh_lua_module_load(bh_lua_module *lua_module, const char *mod_name) {
    int i, res;
    bh_lua_vm *temp_lua_vm = NULL;

    for (i=0; i<lua_module->vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        luaL_loadfile(temp_lua_vm->vm, mod_name);
        res = lua_pcall(temp_lua_vm->vm, 0, LUA_MULTRET, 0);
        printf("load module: %s, res: %d\n", mod_name, res);
    }
}

static void
_set_lua_module(lua_State *L, bh_lua_module *lua_module) {
    lua_getglobal(L, "set_lua_module");
    lua_pushlightuserdata(L, (void *)lua_module);
    lua_call(L, 1, 0);
}

void
bh_lua_module_set_lua_module(bh_lua_module *lua_module) {
    int i;
    bh_lua_vm *temp_lua_vm = NULL;

    for (i=0; i<lua_module->vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        _set_lua_module(temp_lua_vm->vm, lua_module);
    }
}

static void
_set_engine(lua_State *L, bh_engine *engine) {
    lua_getglobal(L, "set_engine");
    lua_pushlightuserdata(L, (void *)engine);
    lua_call(L, 1, 0);
}

void
bh_lua_module_set_engine(bh_lua_module *lua_module, bh_engine *engine) {
    int i;
    bh_lua_vm *temp_lua_vm = NULL;
    
    for (i=0; i<lua_module->vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        _set_engine(temp_lua_vm->vm, engine);
    }
}

static void
_set_event(lua_State *L, bh_event *event) {
    lua_getglobal(L, "set_event");
    lua_pushlightuserdata(L, (void *)event);
    lua_call(L, 1, 0);
}

void
bh_lua_module_set_event(bh_lua_module *lua_module, bh_event *event) {
    int i;
    bh_lua_vm *temp_lua_vm = NULL;

    for (i=0; i<lua_module->vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        _set_event(temp_lua_vm->vm, event);
    }
}

static void
_set_server(lua_State *L, bh_server *server) {
    lua_getglobal(L, "set_server");
    lua_pushlightuserdata(L, (void *)server);
    lua_call(L, 1, 0);
}

void
bh_lua_module_set_server(bh_lua_module *lua_module, bh_server *server) {
    int i;
    bh_lua_vm *temp_lua_vm = NULL;

    for (i=0; i<lua_module->vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        _set_server(temp_lua_vm->vm, server);
    }
}

static void
_set_timer(lua_State *L, bh_timer *timer) {
    lua_getglobal(L, "set_timer");
    lua_pushlightuserdata(L, (void *)timer);
    lua_call(L, 1, 0);
}

void
bh_lua_module_set_timer(bh_lua_module *lua_module, bh_timer *timer) {
    int i;
    bh_lua_vm *temp_lua_vm = NULL;

    for(i=0; i<lua_module->vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        _set_timer(temp_lua_vm->vm, timer);
    }
}

void
_timeout_handler(lua_State *L, char *handler_name) {
    lua_getglobal(L, "timeout_handler");
    lua_pushstring(L, handler_name);
    lua_call(L, 1, 0);
}

void
bh_lua_module_timeout_handler(bh_lua_module *lua_module, char *handler_name) {
    int i;
    bh_lua_vm *temp_lua_vm = NULL;

    for (i=0; i<lua_module->vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        _timeout_handler(temp_lua_vm->vm, handler_name);
    }
}

static void
_set_thread_pool(lua_State *L, bh_thread_pool *thread_pool) {
    lua_getglobal(L, "set_thread_pool");
    lua_pushlightuserdata(L, (void *)thread_pool);
    lua_call(L, 1, 0);
}

void
bh_lua_module_set_thread_pool(bh_lua_module *lua_module, bh_thread_pool *thread_pool) {
    int i;
    bh_lua_vm *temp_lua_vm = NULL;

    for (i=0; i<lua_module->vm_count; i++) {
        temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[i]);
        _set_thread_pool(temp_lua_vm->vm, thread_pool);
    }
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
bh_lua_module_recv(bh_lua_module *lua_module, int sock_fd, char *data, int len, char *type) {
    int res;
    bh_lua_state *temp_lua_state = NULL;
    bh_lua_vm *temp_lua_vm = NULL;
    lua_State *L = NULL;

    pthread_mutex_lock(&(lua_module->lua_state_lock));
    temp_lua_vm = (bh_lua_vm *)&(lua_module->lua_vm[lua_module->current_vm]);
    temp_lua_state = _find(lua_module->first, sock_fd);
    if (temp_lua_state == NULL) {
        L = lua_newthread(temp_lua_vm->vm);
        temp_lua_state = (bh_lua_state *)malloc(sizeof(bh_lua_state));
        temp_lua_state->state = L;
        temp_lua_state->sock_fd = sock_fd;
        temp_lua_state->vm_id = lua_module->current_vm;
        temp_lua_state->prev = NULL;
        temp_lua_state->next = NULL;
        if (lua_module->first == NULL) {
            lua_module->first = temp_lua_state;
            lua_module->last = temp_lua_state;
        } else {
            lua_module->last->next = temp_lua_state;
            temp_lua_state->prev = lua_module->last;
            lua_module->last = temp_lua_state;
        }
        lua_module->current_vm += 1;
        if (lua_module->current_vm == lua_module->vm_count) {
            lua_module->current_vm = 0;
        }
    }
    L = temp_lua_state->state;
    pthread_mutex_unlock(&(lua_module->lua_state_lock));

    pthread_mutex_lock(&(temp_lua_vm->vm_lock));
    res = _recv(L, sock_fd, data, len, type);
    pthread_mutex_unlock(&(temp_lua_vm->vm_lock));

    pthread_mutex_lock(&(lua_module->lua_state_lock));
    if (res == LUA_OK) {
        if (temp_lua_state->prev==NULL && temp_lua_state->next==NULL) {
            lua_module->first = NULL;
            lua_module->last = NULL;
        } else if (temp_lua_state->prev==NULL && temp_lua_state->next!=NULL) {
            lua_module->first = temp_lua_state->next;
            temp_lua_state->next->prev = NULL;
        } else if (temp_lua_state->prev!=NULL && temp_lua_state->next==NULL) {
            lua_module->last = temp_lua_state->prev;
            temp_lua_state->prev->next = NULL;
        } else {
            temp_lua_state->prev->next = temp_lua_state->next;
            temp_lua_state->next->prev = temp_lua_state->prev;
        }
        free(temp_lua_state);
        temp_lua_state = NULL;
    } else if (res == LUA_YIELD) {
    } else {
        printf("lua_error: %d\n", res);
    }
    pthread_mutex_unlock(&(lua_module->lua_state_lock));
}
