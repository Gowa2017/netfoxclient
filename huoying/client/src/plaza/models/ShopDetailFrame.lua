--[[
	商城接口
	2016_07_05 Ravioyla
]]

local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local ShopDetailFrame = class("ShopDetailFrame",BaseFrame)
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

function ShopDetailFrame:ctor(view,callbcak)
	ShopDetailFrame.super.ctor(self,view,callbcak)
end

--连接结果
function ShopDetailFrame:onConnectCompeleted() 

	print("============ShopDetail onConnectCompeleted============")
	print("ConnectCompeleted oprateCode="..self._oprateCode)

	if self._oprateCode == 0 then	--购买
		self:sendPropertyBuy()
	elseif self._oprateCode == 1 then	--使用
		self:sendPropertyUse()
	elseif self._oprateCode == 2 then	--背包查询
		self:sendQueryBag()
	elseif self._oprateCode == 3 then	--赠送
		self:sendPropertyTrans()
	elseif self._oprateCode == 4 then
		self:sendQuerySend() 			--获取赠送
	else
		print("self:onCloseSocket() 1")
		self:onCloseSocket()
		self._callBack(-1,"未知操作模式！")
	end

end

--网络信息(短连接)
function ShopDetailFrame:onSocketEvent(main,sub,pData)
	print("============ ShopDetail onSocketEvent ============")
	print("socket event:"..main.."#"..sub)

	if main == yl.MDM_GP_PROPERTY then --道具命令
		if sub == yl.SUB_GP_PROPERTY_BUY_RESULT then --购买
			local buySuccess = self:onSubPropertyBuyResult(pData)
			if self._use == 1 and buySuccess then
				self._use = 0
				if false == self._callBack(yl.SUB_GP_PROPERTY_BUY_RESULT,tonumber(self._id)) then					
					self:sendPropertyUseNoConnect(self._id,self._itemCount)
					return
				end				
			end			
		elseif sub == yl.SUB_GP_PROPERTY_USE_RESULT then 		--使用
			self:onSubPropertyUseResult(pData)
		elseif sub == yl.SUB_GP_PROPERTY_FAILURE then 			--失败
			self:onSubPropertyFailure(pData)
		elseif sub == yl.SUB_GP_QUERY_BACKPACKET_RESULT then 	--背包查询
			self:onSubQueryBag(pData)
		elseif sub == yl.SUB_GP_PROPERTY_PRESENT_RESULT then 	--赠送
			self:onSubPropertyTransResult(pData)
		elseif sub == yl.SUB_GP_GET_SEND_PRESENT_RESULT then	--获取赠送
			self:onSubQuerySend(pData)
			--查询背包
			self:sendQueryBag()
			return
		else
			local message = string.format("未知命令码：%d-%d",main,sub)
			self._callBack(-1,message)
		end
	end

	self._use = 0
	self:onCloseSocket()
end

--网络消息(长连接)
function ShopDetailFrame:onGameSocketEvent(main,sub,pData)
	if main == game_cmd.MDM_GR_PROPERTY then
		print("GameSocket ShopDetail #" .. main .. "# #" .. sub .. "#")
		if sub == game_cmd.SUB_GR_GAME_PROPERTY_BUY_RESULT then 			-- 道具购买
			local buySuccess = self:onSubPropertyBuyResult(pData)
			if self._use == 1 and buySuccess then
				self._use = 0
				if false == self._callBack(yl.SUB_GP_PROPERTY_BUY_RESULT,tonumber(self._id)) then					
					self:onPropertyUse(self._id,self._itemCount)
				end				
			end	
			self._use = 0
		elseif sub == game_cmd.SUB_GR_PROPERTY_USE_RESULT then 				-- 道具使用
			local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_S_PropertyUse, pData)
			dump(cmd_table, "CMD_GR_S_PropertyUse", 5)

			GlobalUserItem.lUserScore = cmd_table.Score
			local tmpOrder = cmd_table.cbMemberOrder
			if tmpOrder > GlobalUserItem.cbMemberOrder then
				GlobalUserItem.cbMemberOrder = tmpOrder
			end
			if nil ~= self._callBack then
				self._callBack(2,cmd_table.szNotifyContent)
			end
			--通知更新        
			local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
		    eventListener.obj = yl.RY_MSG_USERWEALTH
		    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
		elseif sub == game_cmd.SUB_GR_PROPERTY_FAILURE then 				-- 道具失败
			local cmd_table = ExternalFun.read_netdata(game_cmd.CMD_GR_PropertyFailure, pData)
			dump(cmd_table, "CMD_GR_PropertyFailure", 5)
			if nil ~= self._callBack then
				self._callBack(0,cmd_table.szDescribeString)
			end
		elseif sub == game_cmd.SUB_GR_PROPERTY_BACKPACK_RESULT then 		-- 背包查询
			self:onSubQueryBag(pData)
		elseif sub == game_cmd.SUB_GR_PROPERTY_PRESENT_RESULT then 			-- 道具赠送
			self:onSubPropertyTransResult(pData)
		elseif sub == game_cmd.SUB_GR_GET_SEND_PRESENT_RESULT then 			-- 获取赠送
			-- 获取赠送

			-- 查询背包
			self:onQueryBag()
		elseif sub == game_cmd.SUB_GR_GAME_PROPERTY_FAILURE then
			self:onSubPropertyFailure(pData)
		end
	end
end

-- CMD_LogonServer CMD_GP_S_PropertySuccess
function ShopDetailFrame:onSubPropertyBuyResult(pData)
	print("============ ShopDetailFrame:onSubPropertyBuyResult ============")
	local cmdtable = ExternalFun.read_netdata(logincmd.CMD_GP_S_PropertySuccess, pData)

	--判断是大喇叭
	if cmdtable.dwPropID == yl.LARGE_TRUMPET then
		GlobalUserItem.nLargeTrumpetCount = GlobalUserItem.nLargeTrumpetCount + cmdtable.dwPropCount
	end

	--更新用户银行金币
	GlobalUserItem.lUserInsure = cmdtable.lInsureScore
	--更新元宝
	GlobalUserItem.lUserIngot = cmdtable.lUserMedal
	--更新游戏豆
	GlobalUserItem.dUserBeans = cmdtable.dCash

	--通知更新        
	local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
    eventListener.obj = yl.RY_MSG_USERWEALTH
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

    local tips = cmdtable.szNotifyContent
    if 1 == self._use then
    	tips = nil
    end
    if nil ~= self._callBack then
    	self._callBack(1,tips)
    end
	
	return true
end

function ShopDetailFrame:onSubPropertyUseResult(pData)
	print("============ ShopDetailFrame:onSubPropertyUseResult ============")
	-- CMD_GP_S_PropertyUse
	local userID = pData:readdword()
	local recvUserID = pData:readdword()
	local propID = pData:readdword()
	local multiple = pData:readdword()
	local propCount = pData:readdword()
	local remainderPropCount = pData:readdword()

	GlobalUserItem.lUserScore = GlobalUserItem:readScore(pData)

	print("*propCount-"..propCount)
	print("*lSendLoveLiness-"..GlobalUserItem:readScore(pData))
	print("*lRecvLoveLiness-"..GlobalUserItem:readScore(pData))
	print("*lUseResultsGold-"..GlobalUserItem:readScore(pData))

	local propKind = pData:readdword()
	local userTime = pData:readdword()--GlobalUserItem:readScore(pData)
	local resultValidTime = pData:readdword()
	local handleCode = pData:readdword()
	local szName = pData:readstring(16)
	local tmpOrder = pData:readbyte()
	if tmpOrder > GlobalUserItem.cbMemberOrder then
		GlobalUserItem.cbMemberOrder = tmpOrder
	end

	local szTip = pData:readstring()

	if nil ~= self._callBack then
		self._callBack(2,szTip)
	end
	-- 通知更新
	local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
    eventListener.obj = yl.RY_MSG_USERWEALTH
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

function ShopDetailFrame:onSubPropertyFailure(pData)
	print("============ ShopDetailFrame:onSubPropertyFailure ============")
	local code = pData:readdword()
	local szTip = pData:readstring()

	if nil ~= self._callBack then
		self._callBack(0,szTip)
	end
end

-- CMD_LogonServer CMD_GP_S_BackpackProperty
function ShopDetailFrame:onSubQueryBag(pData)
	print("============ ShopDetailFrame:onSubQueryBag ============")
	local userID = pData:readdword()
	local status = pData:readdword()
	local count = pData:readdword()
	local list = {}
	for i=1,count do
		local item = {}
		--道具信息
		item._count = pData:readint()
		--game_cmd.tagPropertyInfo
		item._index = pData:readword()
		item._kind  = pData:readword()
		item._shop  = pData:readword()
		item._area  = pData:readword()
		item._service = pData:readword()
		item._commend = pData:readword()
		item._multiple= pData:readword()
		item._cbSuportMobile = pData:readbyte()
		--销售价格
		item._gold  = GlobalUserItem:readScore(pData)
		item._bean  = pData:readdouble()
		item._ingot = GlobalUserItem:readScore(pData) 
		item._loveliness = GlobalUserItem:readScore(pData)
		--赠送内容
		item._sendLoveliness = GlobalUserItem:readScore(pData)
		item._recvLoveliness = GlobalUserItem:readScore(pData)
		item._resultGold = GlobalUserItem:readScore(pData)

		item._name = pData:readstring(32) or ""
		item._info = pData:readstring(128) or ""

		table.insert(list,item)
		--如果是大喇叭
		if item._index == yl.LARGE_TRUMPET then
			GlobalUserItem.nLargeTrumpetCount = item._count
		end
	end

	if nil ~= self._callBack then
		self._callBack(yl.SUB_GP_QUERY_BACKPACKET_RESULT,list)
	end
end

function ShopDetailFrame:onSubPropertyTransResult(pData)
	print("============ ShopDetailFrame:onSubPropertyTransResult ============")
	local userID = pData:readdword()
	local recvGameID = pData:readdword()
	local propID = pData:readdword()

	local propCount = pData:readword()
	local wType = pData:readword()

	local szName = pData:readstring(16)
	local nHandleCode = pData:readint()
	local szTip = pData:readstring()

	print("----------------------------")
	print("*userid-"..userID)
	print("*recvGameID-"..recvGameID)
	print("*propID-"..propID)
	print("*propCount-"..propCount)
	print("*type-"..wType)
	print("*name-"..szName)
	print("*code-"..nHandleCode)
	print("*tip-"..szTip)

	if nil ~= self._callBack then
		self._callBack(2,szTip)
	end
end

function ShopDetailFrame:onSubQuerySend(pData)
	local wCount = pData:readword()
	print("count ==> " .. wCount)
	--[[for i = 1, wCount do
		local presend = ExternalFun.read_netdata(logincmd.SendPresent, pData)
		dump(presend, "pData", 6)
	end]]
end

--道具购买
function ShopDetailFrame:sendPropertyBuy()
	local PropertyBuy = CCmd_Data:create(145)
	PropertyBuy:setcmdinfo(yl.MDM_GP_PROPERTY,yl.SUB_GP_PROPERTY_BUY)

	PropertyBuy:pushdword(GlobalUserItem.dwUserID)
	PropertyBuy:pushdword(self._id)
	PropertyBuy:pushdword(self._itemCount)
	PropertyBuy:pushbyte(self._consumeType)
	PropertyBuy:pushstring(md5(GlobalUserItem.szPassword),33)
	PropertyBuy:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)

	--发送失败
	if not self:sendSocketData(PropertyBuy) and nil ~= self._callBack then
		self._callBack(-1,"发送道具购买失败！")
	end
end

--道具使用(非连接)
function ShopDetailFrame:sendPropertyUseNoConnect(id,count)
	print("============ ShopDetailFrame:sendPropertyUseNoConnect ============")
	local PropertyUse = CCmd_Data:create(14)
	PropertyUse:setcmdinfo(yl.MDM_GP_PROPERTY,yl.SUB_GP_PROPERTY_USE)
	PropertyUse:pushdword(GlobalUserItem.dwUserID)
	PropertyUse:pushdword(GlobalUserItem.dwUserID)
	PropertyUse:pushdword(id)
	PropertyUse:pushword(count)

	--发送失败
	if not self:sendSocketData(PropertyUse) and nil ~= self._callBack then
		self._callBack(-1,"发送道具使用失败！")
	end
end


--道具使用(正常)
function ShopDetailFrame:sendPropertyUse()
	local PropertyUse = CCmd_Data:create(14)
	PropertyUse:setcmdinfo(yl.MDM_GP_PROPERTY,yl.SUB_GP_PROPERTY_USE)
	PropertyUse:pushdword(GlobalUserItem.dwUserID)
	PropertyUse:pushdword(GlobalUserItem.dwUserID)
	PropertyUse:pushdword(self._id)
	PropertyUse:pushword(self._itemCount)

	--发送失败
	if not self:sendSocketData(PropertyUse) and nil ~= self._callBack then
		self._callBack(-1,"发送道具使用失败！")
	end
end

--获取背包
function ShopDetailFrame:sendQueryBag()
	local QueryBag = CCmd_Data:create(8)
	QueryBag:setcmdinfo(yl.MDM_GP_PROPERTY,yl.SUB_GP_QUERY_BACKPACKET)
	QueryBag:pushdword(GlobalUserItem.dwUserID)
	QueryBag:pushdword(0)

	--发送失败
	if not self:sendSocketData(QueryBag) and nil ~= self._callBack then
		self._callBack(-1,"发送背包查询失败！")
	end
end

--获取赠送
function ShopDetailFrame:sendQuerySend()
	-- CMD_GP_C_GetSendPresent
	local QueryBag = CCmd_Data:create(70)
	QueryBag:setcmdinfo(yl.MDM_GP_PROPERTY,yl.SUB_GP_GET_SEND_PRESENT)
	QueryBag:pushdword(GlobalUserItem.dwUserID)
	QueryBag:pushstring(md5(GlobalUserItem.szPassword),33)

	--发送失败
	if not self:sendSocketData(QueryBag) and nil ~= self._callBack then
		self._callBack(-1,"发送赠送查询失败！")
	end
end

--赠送
function ShopDetailFrame:sendPropertyTrans()
	local PropertyTrans = CCmd_Data:create(48)
	PropertyTrans:setcmdinfo(yl.MDM_GP_PROPERTY,yl.SUB_GP_PROPERTY_PRESENT)

	PropertyTrans:pushdword(GlobalUserItem.dwUserID)
	PropertyTrans:pushdword(self._recvid)
	PropertyTrans:pushdword(self._id)

	PropertyTrans:pushword(self._count)
	PropertyTrans:pushword(self._type)

	PropertyTrans:pushstring(self._nickname)

	print("-------------------------------")
	print("*recvid-"..self._recvid)
	print("*id-"..self._id)
	print("*count-"..self._count)
	print("*nickname-"..self._nickname)

	--发送失败
	if not self:sendSocketData(PropertyTrans) and nil ~= self._callBack then
		self._callBack(-1,"发送赠送失败！")
	end
end

--道具购买
function ShopDetailFrame:onPropertyBuy(type,count,id,use)	
	self._consumeType = type
	self._itemCount = count
	self._id = id
	self._use = use

	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_GamePropertyBuy)
		buffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_GAME_PROPERTY_BUY)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushdword(self._id)
		buffer:pushdword(self._itemCount)
		buffer:pushbyte(self._consumeType)
		buffer:pushstring(md5(GlobalUserItem.szPassword),33)
		buffer:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
		
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送购买失败！")
		end
	else
		--操作记录
		self._oprateCode = 0
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--道具使用
function ShopDetailFrame:onPropertyUse(id,count)	
	self._itemCount = count
	self._id = id

	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_PropertyUse)
		buffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_PROPERTY_USE)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushdword(self._id)
		buffer:pushword(self._itemCount)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送使用失败！")
		end
	else
		--操作记录
		self._oprateCode = 1
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--获取背包
function ShopDetailFrame:onQueryBag()
	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_BackpackProperty)
		buffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_PROPERTY_BACKPACK)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送查询失败！")
		end
	else
		--操作记录
		self._oprateCode = 2

		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--赠送
function ShopDetailFrame:onPropertyTrans(itemid,type,recvid,nickname,count)	
	self._id = itemid
	self._type = type
	self._recvid = recvid
	self._nickname = nickname
	self._count = count

	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_PropertyPresent)
		buffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_PROPERTY_PRESENT)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushdword(self._recvid)
		buffer:pushdword(self._id)
		buffer:pushword(self._count)
		buffer:pushword(self._type)
		buffer:pushstring(self._nickname)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送赠送失败！")
		end
	else
		--操作记录
		self._oprateCode = 3
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--获取赠送
function ShopDetailFrame:onQuerySend()
	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_GetSendPresent)
		buffer:setcmdinfo(game_cmd.MDM_GR_PROPERTY,game_cmd.SUB_GR_GET_SEND_PRESENT)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushdword(0)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送查询失败！")
		end
	else
		--操作记录
		self._oprateCode = 4

		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

return ShopDetailFrame