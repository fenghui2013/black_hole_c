package.cpath = package.cpath .. ";./lualib_c/?.so"
package.path = package.path .. ";./service_lua/?.lua"

local redis = require "redis"
local timer = require "bh_timer"

local sock_fd = redis.connect("127.0.0.1", 6379)

redis.set(sock_fd, "hello", "world")

function get_handler()
    redis.get(sock_fd, "hello")
end

_G["bh"]["timeout_handlers"]["get_handler"] = get_handler

timer.set(_G["bh"]["timer"], 1000, -1, "get_handler")
--redis.disconnect(sock_fd)
