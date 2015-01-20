package.cpath = "lualib_c/?.so"

temp = "a"
a = ""
for v=1, 1024, 1 do
    a = a .. temp
end
print(#a)
---[[
local bh_string = require "bh_string"
s = bh_string.create(1)
print("size: " .. bh_string.get_size(s) .. " len: " .. bh_string.get_len(s) .. " free:" .. bh_string.get_free(s))
bh_string.set(s, "abcdef", 0, 6, 0)
print("size: " .. bh_string.get_size(s) .. " len: " .. bh_string.get_len(s) .. " free:" .. bh_string.get_free(s))
print(string.sub(bh_string.get(s), 0, bh_string.get_len(s)))
for v=1, 65, 1 do
    bh_string.set(s, a, 0, 1024, 1)
    print("size: " .. bh_string.get_size(s) .. " len: " .. bh_string.get_len(s) .. " free:" .. bh_string.get_free(s))
    --print(string.sub(bh_string.get(s), 0, bh_string.get_len(s)))
end
bh_string.release(s)
--]]
