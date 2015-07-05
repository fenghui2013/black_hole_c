package.cpath = package.cpath .. ";./lualib_c/?.so"

local server = require "bh_server"
local timer = require "bh_timer"

function data_handler(sock_fd, data, len)
    server.send(bh_get_event(), bh_get_server(), sock_fd, data, len)
end

bh_run("normal", data_handler)
