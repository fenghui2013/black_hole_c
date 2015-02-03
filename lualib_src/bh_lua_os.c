#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <unistd.h>

static int
_sleep(lua_State *L) {
    int sec = luaL_checkint(L, 1);

    sleep(sec);
    return 0;
}

int
luaopen_bh_os(lua_State *L) {
    luaL_Reg l[] = {
        {"sleep", _sleep},
        {NULL, NULL}
    };

    luaL_newlib(L, l);

    return 1;
}
