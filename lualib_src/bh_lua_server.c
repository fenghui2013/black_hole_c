#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>

#include "bh_server.h"
#include "bh_event.h"
#include "bh_lua_module.h"

static int
_connect(lua_State *L) {
    bh_event *event = (bh_event *)lua_touserdata(L, 1);
    bh_server *server = (bh_server *)lua_touserdata(L, 2);
    const char *ip = luaL_checkstring(L, 3);
    int port = luaL_checkint(L, 4);
    const char *type = luaL_checkstring(L, 5);

    int sock_fd = bh_server_client_connect(event, server, ip, port, (char *)type);

    lua_pushinteger(L, sock_fd);

    return 1;
}

static int
_send(lua_State *L) {
    bh_event *event = (bh_event *)lua_touserdata(L, 1);
    bh_server *server = (bh_server *)lua_touserdata(L, 2);
    int sock_fd = luaL_checkint(L, 3);
    char *data = (char *)luaL_checkstring(L, 4);
    int len = luaL_checkint(L, 5);
    
    up_to_down(event, server, sock_fd, data, len);
    //bh_up_to_down_task_arg *up_to_down_task_arg = bh_up_to_down_task_generator(event, server, sock_fd, data, len, data_ref);
    //bh_thread_pool_add_task(thread_pool, bh_task_executer, bh_task_generator(UP_TO_DOWN, (void *)up_to_down_task_arg), sock_fd, bh_task_terminator);

    return 0;
}

static int
_close(lua_State *L) {
    bh_lua_module *lua_module = (bh_lua_module *)lua_touserdata(L, 1);
    bh_event *event = (bh_event *)lua_touserdata(L, 2);
    bh_server *server = (bh_server *)lua_touserdata(L, 3);
    int sock_fd = luaL_checkint(L, 4);

    bh_server_client_close(lua_module, event, server, sock_fd);
    //bh_close_task_arg *close_task_arg = bh_close_task_generator(lua_module, event, server, sock_fd);
    //bh_thread_pool_add_task(thread_pool, bh_task_executer, bh_task_generator(CLOSE, (void *)close_task_arg), sock_fd, bh_task_terminator);

    return 0;
}

int
luaopen_bh_server(lua_State *L) {
    luaL_Reg l[] = {
        {"connect", _connect},
        {"send", _send},
        {"close", _close},
        {NULL, NULL},
    };

    luaL_newlib(L, l);

    return 1;
}
