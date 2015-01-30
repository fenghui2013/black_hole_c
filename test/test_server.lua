package.cpath = "lualib_c/?.so"

local server = require "bh_server"
local timer = require "bh_timer"

local sock_fd = server.connect(_G["bh"]["event"], _G["bh"]["server"], "127.0.0.1", 8000)

function send_data()
    server.send(_G["bh"]["server"], sock_fd, "hello world", 11)
end

function close_handler()
    server.close(_G["bh"]["event"], _G["bh"]["server"], sock_fd)
end

_G["bh"]["timeout_handlers"]["send_data"] = send_data
_G["bh"]["timeout_handlers"]["close_handler"] = close_handler 


timer.set(_G["bh"]["timer"], 1000, 10, "send_data")
timer.set(_G["bh"]["timer"], 15000, 1, "close_handler")
