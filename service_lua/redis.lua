package.cpath = package.cpath .. ";./lualib_c/?.so"

local server = require "bh_server"
local timer = require "bh_timer"

local redis = {}


function redis.connect(host, port)
    print(_G["bh"]["module"])
    print(_G["bh"]["event"])
    print(_G["bh"]["server"])
    print(host)
    print(port)
    return server.connect(_G["bh"]["module"], _G["bh"]["event"], _G["bh"]["server"], host, port)
end

function redis.disconnect(sock_fd)
    server.close(_G["bh"]["event"], _G["bh"]["server"], sock_fd)
end

function redis.set(sock_fd, key, value)
    message = "*3\r\n$3\r\nSET\r\n$" .. #key .. "\r\n" .. key .. "\r\n$" .. #value .. "\r\n" .. value .. "\r\n"
    server.send(_G["bh"]["server"], sock_fd, message, #message)
end

function redis.get(sock_fd, key)
    message = "*2\r\n$3\r\nGET\r\n$" .. #key .. "\r\n" .. key .. "\r\n"
    server.send(_G["bh"]["server"], sock_fd, message, #message)
end

function data_handler(sock_fd, data, len)
    print(data)
end

_G["bh"]["handler"] = data_handler

return redis
