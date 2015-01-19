package.cpath = "lualib_c/?.so"

local socket = require "socket"

sock_fd = socket.create()
socket.bind(sock_fd, "127.0.0.1", 8000)
socket.listen(sock_fd, 1024)

local new_fd, ip, port = socket.accept(sock_fd)
if new_fd > 0 then
    print("ip: " .. tostring(ip) .. " port: " .. port)
elseif new_fd == 0 then
    print("again")
elseif new_fd == -1 then
    print("error")
end

local data, size = socket.recv(new_fd, 1024)
print("receiving data: " .. data)

socket.send(new_fd, "hello", 5)

socket.close(new_fd)
socket.close(sock_fd)
