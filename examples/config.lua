root = "./"
ip = "0.0.0.0"
port = 8000
threads = 3
lua_vms = 1
server_type = "http"
lua_modules = root .. "lualib/bh_http.lua;"
http = {
    {
        ip = "0.0.0.0",
        port = 8081,
        root = "./examples",
        server_name = "127.0.0.1",
        [".*$.lua"] = {
        },
        [".*$.html"] = {
        }
    }
}
