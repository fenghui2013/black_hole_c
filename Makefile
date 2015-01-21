BLACK_HOLE_BUILD_PATH ?= .

CC = gcc

LUA_INC := -I/usr/include/lua5.2
LUA_FLAGS := -llua5.2

CFLAGS := -O2 -Wall
SHARED := -fPIC --shared

LUA_CLIB_PATH ?= lualib_c
LUA_CLIB = bh_socket

all: \
	$(foreach v, $(LUA_CLIB), $(LUA_CLIB_PATH)/$(v).so)

$(LUA_CLIB_PATH):
	mkdir $(LUA_CLIB_PATH)

$(LUA_CLIB_PATH)/bh_socket.so: src/bh_socket.c src/bh_socket_lua.c | $(LUA_CLIB_PATH)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ $(LUA_INC) $(LUA_FLAGS)

clean:
	rm -rf $(LUA_CLIB_PATH)/*.so
cleanall:
	rm -rf $(LUA_CLIB_PATH)
