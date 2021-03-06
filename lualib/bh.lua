bh = {}
--bh.module = nil
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

function set_module(module)
    if (not bh["module"]) then
        bh["module"] = module
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

function init(sock_fd)
    local co = coroutine.create(
        function(fd, data, len, type_name)
            while true do
                if data == "" then
                    break
                end
                bh[type_name .. "_handler"](fd, data, len)
                fd, data, len, type_name = coroutine.yield()
            end
            bh[sock_fd] = nil
        end
    )
    bh[sock_fd] = co
end

function recv(sock_fd, data, len, type_name)
    coroutine.resume(bh[sock_fd], sock_fd, data, len, type_name)
end

function timeout_handler(handler_name)
    bh.timeout_handlers[handler_name]()
end

_G["bh"] = bh
