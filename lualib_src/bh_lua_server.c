#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>

#include "bh_server.h"
#include "bh_event.h"

static int
_connect(lua_State *L) {
    bh_event *event = (bh_event *)lua_touserdata(L, 1);
    bh_server *server = (bh_server *)lua_touserdata(L, 2);
    char *ip = (char *)luaL_checkstring(L, 3);
    int port = luaL_checkint(L, 4);

    int sock_fd = bh_server_client_connect(event, server, ip, port);

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
    
    printf("_send======================\n");
    up_to_down(event, server, sock_fd, data, len);

    return 0;
}

static int
_close(lua_State *L) {
    bh_event *event = (bh_event *)lua_touserdata(L, 1);
    bh_server *server = (bh_server *)lua_touserdata(L, 2);
    int sock_fd = luaL_checkint(L, 3);

    bh_server_client_close(event, server, sock_fd);

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
