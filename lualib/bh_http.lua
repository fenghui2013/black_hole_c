package.path = package.path .. ";./lualib/?.lua"

local bh_url = require "bh_url"

cgi_envs = {}
--SERVER_PROTOCOL = ""
--REQUEST_METHOD = ""
--REQUEST_URI = ""
--QUERY_STRING = ""
--DOCUMENT_ROOT = "./"
SERVER = {}
GET = {}
POST = {}

coroutines = {}

function global_cgi_env_set(sock_fd)
    SERVER = cgi_envs[sock_fd]["SERVER"]
    GET = cgi_envs[sock_fd]["GET"]
    POST = cgi_envs[sock_fd]["POST"]
end

function global_cgi_env_reset(sock_fd)
    if cgi_envs[sock_fd]["SERVER"]["REQUEST_METHOD"] == "GET" then
        cgi_envs[sock_fd]["GET"] = {}
    elseif cgi_envs[sock_fd]["SERVER"]["REQUEST_METHOD"] == "POST" then
        cgi_envs[sock_fd]["POST"] = {}
    else
    end
    SERVER ={}
    GET = {}
    POST = {}
end

function get_newline(buf, pos)
    local b, e, str = string.find(buf, "^([^\r\n]*)\r?\n", pos)
    return b, e, str
end

function protocol_parser(sock_fd, data, len)
    local index = 1
    local first_line = true
    local path, args = nil, nil
    while true do
        local b, e, str = get_newline(data, index)
        print(b, e, str)
        if b == nil then
            coroutine.yield(100, len+1-index)
        end
        if str == "" then
            --body
            if cgi_envs[sock_fd]["SERVER"]["REQUEST_METHOD"] == "POST" then
            end
            index = e + 1
            break
        end
        if first_line then
            --request line
            local method, url, httpver = string.match(str, "^(%a+)%s+(.-)%s+HTTP/([%d%.]+)$")
            print(method, url, httpver)
            assert(method and url and httpver)

            if string.find(url, "?", 1, true) then
                path , args = string.match(url, "^([^?]+)%?(.+)$")
                print(path, args)
            else
                path, args = url, nil
            end
            cgi_envs[sock_fd]["SERVER"]["SERVER_PROTOCOL"] = "HTTP/" .. httpver
            cgi_envs[sock_fd]["SERVER"]["REQUEST_METHOD"] = method
            cgi_envs[sock_fd]["SERVER"]["REQUEST_URI"] = bh_url.path_parser(path)
            cgi_envs[sock_fd]["SERVER"]["QUERY_STRING"] = args
            if method == "GET" then
                if args then
                    local res = bh_url.query_string_parser(args)
                    cgi_envs[sock_fd]["GET"] = res
                end
            elseif method == "POST" then
                if args then
                    local res = bh_url.query_string_parser(args)
                    cgi_envs[sock_fd]["POST"] = res
                end
            else
            end
            first_line = false
        else
            --headers
            if string.find(str, ":") then
                local key, value = string.match(str, "(.-)%s*:%s*(.*)")
                print(key, value)
            else
                return 400, len+1-index
            end
        end
        index = e + 1
    end
    return 200, len+1-index
end

function data_handler(sock_fd, data, len)
    local res = nil

    if data == "" then
        cgi_envs[sock_fd] = nil
        return 0
    end

    if not cgi_envs[sock_fd] then
        cgi_envs[sock_fd] = {}
        cgi_envs[sock_fd]["SERVER"] = {}
        cgi_envs[sock_fd]["GET"] = {}
        cgi_envs[sock_fd]["POST"] = {}
    end

    if not coroutines[sock_fd] then
        local co = coroutine.create(protocol_parser)
        coroutines[sock_fd] = co
    end

    print(data, len)
    local err, code, left_len = coroutine.resume(coroutines[sock_fd], sock_fd, data, len)
    print(err, code, left_len)

    global_cgi_env_set(sock_fd)
    if code == 200 then
        coroutines[sock_fd] = nil
        print(SERVER["SERVER_PROTOCOL"])
        if string.find(SERVER["REQUEST_URI"], ".*%.lua") then
            res = dofile("./examples/index.lua")
        elseif string.find(SERVER["REQUEST_URI"], ".*%.html") then
            --local f = io.open(DOCUMNET_ROOT .. REQUEST_URI, "r")
            local f = io.open("./examples/index.html", "r")
            res = f:read("*a")
            f:close()
        else
            res = ""
        end
        local response = SERVER["SERVER_PROTOCOL"] .. " 200 OK\r\n"
        response = response .. "Content-type: text/html\r\n"
        response = response .. "Content-length: " .. #res .. "\r\n"
        response = response .. "\r\n"
        response = response .. res
        print(response)
        bh_write(sock_fd, response, #response)
        global_cgi_env_reset(sock_fd)
    elseif code == 100 then
    elseif code == 400 then
    else
    end
    return left_len
end

function bh_include(file_name)
    local f = io.open(file_name, "r")
    local res = f:read("*a")
    f:close()
    res = string.gsub(res, "%[%[(.-)%]%]", function(args)
        return load(args)()
    end)
    return res
end

bh_run("normal", data_handler)
