cgi_envs = {}
cgi_env = {
    AUTH_TYPE = "",
    CONTENT_LENGTH = "",
    CONTENT_TYPE = "",
    GATEWAY_INTERFACE = "",
    HTTP_* = "",
    PATH_INFO = "",
    PATH_TRANSLATED = "",
    QUERY_STRING = "",
    REMOTE_ADDR = "",
    REMOTE_HOST = "",
    REMOTE_USER = "",
    REMOTE_IDENT = "",
    SCRIPT_NAME = "",
    SERVER_NAME = "blackhole 0.0.1",
    SERVER_PORT = 80,
    SERVER_PROTOCOL = "",
    SERVER_SOFTWARE = ""
}

-- cgi environments
AUTH_TYPE = "",
CONTENT_LENGTH = "",
CONTENT_TYPE = "",
GATEWAY_INTERFACE = "",
HTTP_* = "",
PATH_INFO = "",
PATH_TRANSLATED = "",
QUERY_STRING = "",
REMOTE_ADDR = "",
REMOTE_HOST = "",
REMOTE_USER = "",
REMOTE_IDENT = "",
SCRIPT_NAME = "",
SERVER_NAME = "blackhole 0.0.1",
SERVER_PORT = 80,
SERVER_PROTOCOL = "",
SERVER_SOFTWARE = ""

function set_global_cgi_env(sock_fd)
    AUTH_TYPE = cgi_envs[sock_fd]["AUTH_TYPE"],
    CONTENT_LENGTH = cgi_envs[sock_fd]["CONTENT_LENGTH"],
    CONTENT_TYPE = cgi_envs[sock_fd]["CONTENT_TYPE"],
    GATEWAY_INTERFACE = cgi_envs[sock_fd]["GATEWAY_INTERFACE"],
    HTTP_* = cgi_envs[sock_fd]["HTTP_*"],
    PATH_INFO = cgi_envs[sock_fd]["PATH_INFO"],
    PATH_TRANSLATED = cgi_envs[sock_fd]["PATH_TRANSLATED"],
    QUERY_STRING = cgi_envs[sock_fd]["QUERY_STRING"],
    REMOTE_ADDR = cgi_envs[sock_fd]["REMOTE_ADDR"],
    REMOTE_HOST = cgi_envs[sock_fd]["REMOTE_HOST"],
    REMOTE_USER = cgi_envs[sock_fd]["REMOTE_USER"],
    REMOTE_IDENT = cgi_envs[sock_fd]["REMOTE_IDENT"],
    SCRIPT_NAME = cgi_envs[sock_fd]["SCRIPT_NAME"],
    SERVER_NAME = cgi_envs[sock_fd]["SERVER_NAME"],
    SERVER_PORT = cgi_envs[sock_fd]["SERVER_PORT"],
    SERVER_PROTOCOL = cgi_envs[sock_fd]["SERVER_PROTOCOL"],
    SERVER_SOFTWARE = cgi_envs[sock_fd]["SERVER_SOFTWARE"]
end

function data_handler(sock_fd, data, len)
    if data == "" then
        cgi_envs[sock_fd] = nil
        return 0
    end

    if not cgi_envs[sock_fd] then
        cgi_envs[sock_fd] = cgi_env
    end
    set_global_cgi_env(sock_fd)
    local res = dofile("./examples/test.lua")
    bh_write(sock_fd, res, #res)
    return 0
end

bh_run("normal", data_handler)
