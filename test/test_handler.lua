package.cpath = "lualib_c/?.so"

local server = require "bh_server"
local timer = require "bh_timer"

function data_handler(sock_fd, data, len)
    print(os.date(), data .. "\0")
    print(len)
    print(#data)
    server.send(_G["bh"]["event"], _G["bh"]["server"], sock_fd, data .. "\0", #data+1)
end

_G["bh"]["handler"] = data_handler
