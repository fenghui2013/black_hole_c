package.cpath = package.cpath .. ";./lualib_c/?.so"

local server = require "bh_server"

local redis = {}
redis.data = ""
redis.len = 0
redis.request_count = 1
redis.response_count = 1

function _request_count_inc()
    redis.request_count = redis.request_count + 1
end

function _response_count_inc()
    redis.response_count = redis.response_count + 1
end

function redis.connect(host, port)
    return server.connect(_G["bh"]["module"], _G["bh"]["event"], _G["bh"]["server"], host, port, "redis")
end

function redis.disconnect(sock_fd)
    server.close(_G["bh"]["event"], _G["bh"]["server"], sock_fd)
end

function _parse(prefix, data)
    local position = 1
    while string.byte(data, position) ~= 13 do
        position = position + 1
    end
    if prefix == "$" and string.byte(data, position) == 13 then
        return true, #tostring(position-1), tonumber(data:sub(1, position-1))
    else
        return false, nil
    end
end

function response_parse()
    while redis.len > 0 do
        local temp, data, len = nil, redis.data, redis.len
        local prefix, data, len = data:sub(1, -#data), data:sub(2), len-1
        local is_send = false
        -- status reply
        if prefix == "+" then
            if len >= 4 and data:sub(1, 2) == "OK" then
                temp, data, len = data:sub(1, 2), data:sub(5) or "", len-4
                is_send = true
            end
        -- bulk reply
        elseif prefix == "$" then
            local is_suc, position_len, data_len = _parse(prefix, data)
            if is_suc then
                if len >= (position_len+data_len+4) then
                    temp, data, len = data:sub(position_len+3, position_len+2+data_len), 
                    data:sub(position_len+data_len+5) or "", len-position_len-4-data_len
                    is_send = true
                end
            end
        end

        if is_send and redis[redis.response_count] then
            redis[redis.response_count](temp)
        end
        redis.data = data
        redis.len = len
        _response_count_inc()
    end
end

function redis.set(sock_fd, key, value, has_res, callback)
    message = "*3\r\n$3\r\nSET\r\n$" .. #key .. "\r\n" .. key .. "\r\n$" .. #value .. "\r\n" .. value .. "\r\n"
    server.send(_G["bh"]["event"], _G["bh"]["server"], sock_fd, message, #message)
    if has_res then
        if callback then
            redis[redis.request_count] = callback
        end
        _request_count_inc()
    end
end

function redis.get(sock_fd, key, has_res, callback)
    message = "*2\r\n$3\r\nGET\r\n$" .. #key .. "\r\n" .. key .. "\r\n"
    server.send(_G["bh"]["event"], _G["bh"]["server"], sock_fd, message, #message)
    if has_res then
        if callback then
            redis[redis.request_count] = callback
        end
        _request_count_inc()
    end
end

function data_handler(sock_fd, data, len)
    redis.data = redis.data .. data
    redis.len = redis.len + len
    response_parse()
end

_G["bh"]["redis_handler"] = data_handler

return redis
