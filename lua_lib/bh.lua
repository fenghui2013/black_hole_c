bh_module = {}

bh_module.engine = nil
bh_module.handler = nil
bh_module.timeout_handlers = {}

function set_engine(engine)
    if (not bh_module["engine"]) then
        bh_module["engine"] = engine
    end
end

function init(sock_fd)
    local co = coroutine.create(
        function(data)
            while true do
                if data == "" then
                    break
                end
                bh_module.handler(data)
                data = coroutine.yield()
            end
            bh_module[sock_fd] = nil
        end
    )
    bh_module[sock_fd] = co
end

function recv(sock_fd, data)
    coroutine.resume(bh_module[sock_fd], data)
end

function timeout_handler(handler_name)
    bh_module.timeout_handlers[handler_name]()
end

return bh_module
