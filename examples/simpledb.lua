local command = {}

function command.GET(key)
    return db[key]
end

function command.SET(key, value)
    local last = db[key]
    db[key] = value
    return last
end

function protocol_parser(data, len)
end

function data_handler(sock_fd, data, len)
end

bh_run("simpledb", data_handler)
