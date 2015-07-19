package.cpath = package.cpath .. ";./lualib_c/?.so"

bh = {}
--bh.lua_module = nil
--bh.engine = nil
--bh.event = nil
--bh.server = nil
--bh.timer = nil
--bh.handler = nil
bh.timeout_handlers = {}

local server = require "bh_server"

function set_engine(engine)
    if (not bh["engine"]) then
        bh["engine"] = engine
    end
end

function bh_get_engine()
    return bh["engine"]
end

function set_lua_module(lua_module)
    if (not bh["lua_module"]) then
        bh["lua_module"] = lua_module
    end
end

function bh_get_lua_module()
    return bh["lua_module"]
end

function set_event(event)
    if (not bh["event"]) then
        bh["event"] = event
    end
end

function bh_get_event()
    return bh["event"]
end

function set_server(server)
    if (not bh["server"]) then
        bh["server"] = server
    end
end

function bh_get_server()
    return bh["server"]
end

function set_timer(timer)
    if (not bh["timer"]) then
        bh["timer"] = timer
    end
end

function bh_get_timer()
    return bh["timer"]
end

function recv(sock_fd, data, len, type_name)
    while true do
        local left_len = bh[type_name .. "_handler"](sock_fd, data, len)
        if data == "" then
            break;
        end
        sock_fd, data, len, type_name = coroutine.yield(left_len)
    end
    return 0
end

function timeout_handler(handler_name)
    bh.timeout_handlers[handler_name]()
end

function bh_write(sock_fd, data, len)
    server.send(bh_get_event(), bh_get_server(), sock_fd, data, len)
end

function bh_run(module_name, run_fun)
    bh[module_name .. "_handler"] = run_fun
end

function bh_http_server_run()
    for key, value in pairs(http) do
        server.listen(bh_get_event(), value["ip"], value["port"])
    end
end
