bh_module = {}

bh_module.handler = nil

function init(sock_fd)
    local co = coroutine.create(
        function(data)
            while true do
                bh_module.handler(data)
                data = coroutine.yield()
            end
        end
    )
    bh_module[sock_fd] = co
end

function recv(sock_fd, data)
    coroutine.resume(bh_module[sock_fd], data)
end

return bh_module
