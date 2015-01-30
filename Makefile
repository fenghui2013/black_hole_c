BLACKHOLE_BUILD_PATH ?= .

CC = gcc

LUA_INC := -I/usr/include/lua5.2
LUA_FLAGS := -llua5.2

CFLAGS := -O2 -Wall
SHARED := -fPIC --shared

LUA_CLIB_PATH ?= lualib_c
LUA_CLIB = bh_server bh_timer

SRC = bh_main.c bh_buffer.c bh_engine.c bh_module.c bh_server.c bh_socket.c \
	  bh_string.c bh_timer.c

all: \
	$(BLACKHOLE_BUILD_PATH)/blackhole \
	$(foreach v, $(LUA_CLIB), $(LUA_CLIB_PATH)/$(v).so)

$(BLACKHOLE_BUILD_PATH)/blackhole: $(foreach v, $(SRC), src/$(v))
	$(CC) $(CFLAGS) -o $@ $^ -Isrc $(LUA_INC) $(LUA_FLAGS)

$(LUA_CLIB_PATH):
	mkdir $(LUA_CLIB_PATH)

$(LUA_CLIB_PATH)/bh_server.so: src/bh_buffer.c src/bh_string.c src/bh_socket.c src/bh_module.c src/bh_timer.c src/bh_server.c \
	lualib_src/bh_lua_server.c | $(LUA_CLIB_PATH)
	$(CC) $(CFLAGS) $(SHARED) -o $@ $^ -Isrc $(LUA_INC) $(LUA_FLAGS)

$(LUA_CLIB_PATH)/bh_timer.so: src/bh_module.c src/bh_timer.c \
	lualib_src/bh_lua_timer.c | $(LUA_CLIB_PATH)
	$(CC) $(CFLAGS) $(SHARED) -o $@ $^ -Isrc $(LUA_INC) $(LUA_FLAGS)

clean:
	rm -rf $(BLACKHOLE_BUILD_PATH)/blackhole $(LUA_CLIB_PATH)
