package.path = package.path .. ";./service_lua/?.lua"

local redis = require "redis"

local sock_fd = redis.connect("127.0.0.1", 6379)

--redis.set(sock_fd, "hello", "world")
--redis.get(sock_fd, "hello")
redis.disconnect()
