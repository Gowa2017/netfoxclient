
local M = {}

local function _assign(old, new, k)
    local otype = type(old[k])
    local ntype = type(new[k])
    if (otype == 'thread' or otype == 'userdata') or (ntype == 'thread' or ntype == 'userdata') then
        print(string.format('warning: old or new attr %s type be thread or userdata', k))
    end
    old[k] = new[k]
end

local function _replace(old, new, repeat_tbl)
    if repeat_tbl[old] then
        return
    end
    repeat_tbl[old] = true

    --收集该删除的
    local dellist = {}
    for k, v in pairs(old) do
        if not new[k] then
            table.insert(dellist, k)
        end
    end
    for _, v in ipairs(dellist) do
        old[v] = nil
    end

    --增加和替换
    for k, v in pairs(new) do
        if not old[k] then
            old[k] = new[k]
        else
            if type(old[k]) ~= type(new[k]) then
                print(string.format('warning: attr %s old type no equal new type!!!', k))
                _assign(old, new, k)
            else
                if type(old[k]) == 'table' then
                    _replace(old[k], new[k], repeat_tbl)
                else
                    _assign(old, new, k)
                end

            end
        end
    end
end

function M.reload(mod_name)
	local old_obj = package.loaded[mod_name] 

    if not old_obj then
        local m = require(mod_name)
        return m
    end
	
	--全局文件的更新
	if (type(old_obj) ~= "table") then
		package.loaded[mod_name] = nil
		require(mod_name)
		return
	end	
	
    printf("reload [%s]", mod_name)

    package.loaded[mod_name] = nil
	require(mod_name)
	
    return old_obj
end

function M.reloadObj(old_obj, new_obj)
    if type(old_obj) == 'table' and type(new_obj) == 'table' then
        local repeat_tbl = {}
        _replace(old_obj, new_obj, repeat_tbl)
    end
end

local functor = {}
M.functor = functor
functor.__index = functor

function functor.new(func, mod, ...)
    local self = {}
    self.func = func
    self.mod = mod
    local arg_len = select('#', ...)
    local arg1 = select(1, ...)
    if arg1 or arg_len > 1 then
        self.args = table.pack(...)
    end
    setmetatable(self, functor)
    return self
end

function functor:get()
    if self.mod then
        return self.mod[self.func]
    else
        return self.func
    end
end

function functor:call(...)
    local func = self:get()
    if self.args then
        return func(table.unpack(self.args), ...)
    else
        return func(...)
    end
end

return M

