bh = {}
--bh.lua_module = nil
--bh.engine = nil
--bh.event = nil
--bh.server = nil
--bh.timer = nil
--bh.handler = nil
bh.timeout_handlers = {}

function set_engine(engine)
    if (not bh["engine"]) then
        bh["engine"] = engine
    end
end

function set_lua_module(lua_module)
    if (not bh["lua_module"]) then
        bh["lua_module"] = lua_module
    end
end

function set_event(event)
    if (not bh["event"]) then
        bh["event"] = event
    end
end

function set_server(server)
    if (not bh["server"]) then
        bh["server"] = server
    end
end

function set_timer(timer)
    if (not bh["timer"]) then
        bh["timer"] = timer
    end
end

function set_thread_pool(thread_pool)
    if (not bh["thread_pool"]) then
        bh["thread_pool"] = thread_pool
    end
end

function recv(sock_fd, data, len, type_name)
    while true do
        if data == "" then
            print("connect close")
            break;
        end
        bh[type_name .. "_handler"](sock_fd, data, len);
        fd, data, len, type_name = coroutine.yield()
    end
end

function timeout_handler(handler_name)
    bh.timeout_handlers[handler_name]()
end

_G["bh"] = bh
