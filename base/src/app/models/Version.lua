--[[
	版本保存
]]
local Version = class("Version", function()
     local node = display.newNode()
     return node
end)	

function Version:ctor()
	self:retain()
	local sp = ""
	local fileUitls=cc.FileUtils:getInstance()
	--保存路径
	self._path = device.writablePath..sp.."version.plist"
	--保存信息
	self._versionInfo  = fileUitls:getValueMapFromFile(self._path)
	self._downUrl = nil
end

--设置版本
function Version:setVersion(version,kindid)
	if not kindid then
		self._versionInfo["client"] = version
	else
		self._versionInfo["game_"..kindid] = version
	end
end

--获取版本
function Version:getVersion(kindid)
	if not kindid then 
		return self._versionInfo["client"]
	else
		return self._versionInfo["game_"..kindid]
	end
end

--设置资源版本
function Version:setResVersion(version,kindid)
	if not kindid then
		self._versionInfo["res_client"] = version
	else
		self._versionInfo["res_game_"..kindid] = version
	end
	self:save()
end

--获取资源版本
function Version:getResVersion(kindid)
	if not kindid then 
		return self._versionInfo["res_client"]
	else
		return self._versionInfo["res_game_"..kindid]
	end
end

--保存版本
function Version:save()
	cc.FileUtils:getInstance():writeToFile(self._versionInfo,self._path)
end

return Version