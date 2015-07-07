local db = {}
local commands = {}

function commands.GET(key)
    return db[key]
end

function commands.SET(key, value)
    local last = db[key]
    db[key] = value
    return last
end

function commands.ERROR()
    return "invalid command"
end

function protocol_parser(mes)
    local i, j, index = 0, 0, 0
    i = j + 1
    j = string.find(mes, "#", i)
    if j == nil then
        return nil
    end
    index = j + 1
    local temp = string.sub(mes, i, j)
    local count = select(2, string.gsub(temp, " ", " "))
    if count == 1 then
        local command, key = string.match(temp, "(%a+)%s*(%a+)")
        return index, command, key
    elseif count == 2 then
        local command, key, value = string.match(temp, "(%a+)%s*(%a+)%s*(%a+)")
        return index, command, key, value
    else
        return index, "ERROR"
    end
end

function data_handler(sock_fd, data, len)
    while len ~= 0 do
        local index, command, key, value = protocol_parser(data)
        if index == nil then
            bh_write(sock_fd, "data is incomplete", 18)
            break
        end
        if string.upper(command) == "GET" then
            value = commands["GET"](key)
            bh_write(sock_fd, value, #value)
        elseif string.upper(command) == "SET" then
            commands["SET"](key, value)
            bh_write(sock_fd, "OK", 2)
        else
            command = "ERROR"
            value = commands[command]()
            bh_write(sock_fd, value, #value)
        end
        data = string.sub(data, index)
        len = len + 1 - index
    end
    return len
end

bh_run("normal", data_handler)
