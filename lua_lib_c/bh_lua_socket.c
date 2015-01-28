#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

#include "bh_server.h"

static int
_connect(lua_State *L) {
    bh_event *event = (bh_event *)lua_touserdata(L, 1);
    bh_server *server = (bh_server *)lua_touserdata(L, 2);
    char *ip = (char *)luaL_checkstring(L, 3);
    int port = luaL_checkint(L, 4);

    bh_server_client_connect(event, server, ip, port);
    
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
luaopen_bh_socket(lua_State *L) {
    luaL_Reg l[] = {
        {"connect", _connect},
        {"close", _close},
        {NULL, NULL},
    };

    luaL_newlib(L, l);

    return 1;
}
