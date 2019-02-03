--[[
 	下载更新记录
]]--
local HotFixUpdateProxy = class("HotFixUpdateProxy", puremvc.Proxy)

-- url:下载list地址 
-- wirtepath:保存路径
-- curfile:当前list路径
function HotFixUpdateProxy:ctor()
    self.super.ctor(self)
    self.proxyName = "HotFixUpdateProxy"
	
	self.updateFiles = {}
end

function HotFixUpdateProxy:setUpdateFiles(var)
	self.updateFiles = var or {}
end

function HotFixUpdateProxy:getUpdateFiles()
	return self.updateFiles
end

return HotFixUpdateProxy