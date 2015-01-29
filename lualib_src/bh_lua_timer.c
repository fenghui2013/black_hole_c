#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "bh_timer.h"

static int
_set(lua_State *L) {
    bh_timer *timer = (bh_timer *)lua_touserdata(L, 1);
    int time = luaL_checkint(L, 2);
    int times = luaL_checkint(L, 3);
    char *handler_name = (char *)luaL_checkstring(L, 4);
    
    bh_timer_set(timer, time, times, handler_name);

    return 0;
}

int
luaopen_bh_timer(lua_State *L) {
    luaL_Reg l[] = {
        {"set", _set},
        {NULL, NULL},
    };

    luaL_newlib(L, l);

    return 1;
}
