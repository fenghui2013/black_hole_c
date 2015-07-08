cgi_envs = {}
cgi_env = {
    SERVER_PROTOCOL = "",
    REQUEST_METHOD = "",
    REQUEST_URI = "",
    QUERY_STRING = "",
    DOCUMENT_ROOT = "./"
}

SERVER_PROTOCOL = ""
REQUEST_METHOD = ""
REQUEST_URI = ""
QUERY_STRING = ""
DOCUMENT_ROOT = "./"

coroutines = {}

function set_global_cgi_env(sock_fd)
    SERVER_PROTOCOL = cgi_envs[sock_fd]["SERVER_PROTOCOL"]
    REQUEST_METHOD = cgi_envs[sock_fd]["REQUEST_METHOD"]
    REQUEST_URI = cgi_envs[sock_fd]["REQUEST_URI"]
    QUERY_STRING = cgi_envs[sock_fd]["QUERY_STRING"]
    DOCUMENT_ROOT = cgi_envs[sock_fd]["DOCUMENT_ROOT"]
end

function get_newline(buf, pos)
    local b, e, str = string.find(buf, "^([^\r\n]*)\r?\n", pos)
    return b, e, str
end

function protocol_parser(sock_fd, data, len)
    local index = 1
    local first_line = true
    local path, args
    while true do
        local b, e, str = get_newline(data, index)
        print(b, e, str)
        if b == nil then
            coroutine.yield(100, len+1-index)
        end
        if str == "" then
            --body
            if cgi_envs[sock_fd]["REQUEST_METHOD"] == "POST" then
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
            cgi_envs[sock_fd]["SERVER_PROTOCOL"] = "HTTP/" .. httpver
            cgi_envs[sock_fd]["REQUEST_METHOD"] = method
            cgi_envs[sock_fd]["REQUEST_URI"] = path
            cgi_envs[sock_fd]["QUERY_STRING"] = args
            first_line = false
        else
            --headers
            if string.find(str, ":") then
                local key, value = string.match(str, "(.*)%s*:%s*(.*)")
                print(key, value)
                if cgi_envs[sock_fd]["REQUEST_METHOD"] == "GET" then
                elseif cgi_envs[sock_fd]["REQUEST_METHOD"] == "POST" then
                else
                end
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
        cgi_envs[sock_fd] = cgi_env
    end

    if not coroutines[sock_fd] then
        local co = coroutine.create(protocol_parser)
        coroutines[sock_fd] = co
    end

    print(data, len)
    local err, code, left_len = coroutine.resume(coroutines[sock_fd], sock_fd, data, len)
    print(err, code, left_len)

    if code == 200 then
        coroutines[sock_fd] = nil
        set_global_cgi_env(sock_fd)
        print(SERVER_PROTOCOL)
        if string.find(REQUEST_URI, ".*%.lua$") then
            res = dofile("./examples/index.lua")
        elseif string.find(REQUEST_URI, ".*%.html$") then
            --local f = io.open(DOCUMNET_ROOT .. REQUEST_URI, "r")
            local f = io.open("./examples/index.html", "r")
            res = f:read("*a")
            f:close()
        else
            res = ""
        end
        local response = SERVER_PROTOCOL .. " 200 OK\r\n"
        response = response .. "Content-type: text/html\r\n"
        response = response .. "Content-length: " .. #res .. "\r\n"
        response = response .. "\r\n"
        response = response .. res
        print(response)
        bh_write(sock_fd, response, #response)
    elseif code == 100 then
    elseif code == 400 then
    else
    end
    return left_len
end

bh_run("normal", data_handler)
