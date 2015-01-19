package.cpath = "lualib_c/?.so"

local socket = require "socket"

sock_fd = socket.create()
socket.connect(sock_fd, "127.0.0.1", 8000)

socket.send(sock_fd, "world", 5)
local data, size = socket.recv(sock_fd, 1024)
print("receiving data: " .. data)

socket.close(sock_fd);
