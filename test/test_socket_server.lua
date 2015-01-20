package.cpath = "lualib_c/?.so"

local bh_socket = require "bh_socket"

sock_fd = bh_socket.create()
bh_socket.bind(sock_fd, "127.0.0.1", 8000)
bh_socket.listen(sock_fd, 1024)

local new_fd, ip, port = bh_socket.accept(sock_fd)
if new_fd > 0 then
    print("ip: " .. tostring(ip) .. " port: " .. port)
elseif new_fd == 0 then
    print("again")
elseif new_fd == -1 then
    print("error")
end

local data, size = bh_socket.recv(new_fd, 1024)
print("receiving data: " .. data)

bh_socket.send(new_fd, "hello", 5)

bh_socket.close(new_fd)
bh_socket.close(sock_fd)
