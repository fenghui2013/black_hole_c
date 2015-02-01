package.cpath = "lualib_c/?.so" .. package.cpath

local server = require "bh_server"
local timer = require "bh_timer"

count = 0
function data_handler(sock_fd, data, len)
    count = count + #data
    server.send(_G["bh"]["event"], _G["bh"]["server"], sock_fd, data, #data)
end

_G["bh"]["handler"] = data_handler
