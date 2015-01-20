#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <string.h>

#include "bh_string.h"

static int
_create(lua_State *L) {
    int size = luaL_checkint(L, 1);
    bh_string *string;
    
    string = bh_string_create(size);
    lua_pushlightuserdata(L, (void *)string);
    return 1;
}

static int
_release(lua_State *L) {
    bh_string *string = (bh_string *)lua_touserdata(L, 1);

    bh_string_release(string);
    return 0;
}

static int
_set(lua_State *L) {
    bh_string *string = (bh_string *)lua_touserdata(L, 1);
    char *s = (char *)luaL_checkstring(L, 2);
    int start = luaL_checkint(L, 3);
    int size = luaL_checkint(L, 4);
    int appending = luaL_checkint(L, 5);

    bh_string_set(string, s, start, size, appending);
    return 0;
}

static int
_update(lua_State *L) {
    bh_string *string = (bh_string *)lua_touserdata(L, 1);
    int len = luaL_checkint(L, 2);
    int res;

    res = bh_string_update(string, len);
    lua_pushinteger(L, res);
    return 1;
}

static int
_get(lua_State *L) {
    bh_string *string = (bh_string *)lua_touserdata(L, 1);
    char *s;

    s = bh_string_get(string);

    lua_pushstring(L, s);
    return 1;
}

static int
_get_size(lua_State *L) {
    bh_string *string = (bh_string *)lua_touserdata(L, 1);
    int size;

    size = bh_string_get_size(string);
    lua_pushinteger(L, size);
    return 1;
}

static int
_get_len(lua_State *L) {
    bh_string *string = (bh_string *)lua_touserdata(L, 1);
    int len;

    len = bh_string_get_len(string);
    lua_pushinteger(L, len);
    return 1;
}

static int
_get_free(lua_State *L) {
    bh_string *string = (bh_string *)lua_touserdata(L, 1);
    int free;

    free = bh_string_get_free(string);
    lua_pushinteger(L, free);
    return 1;
}

int
luaopen_bh_string(lua_State *L) {
    luaL_Reg l[] = {
        {"create", _create},
        {"release", _release},
        {"set", _set},
        {"update", _update},
        {"get", _get},
        {"get_size", _get_size},
        {"get_len", _get_len},
        {"get_free", _get_free},
        {NULL, NULL},
    };

    luaL_newlib(L, l);

    return 1;
}
