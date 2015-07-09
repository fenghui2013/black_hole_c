local url = {}

function url.decode(s)
    s = string.gsub(s, "+", " ")
    s = string.gsub(s, "%%(%x%x)", function (h)
        return string.char(tonumber(h, 16))
    end)
    
    return s
end

function url.path_parser(path)
    return url.decode(path)
end

function url.query_string_parser(args)
    local res = {}
    for key, value in string.gmatch(args, "([^&=]+)=([^&=]+)") do
        key = url.decode(key)
        value = url.decode(value)
        res[key] = value
    end
    return res
end

return url
