local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local LevelFrame = class("LevelFrame",BaseFrame)

function LevelFrame:ctor(view,callbcak)
	LevelFrame.super.ctor(self,view,callbcak)
end

function LevelFrame:onConnectCompeleted()
	if self._operateCode == 0 then
		self:sendLoadLevel()
	end

end

function LevelFrame:onLoadLevel()
	self._operateCode = 0
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end

function LevelFrame:sendLoadLevel()
	local GrowLevelQueryInfo = CCmd_Data:create(136)
	GrowLevelQueryInfo:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_GROWLEVEL_QUERY)
	GrowLevelQueryInfo:pushdword(GlobalUserItem.dwUserID)
	GrowLevelQueryInfo:pushstring(string.upper(md5(GlobalUserItem.szPassword)),yl.LEN_MD5)
	GrowLevelQueryInfo:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
		--发送失败
	if not self:sendSocketData(GrowLevelQueryInfo) and nil ~= self._callBack then
		self._callBack(-1,"发送等级查询失败！")
	end
end

function LevelFrame:onSocketEvent(main,sub,pData)
	print("LevelFrame:onSocketEvent "..main.." "..sub)
	if main == yl.MDM_GP_USER_SERVICE then --用户服务
		if sub == yl.SUB_GP_GROWLEVEL_PARAMETER then
			-- 等级参数
			self:onSubGrowLevelParameter(pData)
		elseif sub == yl.SUB_GP_GROWLEVEL_UPGRADE then
			-- 用户升级
			self:onSubGrowLevelUpgrade(pData)
		end
	end
end

function LevelFrame:onSubGrowLevelParameter(pData)
	GlobalUserItem.wCurrLevelID = pData:readword()
	GlobalUserItem.dwExperience = pData:readdword()
	GlobalUserItem.dwUpgradeExperience = pData:readdword()
	GlobalUserItem.lUpgradeRewardGold = GlobalUserItem:readScore(pData)
	GlobalUserItem.lUpgradeRewardIngot = GlobalUserItem:readScore(pData)

	self:onCloseSocket()
	if nil ~= self._callBack then
		self._callBack(1)
	end	
end

function LevelFrame:onSubGrowLevelUpgrade(pData)
	local score = GlobalUserItem:readScore(pData)
	local ingot = GlobalUserItem:readScore(pData)
	local tips = pData:readstring()

	GlobalUserItem.lUserScore = score
	GlobalUserItem.lUserIngot = ingot
	--通知更新        
	local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
    eventListener.obj = yl.RY_MSG_USERWEALTH
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)	
	
	if nil ~= self._callBack then
		self._callBack(2, tips)
	end	
end

return LevelFrame