name = GET["name"] or "hello"
local res = bh_include("./examples/index.html")
name = nil
return res
