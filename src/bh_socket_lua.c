#include <lua.h>
#include <lauxlib.h>

#include "bh_socket.h"

static int
_create(lua_State *L) {
    int sock_fd;

    sock_fd = bh_socket_create();
    
    lua_pushinteger(L, sock_fd);
    return 1;
}

static int
_close(lua_State *L) {
    int sock_fd = luaL_checkint(L, 1);

    bh_socket_close(sock_fd);

    return 0;
}

static int
_nonblocking(lua_State *L) {
    int sock_fd = luaL_checkint(L, 1);

    bh_socket_nonblocking(sock_fd);

    return 0;
}

static int
_tcpnodelay(lua_State *L) {
    int sock_fd = luaL_checkint(L, 1);

    bh_socket_tcpnodelay(sock_fd);

    return 0;
}

static int
_bind(lua_State *L) {
    int sock_fd = luaL_checkint(L, 1);
    const char *ip = luaL_checkstring(L, 2);
    int port = luaL_checkint(L, 3);

    bh_socket_bind(sock_fd, ip, port);
    
    return 0;
}

static int
_listen(lua_State *L) {
    int sock_fd = luaL_checkint(L, 1);
    int backlog = luaL_checkint(L, 2);

    bh_socket_listen(sock_fd, backlog);

    return 0;
}

static int
_accept(lua_State *L) {
    int sock_fd = luaL_checkint(L, 1);
    char *ip;
    int port;
    int new_fd;

    new_fd = bh_socket_accept(sock_fd, ip, &port);
    if (new_fd > 0)
        goto normal;
    if (new_fd == 0)
        goto again;
    if (new_fd == -1)
        goto error;
normal:
    lua_pushinteger(L, new_fd);
    lua_pushstring(L, ip);
    lua_pushinteger(L, port);
    return 3;
again:
    lua_pushinteger(L, new_fd);
    lua_pushliteral(L, "again");
    lua_pushinteger(L, 0);
    return 3;
error:
    lua_pushinteger(L, new_fd);
    lua_pushliteral(L, "error");
    lua_pushinteger(L, 0);
    return 3;
}

static int
_connect(lua_State *L) {
    int sock_fd = luaL_checkint(L, 1);
    const char *ip = luaL_checkstring(L, 2);
    int port = luaL_checkint(L, 3);

    bh_socket_connect(sock_fd, ip, port);
    
    return 0;    
}

static int
_recv(lua_State *L) {
    int sock_fd = luaL_checkint(L, 1);
    int size = luaL_checkint(L, 2);
    char *buffer = (char *)malloc(size*sizeof(char));
    int recv_size;

    recv_size = bh_socket_recv(sock_fd, buffer, size);
    if (recv_size >= 0)
        goto normal;
    if (recv_size == -1)
        goto error;
    if (recv_size == -2)
        goto again;
normal:
    lua_pushlstring(L, buffer, recv_size);
    lua_pushinteger(L, recv_size);
    return 2;
error:
    lua_pushliteral(L, "error");
    lua_pushinteger(L, recv_size);
    return 2;
again:
    lua_pushliteral(L, "again");
    lua_pushinteger(L, recv_size);
    return 2;
}

static int
_send(lua_State *L) {
    int sock_fd = luaL_checkint(L, 1);
    const char *buffer = luaL_checkstring(L, 2);
    int size = luaL_checkint(L, 3);
    int send_size;

    send_size = bh_socket_send(sock_fd, buffer, size);
    if (send_size >= 0)
        goto normal;
    if (send_size == -1)
        goto error;
    if (send_size == -2)
        goto again;
normal:
    lua_pushliteral(L, "normal");
    lua_pushinteger(L, send_size);
    return 2;
error:
    lua_pushliteral(L, "error");
    lua_pushinteger(L, send_size);
    return 2;
again:
    lua_pushliteral(L, "again");
    lua_pushinteger(L, send_size);
    return 2;
}

int
luaopen_socket(lua_State *L) {
    luaL_Reg l[] = {
        {"create", _create},
        {"close", _close},
        {"nonblocking", _nonblocking},
        {"tcpnodelay", _tcpnodelay},
        {"bind", _bind},
        {"listen", _listen},
        {"accept", _accept},
        {"connect", _connect},
        {"recv", _recv},
        {"send", _send},
        {NULL, NULL},
    };

    luaL_newlib(L, l);

    return 1;
}
