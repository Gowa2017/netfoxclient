--[[
	签到接口
	2016_06_16 Ravioyla
]]

local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local CheckinFrame = class("CheckinFrame",BaseFrame)

function CheckinFrame:ctor(view,callbcak)
	CheckinFrame.super.ctor(self,view,callbcak)
end

--
CheckinFrame.QUERYCHECKIN = 0
CheckinFrame.CHECKINDONE = 1
CheckinFrame.GETMEMBERGIFT = 2
CheckinFrame.QUERYMEMBERGIFT = 3
CheckinFrame.BASEENSURETAKE = 4
CheckinFrame.BASEENSUREQUERY = 5

--连接结果
function CheckinFrame:onConnectCompeleted()

	print("============Checkin onConnectCompeleted============")
	print("CheckinFrame:onConnectCompeleted oprateCode="..self._oprateCode)

	if self._oprateCode == CheckinFrame.QUERYCHECKIN then			--查询
		self:sendCheckinQuery()
	elseif self._oprateCode == CheckinFrame.CHECKINDONE then		--签到
		self:sendCheckinDone()
	elseif self._oprateCode == CheckinFrame.GETMEMBERGIFT then		--领取会员礼包
		self:sendGetMemberGift()
	elseif self._oprateCode == CheckinFrame.QUERYMEMBERGIFT then 	--查询会员礼包
		self:sendCheckMemberGift()		
	elseif self._oprateCode == CheckinFrame.BASEENSURETAKE then 	--领取低保
		self:sendBaseEnsureTake()		
	elseif self._oprateCode == CheckinFrame.BASEENSUREQUERY then 	--低保参数查询
		self:sendBaseEnsureLoad()
	else
		self:onCloseSocket()
		if nil ~= self._callBack then
			self._callBack(-1,"未知操作模式！")
		end
	end

end

--网络信息
function CheckinFrame:onSocketEvent(main,sub,pData)
	print("============Checkin onSocketEvent============")
	print("*socket event:"..main.."#"..sub) 
	local bRes = false
	if main == yl.MDM_GP_USER_SERVICE then --用户服务
		if sub == yl.SUB_GP_CHECKIN_INFO then 						--查询签到
			bRes = self:onSubCheckinInfo(pData)
		elseif sub == yl.SUB_GP_CHECKIN_RESULT then 				--签到结果
			bRes = self:onSubCheckinResult(pData)
		elseif sub == yl.SUB_GP_MEMBER_QUERY_INFO_RESULT then 		--查询礼包结果
			self:onSubCheckMemberGift(pData)
		elseif sub == yl.SUB_GP_MEMBER_DAY_GIFT_RESULT then 		--领取礼包结果
			bRes = self:onSubGetMemberGift(pData)
		elseif sub == yl.SUB_GP_MEMBER_DAY_PRESENT_RESULT then
			self:onSubGetVipPresent(pData)
		elseif sub == yl.SUB_GP_BASEENSURE_RESULT then 				--领取低保结果
			self:onSubBaseEnsureResult(pData)
		elseif sub == yl.SUB_GP_BASEENSURE_PARAMETER then 			--低保参数
			self:onSubBaseEnsureParam(pData)
		else
			local message = string.format("未知命令码：%d-%d",main,sub)
			if nil ~= self._callBack then
				self._callBack(-1,message)
			end			
		end
	end

	if not bRes then
		self:onCloseSocket()
	end	
end

function CheckinFrame:onSubCheckinInfo(pData)
	print("============CheckinFrame:onSubCheckinInfo============")
	GlobalUserItem.wSeriesDate = pData:readword()
	print("wSeriesDate-"..GlobalUserItem.wSeriesDate)
	GlobalUserItem.bTodayChecked = pData:readbool()
	GlobalUserItem.bQueryCheckInData = true
	for i=1,yl.LEN_WEEK do
		GlobalUserItem.lRewardGold[i] = GlobalUserItem:readScore(pData)
		print("lRewardGold["..i.."]-"..GlobalUserItem.lRewardGold[i])
	end

	if GlobalUserItem.bTodayChecked == true then
		--非会员标记当日已签到
		if GlobalUserItem.cbMemberOrder == 0 then
			GlobalUserItem.setTodayCheckIn()
		end
	end

	if nil ~= self._callBack then
		return self._callBack(1)
	end	
end

function CheckinFrame:onSubCheckinResult(pData)
	-- CMD_GP_CheckInResult
	print("============CheckinFrame:onSubCheckinResult============")
	GlobalUserItem.bTodayChecked = pData:readbool()
	local lscore = GlobalUserItem:readScore(pData)	
	local szTip = pData:readstring()
	GlobalUserItem.bQueryCheckInData = true
	if GlobalUserItem.bTodayChecked == true then
		GlobalUserItem.lUserScore = lscore
		GlobalUserItem.wSeriesDate = GlobalUserItem.wSeriesDate+1

		--非会员标记当日已签到
		if GlobalUserItem.cbMemberOrder == 0 then
			GlobalUserItem.setTodayCheckIn()
		end
	end

	--通知更新        
	local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
    eventListener.obj = yl.RY_MSG_USERWEALTH
    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)

	if nil ~= self._callBack then
		return self._callBack(10,szTip)
	end	
end

function CheckinFrame:onSubCheckMemberGift(pData)

	print("============ CheckinFrame:onSubCheckMemberGift ============")
	local bPresent = pData:readbool()
	local bGift = pData:readbool()
	local giftCount = pData:readdword()
	print("gift count " .. giftCount)

	local gifts = {}
	for i=1,giftCount do
		local item = {}		 
		item.count = pData:readword()
		item.id = pData:readword()

		--手机端筛选
		--if item.id > 100 then
			table.insert(gifts,item)
		--end		
	end
	-- 加入会员奖励
	local item = {}
	item.count = GlobalUserItem.MemberList[GlobalUserItem.cbMemberOrder]._present or 0
	print("奖励 " .. item.count)
	if 0 ~= item.count then
		item.id = "money"
		table.insert(gifts,item)
	end	

	if nil ~= self._callBack then
		if bGift then
			self._callBack(CheckinFrame.QUERYMEMBERGIFT, gifts, true)
		else
			if GlobalUserItem.cbMemberOrder ~= 0 then
				GlobalUserItem.setTodayCheckIn()
			end
			self._callBack(CheckinFrame.QUERYMEMBERGIFT, gifts, false)
		end
	end
end

function CheckinFrame:onSubGetMemberGift(pData)
	print("============ CheckinFrame:onSubGetMemberGift ============")

	local bSuccessed = pData:readbool()
	local szTip = pData:readstring()

	print("Gift tips " .. szTip)
	if nil ~= self._callBack then
		return self._callBack(CheckinFrame.GETMEMBERGIFT,"")
	end 
	return false
end

-- 会员送金结果
-- CMD_GP_MemberDayPresentResult
function CheckinFrame:onSubGetVipPresent(pData)
	print("============ CheckinFrame:onSubGetVipPresent ============")
	local bSuccessed = pData:readbool()
	local score = GlobalUserItem:readScore(pData)
	local szTip = pData:readstring()
	if bSuccessed then
		GlobalUserItem.lUserScore = score
		--通知更新        
		local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
	    eventListener.obj = yl.RY_MSG_USERWEALTH
	    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
	end
	print("Present tips " .. szTip)
end

function CheckinFrame:onSubBaseEnsureResult(pData)
	--CMD_GP_BaseEnsureResult
	print("============CheckinFrame:onSubBaseEnsureResult============")
	local bSuccess = pData:readbool()
	local lscore = GlobalUserItem:readScore(pData)
	local szTip = pData:readstring()

	if true == bSuccess then
		GlobalUserItem.lUserScore = lscore
		--通知更新        
		local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
	    eventListener.obj = yl.RY_MSG_USERWEALTH
	    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
	end
	if nil ~= self._callBack then
		self._callBack(2,szTip)
	end
end

-- 低保参数
function CheckinFrame:onSubBaseEnsureParam( pData )
	-- CMD_GP_BaseEnsureParamter
	print("============CheckinFrame:onSubBaseEnsureParam============")
	local tab = {}
	tab.condition = GlobalUserItem:readScore(pData)
	tab.amount = GlobalUserItem:readScore(pData)
	tab.times = pData:readbyte()
	if nil ~= self._callBack then
		self._callBack(CheckinFrame.BASEENSUREQUERY,tab)
	end
end

--查询签到
function CheckinFrame:sendCheckinQuery()
	print("sendCheckinQuery")
	local CheckinQuery = CCmd_Data:create(70)
	CheckinQuery:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_CHECKIN_QUERY)
	local password = md5(GlobalUserItem.szPassword)
	CheckinQuery:pushdword(GlobalUserItem.dwUserID)
	CheckinQuery:pushstring(password,33)

	--发送失败
	if not self:sendSocketData(CheckinQuery) and nil ~= self._callBack then
		self._callBack(-1,"发送查询失败！")
	end
end

--执行签到
function CheckinFrame:sendCheckinDone()
	local CheckinDone = CCmd_Data:create(136)
	CheckinDone:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_CHECKIN_DONE)
	local password = md5(GlobalUserItem.szPassword)
	CheckinDone:pushdword(GlobalUserItem.dwUserID)
	CheckinDone:pushstring(password,33)
	CheckinDone:pushstring(GlobalUserItem.szMachine,33)

	--发送失败
	if not self:sendSocketData(CheckinDone) and nil ~= self._callBack then
		self._callBack(-1,"发送签到失败！")
	end
end

--查询礼包
function CheckinFrame:sendCheckMemberGift()
	local CheckMemberGift = CCmd_Data:create(136)
	CheckMemberGift:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_MEMBER_QUERY_INFO)
	CheckMemberGift:pushdword(GlobalUserItem.dwUserID)
	CheckMemberGift:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD)
	CheckMemberGift:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)

	--发送失败
	if not self:sendSocketData(CheckMemberGift) and nil ~= self._callBack then
		self._callBack(-1,"发送查询礼包失败！")
	end
end

--获取礼包
function CheckinFrame:sendGetMemberGift()
	local GetMemberGift = CCmd_Data:create(136)
	GetMemberGift:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_MEMBER_DAY_GIFT)
	GetMemberGift:pushdword(GlobalUserItem.dwUserID)
	GetMemberGift:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD)
	GetMemberGift:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)

	--发送失败
	if not self:sendSocketData(GetMemberGift) and nil ~= self._callBack then
		self._callBack(-1,"发送获取礼包失败！")
	end
end

--获取送金
function CheckinFrame:sendGetVipPresend()
	local dataBuffer = CCmd_Data:create(136)
	dataBuffer:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_MEMBER_DAY_PRESENT)
	dataBuffer:pushdword(GlobalUserItem.dwUserID)
	dataBuffer:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD)
	dataBuffer:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)

	--发送失败
	if not self:sendSocketData(dataBuffer) and nil ~= self._callBack then
		self._callBack(-1,"发送会员送金失败！")
	end
end

--领取低保
function CheckinFrame:sendBaseEnsureTake()
	local BaseEnsureTake = CCmd_Data:create(136)
	BaseEnsureTake:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_BASEENSURE_TAKE)

	BaseEnsureTake:pushdword(GlobalUserItem.dwUserID)
	BaseEnsureTake:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD)
	BaseEnsureTake:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)

	--发送失败
	if not self:sendSocketData(BaseEnsureTake) and nil ~= self._callBack then
		self._callBack(-1,"发送领取低保失败！")
	end
end

-- 查询低保
function CheckinFrame:sendBaseEnsureLoad()
	local databuffer = CCmd_Data:create(0)
	databuffer:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_BASEENSURE_LOAD)
	--发送失败
	if not self:sendSocketData(databuffer) and nil ~= self._callBack then
		self._callBack(-1,"发送加载低保失败！")
	end
end

--查询签到
function CheckinFrame:onCheckinQuery()
	--操作记录
	self._oprateCode = CheckinFrame.QUERYCHECKIN
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end

--执行签到
function CheckinFrame:onCheckinDone()
	--操作记录
	self._oprateCode = CheckinFrame.CHECKINDONE
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end

--查询礼包
function CheckinFrame:onCheckMemberGift()
	--操作记录
	self._oprateCode = CheckinFrame.QUERYMEMBERGIFT
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end

--获取礼包
function CheckinFrame:onGetMemberGift()
	--操作记录
	self._oprateCode = CheckinFrame.GETMEMBERGIFT
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end

--领取低保
function CheckinFrame:onBaseEnsureTake()
	--操作记录
	self._oprateCode = CheckinFrame.BASEENSURETAKE
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end

-- 查询低保
function CheckinFrame:onBaseEnsureLoad()
	--操作记录
	self._oprateCode = CheckinFrame.BASEENSUREQUERY
	if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
		self._callBack(-1,"建立连接失败！")
	end
end


return CheckinFrame