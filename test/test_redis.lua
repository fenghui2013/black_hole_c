package.cpath = package.cpath .. ";./lualib_c/?.so"
package.path = package.path .. ";./service_lua/?.lua"

local redis = require "redis"
local timer = require "bh_timer"

function set_callback1(data)
    print("set_callback1", data)
end

function set_callback2(data)
    print("set_callback2", data)
end

function set_callback3(data)
    print("set_callback3", data)
end

function get_callback1(data)
    print("get_callback1", data)
end

function get_callback2(data)
    print("get_callback2", data)
end

function get_callback3(data)
    print("get_callback3", data)
end

local sock_fd = redis.connect("127.0.0.1", 6379)
redis.set(sock_fd, "hello", "world", true, set_callback1)
redis.get(sock_fd, "hello", true, get_callback1)
redis.set(sock_fd, "hello", "world1", true, set_callback2)
redis.get(sock_fd, "hello", true, get_callback2)
redis.set(sock_fd, "hello", "world2", true, set_callback3)
redis.get(sock_fd, "hello", true, get_callback3)
--redis.disconnect(sock_fd)
