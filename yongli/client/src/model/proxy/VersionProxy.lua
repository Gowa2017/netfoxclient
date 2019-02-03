--[[
	版本保存
]]
local VersionProxy = class("VersionProxy", puremvc.Proxy)	

function VersionProxy:ctor()
    self.proxyName = "VersionProxy"

	local fileUitls=cc.FileUtils:getInstance()
	--保存路径
	self._path = device.writablePath.."version.plist"
	--保存信息
	self._versionInfo  = fileUitls:getValueMapFromFile(self._path)
	self._downUrl = nil
end

--设置版本
function VersionProxy:setVersion(version)
	self._versionInfo["client"] = version
	self:save()
end

--获取版本
function VersionProxy:getVersion()
	return tonumber(self._versionInfo["client"])
end

--设置资源版本
function VersionProxy:setResVersion(version,kindid)
	if not kindid or kindid == 0 then
		self._versionInfo["res_client"] = version
	else
		self._versionInfo["res_game_"..kindid] = version
	end
	self:save()
end

--获取资源版本
function VersionProxy:getResVersion(kindid)
	if not kindid or kindid == 0 then 
		return tonumber(self._versionInfo["res_client"])
	else
		return tonumber(self._versionInfo["res_game_"..kindid])
	end
end

--保存版本
function VersionProxy:save()
	cc.FileUtils:getInstance():writeToFile(self._versionInfo,self._path)
end

function VersionProxy:onRegister()
    self.super.onRegister(self)
end

function VersionProxy:onRemove()
    self.super.onRemove(self)
end

return VersionProxy