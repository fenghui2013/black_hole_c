local db = {}

local command = {}

function command.GET(key)
    return db[key]
end

function command.SET(key, value)
    local last = db[key]
    db[key] = value
    return last
end

function data_handler(sock_fd, data, len)

end
