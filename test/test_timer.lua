package.cpath = "lualib_c/?.so"

local timer = require "bh_timer"

function hello_handler()
    print(os.date(), "hello++++++++++")
end

function world_handler()
    print(os.date(), "world============")
end

function test_handler()
    print(os.date(), "----------------------------------------")
end

_G["bh"]["timeout_handlers"]["hello_handler"] = hello_handler
_G["bh"]["timeout_handlers"]["world_handler"] = world_handler
_G["bh"]["timeout_handlers"]["test_handler"] = test_handler

timer.set(_G["bh"]["timer"], 1000, -1, "hello_handler")
timer.set(_G["bh"]["timer"], 10000, 10, "world_handler")
timer.set(_G["bh"]["timer"], 100, 100, "test_handler")
