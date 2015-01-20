package.cpath = "lualib_c/?.so"

local bh_socket = require "bh_socket"

sock_fd = bh_socket.create()
bh_socket.connect(sock_fd, "127.0.0.1", 8000)

bh_socket.send(sock_fd, "world", 5)
local data, size = bh_socket.recv(sock_fd, 1024)
print("receiving data: " .. data)

bh_socket.close(sock_fd);
