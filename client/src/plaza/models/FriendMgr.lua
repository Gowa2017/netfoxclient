

-- local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
-- local g_var = ExternalFun.req_var
local chat_cmd = appdf.req(appdf.HEADER_SRC.."CMD_ChatServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local cjson = appdf.req("cjson")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")
local PopWait = appdf.req(appdf.BASE_SRC.."app.views.layer.other.PopWait")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
local FriendQueryDialog = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.friend.FriendQueryDialog")

-- FriendMgr
FriendMgr = class("FriendMgr")
FriendMgr.instance = nil

--好友文件存放路径
local FRIEND_PATH = device.writablePath .. "/friend/"
--我的好友
local MYFRIEND_FILE_PATH = FRIEND_PATH
--聊天信息本地存储最大数量
local MAX_CHAT_CACHE = 20

function FriendMgr:getInstance()
    if FriendMgr.instance == nil then
        FriendMgr.instance = FriendMgr:create()
    end
    return FriendMgr.instance
end

function FriendMgr:create()
    local obj= FriendMgr.new()
    obj:init()
    return obj
end

function FriendMgr:init()
	--界面层
	self.m_viewLayer = nil

	self.m_Delegate = nil;
	if self.m_Socket then
		self.m_Socket:relaseSocket()
	end

	self.m_Socket = nil
    self.m_CallBack = nil;
	self.m_FriendTab = {}

	--加载框
	self._popWait = nil

	--断线自动重连次数
	self.m_nAutoLoginCount = 5

	--聊天缓存
	self.m_tabChatCache = {}
	self.m_tabChatCache[GlobalUserItem.dwUserID] = {}
	--通知缓存
	self.m_tabNoticeCache = {}
	self.m_tabNoticeCache[GlobalUserItem.dwUserID] = {}
	--配置好友本地文件存储目录
	if false == cc.FileUtils:getInstance():isDirectoryExist(FRIEND_PATH) then
		cc.FileUtils:getInstance():createDirectory(FRIEND_PATH)
	end
	--配置自己好友文件存储目录
	MYFRIEND_FILE_PATH = FRIEND_PATH .. GlobalUserItem.dwUserID .. "/"
	if false == cc.FileUtils:getInstance():isDirectoryExist(MYFRIEND_FILE_PATH) then
		cc.FileUtils:getInstance():createDirectory(MYFRIEND_FILE_PATH)
	end

	--数据缓存队列(用户缓存网络异常时未发送的数据)
	self.m_sendCache = {}
	--最后收到的喇叭消息
	self.m_lastTrumpetData = {}
	
	--注册监听
	self:registerNotifyList()
	-- 是否请求更新坐标
	self.m_bRequestUpdateLocation = false
	-- 是否请求附近好友
	self.m_bQueryNearUserInfo = false
end

function FriendMgr:registerNotifyList()
	--聊天
	local notify = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_CHAT_NOTIFY)
	notify.name = "friend_chat"
	notify.fun = handler(self,self.onNotify)
	notify.pause = true
	NotifyMgr:getInstance():registerNotify(notify)
	--申请好友通知
	local notify2 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_APPLYFOR_NOTIFY)
	notify2.name = "friend_apply"
	notify2.fun = handler(self,self.onNotify)
	notify2.pause = true
	NotifyMgr:getInstance():registerNotify(notify2)
	--回应通知
	local notify3 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_RESPOND_NOTIFY)
	notify3.name = "friend_response"
	notify3.fun = handler(self,self.onNotify)
	notify3.pause = true
	NotifyMgr:getInstance():registerNotify(notify3)
	--邀请通知
	local notify4 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_GAME_NOTIFY)
	notify4.name = "friend_invite"
	notify4.fun = handler(self,self.onNotify)
	notify4.pause = true
	NotifyMgr:getInstance():registerNotify(notify4)
	--私人房邀请
	local notify5 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_PERSONAL_NOTIFY)
	notify5.name = "pri_friend_invite"
	notify5.fun = handler(self,self.onNotify)
	notify5.pause = true
	NotifyMgr:getInstance():registerNotify(notify5)
	--分享通知
	local notify6 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_SHARE_NOTIFY)
	notify6.name = "friend_share"
	notify6.fun = handler(self,self.onNotify)
	notify6.pause = true
	NotifyMgr:getInstance():registerNotify(notify6)
end

function FriendMgr:setCallBackDelegate(delegate,callback)
	--self.m_Delegate = delegate
	--self.m_CallBack = callback;
end

function FriendMgr:setViewLayer(layer)
	self.m_viewLayer = layer
end

--通知消息回调
function FriendMgr:onNotify(msg)
	if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.onNotify then
		return self.m_viewLayer:onNotify(msg)
	end
end

--网络消息回调
function FriendMgr:onSocketCallBack(pData)
	--无效数据
	if pData == nil or nil == self.m_Socket then 
		return
	end
	-- 连接命令
	local main = pData:getmain()
	local sub =pData:getsub()
	--作为消息通知的参数
	local netdata = nil
	
	if main == yl.MAIN_SOCKET_INFO then 		--网络状态
		if sub == yl.SUB_SOCKET_CONNECT then
			self:onConnectCompeleted()
		elseif sub == yl.SUB_SOCKET_ERROR then	--网络错误			
			self:onSocketError(pData)
		else
			self:onCloseSocket()
		end
	else		
		------------------
		-----单独数据处理
		------------------
		if main == chat_cmd.MDM_GC_LOGON  then
			if sub == chat_cmd.SUB_S_USER_FRIEND then  --好友消息
				self:onSocketEvent(main,sub,pData);
				return;
			end
		elseif main == chat_cmd.MDM_GC_USER then 
			if sub == chat_cmd.SUB_GC_USER_STATUS_NOTIFY then  --好友上线、下线
				self:onSocketEvent(main,sub,pData);
				return;
			end
		end

		if self.m_Delegate then			
			if self.m_CallBack then
				self.m_CallBack(self.m_Delegate,main,sub,pData);
			else
				netdata = self.m_Delegate:onSocketEvent(main,sub,pData);
			end
		else
			netdata = self:onSocketEvent(main,sub,pData);
		end
	end

	--通知处理
	NotifyMgr:getInstance():excute(main, sub, netdata)	
end

--连接结果
function FriendMgr:onConnectCompeleted()
	self.m_Socket:setdelaytime(0)
	self.m_Socket:setwaittime(0.5)

	-- 发送登录
    self:login()
end

function FriendMgr:getLocationInfo()
	MultiPlatform:getInstance():requestLocation(function(result)
		if type(result) == "string" and "" ~= result then
            local ok, tabCoordinate = pcall(function()
                return cjson.decode(result)
            end)
            GlobalUserItem.tabCoordinate.lo = 181.0
        	GlobalUserItem.tabCoordinate.la = 91.0
            if ok then
                dump(tabCoordinate, "FriendMgr:updateLocation", 6)
                -- 是否有错误
                local berror = tabCoordinate["berror"] or false
                if berror then
                	local msg = tabCoordinate["msg"] or ""
                	local runScene = cc.Director:getInstance():getRunningScene()
                	if nil ~= runScene then
                		--showToast(runScene, msg, 2)
                		print(msg)
                	end
                else
                	-- 经度
	                local lo = tonumber(tabCoordinate["longitude"]) or 181.0
	                -- 纬度
	                local la = tonumber(tabCoordinate["latitude"]) or 91.0
	                GlobalUserItem.tabCoordinate.lo = lo
        			GlobalUserItem.tabCoordinate.la = la
                end

                if not GlobalUserItem.bUpdateCoordinate and self.m_bRequestUpdateLocation then
                	self.m_bRequestUpdateLocation = false
                	self:updateLocation()
                end
            end
        end
	end)
end

-- 更新位置
function FriendMgr:updateLocation()
	self.m_bRequestUpdateLocation = true
	if GlobalUserItem.noCoordinateData() then
		self:getLocationInfo()
	else
		-- 更新坐标
        self:sendUpdateCoordinate(GlobalUserItem.tabCoordinate.lo, GlobalUserItem.tabCoordinate.la)
	end
end

-- 设置sleep
function FriendMgr:setWaitTime( var )
	if nil ~= self.m_Socket then
		self.m_Socket:setwaittime(var)
	end
end

--网络错误
function FriendMgr:onSocketError(pData)	
	self:onCloseSocket()
	if not pData then
		print("网络断开！")
	elseif type(pData) == "string" then
		print(pData)
	else
		local errorcode = pData:readword()
		if errorcode == nil then
			print("网络断开！")
		elseif errorcode == 3 then
			print("网络连接超时, 请重试!")
			-- 切换地址
			if nil ~= yl.SERVER_LIST[yl.CURRENT_INDEX] then
				yl.LOGONSERVER = yl.SERVER_LIST[yl.CURRENT_INDEX]
			end
			yl.CURRENT_INDEX = yl.CURRENT_INDEX + 1
			if yl.CURRENT_INDEX > yl.TOTAL_COUNT then
				yl.CURRENT_INDEX = 1
			end
		else
			print("网络错误，code："..errorcode)			
		end
	end
end

--网络信息
function FriendMgr:onSocketEvent(main,sub,pData)
	print("============FriendMgr:onSocketEvent============")
	print("*socket event:"..main.."#"..sub) 
	--作为消息通知的参数
	local netdata = nil
	--self:dismissPopWait()

	if main == chat_cmd.MDM_GC_LOGON  then	 				--用户信息
		if sub == chat_cmd.SUB_S_USER_FRIEND then  			--用户好友			
			local wFriendCount = pData:readword();
			assert(pData:getlen() ~= wFriendCount*102 +2,"--chat_cmd.tagClientFriendInfo-结构不匹配-")
			local getTab = {}; 
			for i=1,wFriendCount do
				getTab[#getTab+1] = ExternalFun.read_netdata(chat_cmd.tagClientFriendInfo, pData);
			end
			--如果好友列表中已經存在該用戶，更新該用戶信息
			for i=1,#getTab do				
				local isExit =  false;
				for n=1,#self.m_FriendTab do					
					if getTab[i].dwUserID == self.m_FriendTab[n].dwUserID then						
						isExit = true
						for k,v in pairs(getTab[i]) do					
							self.m_FriendTab[n].k = v 
							break;
						end
					end
				end

				if not isExit then
					table.insert(self.m_FriendTab, getTab[i])
				end
			end
			dump(self.m_FriendTab, "self.m_FriendTab", 3)
			table.sort(self.m_FriendTab, function(a, b)
		        return a.cbMainStatus > b.cbMainStatus
		    end)
			--刷新好友列表
			if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.refreshFriendList then
				self.m_viewLayer:refreshFriendList()
			end
		elseif sub == chat_cmd.SUB_GC_LOGON_SUCCESS then 	-- 登陆成功
			self:updateLocation()
		end
	elseif main == chat_cmd.MDM_GC_USER then  				--用户命令
		netdata = self:onFriendMessage(sub, pData)
	end

	self:popCacheMessage()
	return netdata
end

function FriendMgr:onFriendMessage(sub,pData)
	local netdata = nil

	if sub == chat_cmd.SUB_GC_USER_STATUS_NOTIFY then  			--好友状态(上下线)
		local userStatus =  ExternalFun.read_netdata(chat_cmd.CMD_GC_UserOnlineStatusNotify, pData)
		local userInfo = nil
		for k,v in pairs(self.m_FriendTab) do
			if v.dwUserID == userStatus.dwUserID then
				print("好友状态 ==> " .. userStatus.cbMainStatus)		
				v.cbMainStatus = userStatus.cbMainStatus
				netdata = userStatus
				userInfo = v
				break
			end			
		end
		table.sort(self.m_FriendTab, function(a, b)
	        return a.cbMainStatus > b.cbMainStatus
	    end)
		--刷新好友列表
		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.refreshFriendList then
			self.m_viewLayer:refreshFriendList()
		end
	elseif sub == chat_cmd.SUB_GC_GAME_STATUS_NOTIFY then  		--游戏状态							
		local userStatus =  ExternalFun.read_netdata(chat_cmd.CMD_GC_UserGameStatusNotify, pData)
		local userInfo = nil
		for k,v in pairs(self.m_FriendTab) do
			if v.dwUserID == userStatus.dwUserID then
				print("游戏状态 ==> " .. userStatus.cbGameStatus)
				v.cbGameStatus = userStatus.cbGameStatus
				v.wServerID = userStatus.wServerID
				v.wTableID = userStatus.wTableID
				v.wKindID = userStatus.wKindID
				v.szServerName = userStatus.szServerName
				userInfo = v
				break
			end
		end
		if nil ~= userInfo then
			if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.refreshFriendState then
				self.m_viewLayer:refreshFriendState(userInfo, true)
			end
		end		
	elseif sub == chat_cmd.SUB_GC_USER_CHAT_NOTIFY  then 		--聊天通知
		local notify =  ExternalFun.read_netdata(chat_cmd.CMD_GC_UserChat, pData)
		dump(notify, "===================聊天消息=====================")
		self:insertUserMsg(notify.dwSenderID,notify.dwSenderID,notify.dwTargetUserID,notify.szMessageContent)
		netdata = notify

		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.messageNotify then
			self.m_viewLayer:messageNotify(notify)
		end
	elseif sub == chat_cmd.SUB_GC_APPLYFOR_NOTIFY then 			--申请通知
		local notify =  ExternalFun.read_netdata(chat_cmd.CMD_GC_ApplyForNotify, pData);
		self:insertFriendNotify(1,notify)

		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.updateNotifyList then
			self.m_viewLayer:updateNotifyList()
		end	
	elseif sub == chat_cmd.SUB_GC_RESPOND_NOTIFY then  			--回应通知
		local szNotifyContent = pData:readstring()
		self:insertFriendNotify(2,szNotifyContent);

		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.updateNotifyList then
			self.m_viewLayer:updateNotifyList()
		end		
	elseif sub == chat_cmd.SUB_GC_SEARCH_USER_RESULT then 		--查找通知/查找结果
		--chat_cmd.CMD_GC_SearchByGameIDResult
		local cbUserCount = pData:readbyte()
	    local userTab =  {}
		for i=1,cbUserCount do
			userTab[i] =  ExternalFun.read_netdata(chat_cmd.tagClientFriendInfo, pData)
		end
		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.searchResult then
			self.m_viewLayer:searchResult(userTab)
		end	
	elseif sub == chat_cmd.SUB_GC_INVITE_GAME_NOTIFY then 		--邀请通知
	    local notify =  ExternalFun.read_netdata(chat_cmd.CMD_GC_InviteGameNotify, pData)
	    dump(notify, "SUB_GC_INVITE_GAME_NOTIFY", 3)
		self:insertUserMsg(notify.dwSenderID,notify.dwSenderID,GlobalUserItem.dwUserID,notify.szInviteMsg)
		netdata = notify
		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.messageNotify then
			self.m_viewLayer:messageNotify(notify)
		end
		if nil ~= self.m_query and nil ~= self.m_query:getParent() then
			self.m_query:removeFromParent()
		end
		local runScene = cc.Director:getInstance():getRunningScene()
		if nil ~= runScene then
			local list = self:getFriendList()
			local nick = ""
			for k,v in pairs(list) do
				if v.dwUserID == notify.dwSenderID then
					nick = v.szNickName
					break
				end
			end
			local msgTab = {}
			msgTab.main = "您的好友 " .. nick .. " @您一起游戏"
			msgTab.sub = notify.szInviteMsg
			self.m_query = FriendQueryDialog:create(msgTab, function(ok)
					
				end, nil, FriendQueryDialog.QUERY_SURE)
                :setCanTouchOutside(false)
                :addTo(runScene)
		end
	elseif sub == chat_cmd.SUB_GC_INVITE_PERSONAL_NOTIFY then --私人房邀请通知
		local notify =  ExternalFun.read_netdata(chat_cmd.CMD_GC_InvitePersonalGameNotify, pData)
		dump(notify, "SUB_GC_INVITE_GAME_NOTIFY", 3)
	    self:insertUserMsg(notify.dwSenderID,notify.dwSenderID,GlobalUserItem.dwUserID,notify.szInviteMsg)
		netdata = notify
		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.messageNotify then
			self.m_viewLayer:messageNotify(notify)
		end
		if nil ~= self.m_query and nil ~= self.m_query:getParent() then
			self.m_query:removeFromParent()
		end
		local runScene = cc.Director:getInstance():getRunningScene()
		if nil ~= runScene then
			local list = self:getFriendList()
			local nick = ""
			for k,v in pairs(list) do
				if v.dwUserID == notify.dwSenderID then
					nick = v.szNickName
					break
				end
			end
			local msgTab = {}
			msgTab.main = "您的好友 " .. nick .. " @您一起约战"
			msgTab.sub = notify.szInviteMsg
			self.m_query = FriendQueryDialog:create(msgTab, function(ok)
					if ok then
						showToast(runScene, "房间ID复制成功!", 1)
						GlobalUserItem.szCopyRoomId = string.format("%06d", notify.dwServerNumber)
					end
					self.m_query = nil
				end, nil, FriendQueryDialog.QUERY_SURE_CANCEL)
                :setCanTouchOutside(false)
                :addTo(runScene)
		end
	elseif sub == chat_cmd.SUB_GC_TRUMPET_NOTIFY then 			--喇叭通知
		local data_tab = ExternalFun.read_netdata(chat_cmd.CMD_GC_Trumpet_S, pData)
		netdata = data_tab
	elseif sub == chat_cmd.SUB_GC_DELETE_FRIEND_NOTIFY then 	--删除通知
		local data_tab = ExternalFun.read_netdata(chat_cmd.CMD_GC_DeleteFriendNotify, pData)
		local idx = nil
		for k,v in pairs(self.m_FriendTab) do
			if v.dwUserID == data_tab.dwFriendUserID then
				idx = k
				break
			end
		end
		if nil ~= idx then
			table.remove(self.m_FriendTab,idx)
		end

		--刷新好友列表
		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.refreshFriendList then
			self.m_viewLayer:refreshFriendList()
		end
	elseif sub == chat_cmd.SUB_GC_MODIFY_FRIEND_NOTIFY then 	--好友修改结果

	elseif sub == chat_cmd.SUB_GC_MODIFY_GROUP_NOTIFY then 		--用户组修改结果

	elseif sub == chat_cmd.SUB_GC_UPDATE_COORDINATE_NOTIFY then --更新坐标
		local cmd_table = ExternalFun.read_netdata(chat_cmd.CMD_GC_Update_CoordinateNotify, pData)
		dump(cmd_table, "CMD_GC_Update_CoordinateNotify", 6)
		GlobalUserItem.bUpdateCoordinate = true
		if self.m_bQueryNearUserInfo then
    		self.m_bQueryNearUserInfo = false
    		self:sendQueryNearUser()
    	end
	elseif sub == chat_cmd.SUB_GC_GET_NEARUSER_RESULT then 		-- 附近结果
		local cmd_table = ExternalFun.read_netdata(chat_cmd.CMD_GC_Get_NearuserResult, pData)
		local tablist = {}
		for i = 1, cmd_table.cbUserCount do
			local nearuser = ExternalFun.read_netdata(chat_cmd.tagNearUserInfo, pData)
			-- 过滤自己
			if nearuser.dwUserID ~= GlobalUserItem.dwUserID then
				table.insert(tablist, nearuser)
			end
		end
		-- 刷新列表
		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.refreshNearFriendList then
			self.m_viewLayer:refreshNearFriendList(tablist)
		end
	elseif sub == chat_cmd.SUB_GC_QUERY_NEARUSER_RESULT then 	-- 查询结果
		local cmd_table = ExternalFun.read_netdata(chat_cmd.CMD_GC_Query_NearuserResult, pData)
		dump(cmd_table, "CMD_GC_Query_NearuserResult", 6)
		if 1 == cmd_table.cbUserCount then
			--通知更新   
			local eventListener = cc.EventCustom:new(yl.RY_NEARUSER_NOTIFY)
	    	eventListener.msg = cmd_table.NearUserInfo
	    	cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
		end
	elseif sub == chat_cmd.SUB_GC_QUERY_NEARUSER_ECHO 
		or sub == chat_cmd.SUB_GC_UPDATE_COORDINATE_ECHO then  	-- 坐标更新反馈
		local data_tab = ExternalFun.read_netdata(chat_cmd.CMD_GC_ECHO, pData)
		print(data_tab.szDescribeString)
		self:dismissPopWait()
	elseif sub == chat_cmd.SUB_GC_USER_SHARE_NOTIFY then 		-- 分享通知
		local notify =  ExternalFun.read_netdata(chat_cmd.CMD_GC_UserShareNotify, pData)
	    dump(notify, "notify", 6)
	    self:insertUserMsg(notify.dwSenderID,notify.dwSenderID,GlobalUserItem.dwUserID,notify.szMessageContent)
		netdata = notify
		-- 下载图片
		if type(notify.szShareImageAddr) == "string"  and "" ~= notify.szShareImageAddr then
			local path = self:downloadShareImage(notify.szShareImageAddr)
			self:insertUserMsg(notify.dwSenderID, notify.dwSenderID, GlobalUserItem.dwUserID, path, true)
		end
		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.messageNotify then
			self.m_viewLayer:messageNotify(notify)
		end
	else 														--所有反馈处理/操作结果处理
		self:showEcho(sub,pData)
	end

	return netdata
end

--显示反馈
function FriendMgr:showEcho(sub,pData)
	if sub == chat_cmd.SUB_GC_OPERATE_SUCCESS then 			--操作成功
		local data_tab = ExternalFun.read_netdata(chat_cmd.CMD_GC_OperateSuccess, pData)
		dump(data_tab, "success")
	elseif sub == chat_cmd.SUB_GP_OPERATE_FAILURE then 		--操作失败
		local data_tab = ExternalFun.read_netdata(chat_cmd.CMD_GC_OperateFailure, pData)
		dump(data_tab, "fail")
		local runScene = cc.Director:getInstance():getRunningScene()
		if nil ~= runScene then
			showToast(runScene,data_tab.szDescribeString,2)
		end
	else 													--通用反馈
		self:showFriendEcho(sub, pData)
	end
end

function FriendMgr:showFriendEcho(sub, pData)
	local data_tab = ExternalFun.read_netdata(chat_cmd.CMD_GC_ECHO, pData)
	print(data_tab.szDescribeString)
	if data_tab.lErrorCode ~= 0 and nil ~= self.m_viewLayer then
		showToast(self.m_viewLayer,data_tab.szDescribeString,2)
	end

	--更新喇叭库存
	if sub == chat_cmd.SUB_GC_TRUMPET_ECHO and data_tab.lErrorCode == 0 then
		GlobalUserItem.nLargeTrumpetCount = GlobalUserItem.nLargeTrumpetCount - 1
		GlobalUserItem.nLargeTrumpetCount = (GlobalUserItem.nLargeTrumpetCount < 0) and 0 or GlobalUserItem.nLargeTrumpetCount

		--通知数量变更
		local event = cc.EventCustom:new(yl.TRUMPET_COUNT_UPDATE_NOTIFY)
		cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
	end
end

function FriendMgr:contentAndLogin()
	self.m_Socket = CClientSocket:createSocket(function(pData)
		self:onSocketCallBack(pData)
	end)

	if self.m_Socket:connectSocket(yl.LOGONSERVER,yl.FRIENDPORT, yl.VALIDATE) then
		--todo
	end
end

--好友登陆
function FriendMgr:login()
	print("FriendMgr send login")
	local friendData = CCmd_Data:create()
	friendData:setcmdinfo(chat_cmd.MDM_GC_LOGON,chat_cmd.SUB_GC_MB_LOGON_USERID)
	friendData:pushdword(GlobalUserItem.dwUserID);
	friendData:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD);
	friendData:pushstring("手机型号",chat_cmd.LEN_PHONE_MODE);

	if not self:sendSocketData(friendData) then
		print("FriendMgr:login 登录好友系统失败！！！")
		return
	end

	--配置自己好友文件存储目录
	MYFRIEND_FILE_PATH = FRIEND_PATH .. GlobalUserItem.dwUserID .. "/"
	if false == cc.FileUtils:getInstance():isDirectoryExist(MYFRIEND_FILE_PATH) then
		cc.FileUtils:getInstance():createDirectory(MYFRIEND_FILE_PATH)
	end
end

--发送聊天
function FriendMgr:sendMessageFriend(userTab)
	--dump(userTab, "======= FriendMgr:sendMessageFriend========");

	local sendMsgData = CCmd_Data:create();
	sendMsgData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_USER_CHAT);
	sendMsgData:pushdword(userTab.dwUserID);
	sendMsgData:pushdword(userTab.dwTargetUserID);
	sendMsgData:pushdword(userTab.dwFontColor);
	sendMsgData:pushbyte(userTab.cbFontSize);
	sendMsgData:pushbyte(userTab.cbFontAttri);
	sendMsgData:pushstring(userTab.szFontName,chat_cmd.LEN_FONT_NAME);
	sendMsgData:pushstring(userTab.szMessageContent,chat_cmd.LEN_MESSAGE_CONTENT);

	if not self:sendSocketData(sendMsgData) then
		print("发送聊天消息失败！");
	else
		self:insertUserMsg(userTab.dwTargetUserID,userTab.dwUserID,userTab.dwTargetUserID,userTab.szMessageContent);

		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.messageNotify then
			self.m_viewLayer:messageNotify(notify)
		end
	end
end

--添加好友/申请好友
function FriendMgr:sendAddFriend(userTab,func)	
	local addFriendData = CCmd_Data:create();
	addFriendData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_APPLYFOR_FRIEND);
	addFriendData:pushdword(userTab.dwUserID);
	addFriendData:pushdword(userTab.dwFriendID);
	addFriendData:pushbyte(userTab.cbGroupID);

	if not self:sendSocketData(addFriendData) then
		print("发送添加好友失败！");
		func(false);
	else
		func(true);
	end	
end

--回应好友/好友回应
function FriendMgr:sendRespondFriend(userTab,notifyId)
	dump(userTab,"=============")

	local respondData = CCmd_Data:create();
	respondData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_RESPOND_FRIEND);
	respondData:pushdword(userTab.dwUserID);
	respondData:pushdword(userTab.dwRequestID);
	respondData:pushbyte(0);
	respondData:pushbyte(0);
	respondData:pushbool(userTab.bAccepted);

	if not self:sendSocketData(respondData) then
		print("发送回应好友失败！");
	else
		self:markFriendNotifyRead(notifyId);
		if nil ~= self.m_viewLayer and nil ~= self.m_viewLayer.updateNotifyList then	
			self.m_viewLayer:updateNotifyList()
		end
	end
end

--查找好友/查找用户
function FriendMgr:sendSearchFriend(dwuserid)	
	print("查找 " .. dwuserid)
	local searchData = ExternalFun.create_netdata(chat_cmd.CMD_GC_SearchByGameID) --CCmd_Data:create(chat_cmd.CMD_GC_SearchByGameID);
	searchData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_SEARCH_USER);
	searchData:pushdword(dwuserid);

	if not self:sendSocketData(searchData) then
		print("发送查找好友失败！");
	end	
end

--邀请游戏
function FriendMgr:sendInviteGame(msgTab)
	local sendMsgData = ExternalFun.create_netdata(chat_cmd.CMD_GC_InviteGame)
	sendMsgData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_INVITE_GAME)
	sendMsgData:pushdword(GlobalUserItem.dwUserID)
	sendMsgData:pushdword(msgTab.dwInvitedUserID)
	sendMsgData:pushword(msgTab.wKindID)
	sendMsgData:pushword(msgTab.wServerID)
	sendMsgData:pushword(msgTab.wTableID)
	sendMsgData:pushstring(msgTab.szInviteMsg, 128)

	if not self:sendSocketData(sendMsgData) then
		print("发送邀请失败！");
		sendMsgData:retain()
		table.insert(self.m_sendCache, sendMsgData)
	else
		-- 内容
		self:insertUserMsg(msgTab.dwInvitedUserID, GlobalUserItem.dwUserID, msgTab.dwInvitedUserID, msgTab.szInviteMsg)
	end
end

-- 私人房邀请游戏
function FriendMgr:sendInvitePrivateGame(msgTab)
	local sendMsgData = ExternalFun.create_netdata(chat_cmd.CMD_GC_InvitePersonalGame)
	sendMsgData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_INVITE_PERSONAL)
	sendMsgData:pushdword(GlobalUserItem.dwUserID)
	sendMsgData:pushdword(msgTab.dwInvitedUserID)
	sendMsgData:pushword(msgTab.wKindID)
	sendMsgData:pushdword(msgTab.wServerNumber)
	sendMsgData:pushword(msgTab.wTableID)
	sendMsgData:pushstring(msgTab.szInviteMsg, 128)

	if not self:sendSocketData(sendMsgData) then
		print("发送邀请失败！");
		sendMsgData:retain()
		table.insert(self.m_sendCache, sendMsgData)
	else
		-- 内容
		self:insertUserMsg(msgTab.dwInvitedUserID, GlobalUserItem.dwUserID, msgTab.dwInvitedUserID, msgTab.szInviteMsg)
		return true
	end
end

--发送喇叭/用户喇叭
function FriendMgr:sendTrupmet( msgTab )
	dump(msgTab, "======= FriendMgr:sendTrupmet========");

	local sendMsgData = ExternalFun.create_netdata(chat_cmd.CMD_GC_Trumpet_C)
	sendMsgData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_TRUMPET)
	sendMsgData:pushword(306)
	sendMsgData:pushdword(msgTab.dwSenderID)
	sendMsgData:pushdword(msgTab.dwFontColor)
	sendMsgData:pushstring(msgTab.szNickName,yl.LEN_NICKNAME)
	sendMsgData:pushstring(msgTab.szMessageContent,chat_cmd.LEN_MESSAGE_CONTENT)

	if not self:sendSocketData(sendMsgData) then
		print("发送喇叭失败！");
		sendMsgData:retain()
		table.insert(self.m_sendCache, sendMsgData)
	end
end

--删除好友
function FriendMgr:sendDeleteFriend(friendid, groupid)
	local sendMsgData = ExternalFun.create_netdata(chat_cmd.CMD_GC_DeleteFriend)
	sendMsgData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_DELETE_FRIEND)
	sendMsgData:pushdword(GlobalUserItem.dwUserID)
	sendMsgData:pushdword(friendid)
	sendMsgData:pushbyte(groupid)

	if not self:sendSocketData(sendMsgData) then
		print("发送删除失败！");
		sendMsgData:retain()
		table.insert(self.m_sendCache, sendMsgData)
	end
end

-- 发送更新坐标
function FriendMgr:sendUpdateCoordinate(lLongitude, lLatitude)
	local sendMsgData = ExternalFun.create_netdata(chat_cmd.CMD_GC_Update_Coordinate)
	sendMsgData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_UPDATE_COORDINATE)
	sendMsgData:pushdword(GlobalUserItem.dwUserID)
	sendMsgData:pushdouble(lLongitude)
	sendMsgData:pushdouble(lLatitude)

	if not self:sendSocketData(sendMsgData) then
		print("发送更新坐标失败！");
		sendMsgData:retain()
		table.insert(self.m_sendCache, sendMsgData)
	end
end

-- 查询附近好友
function FriendMgr:queryNearUser()
	self.m_bRequestUpdateLocation = true
	self.m_bQueryNearUserInfo = true
	GlobalUserItem.bUpdateCoordinate = false
	self:showPopWait()
	self:getLocationInfo()
end

-- 发送查询附近
function FriendMgr:sendQueryNearUser()
	local sendMsgData = ExternalFun.create_netdata(chat_cmd.CMD_GC_Get_Nearuser)
	sendMsgData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_GET_NEARUSER)
	sendMsgData:pushdword(GlobalUserItem.dwUserID)
	sendMsgData:pushdouble(GlobalUserItem.tabCoordinate.lo)
	sendMsgData:pushdouble(GlobalUserItem.tabCoordinate.la)
	if not self:sendSocketData(sendMsgData) then
		print("发送查询附近失败！");
		sendMsgData:retain()
		table.insert(self.m_sendCache, sendMsgData)
	end
end

-- 发送指定用户查询
function FriendMgr:sendQueryUserLocation( dwTargetUserID )
	print("FriendMgr:sendQueryUserLocation")
	local sendMsgData = ExternalFun.create_netdata(chat_cmd.CMD_GC_Query_Nearuser)
	sendMsgData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_QUERY_NEARUSER)
	sendMsgData:pushdword(GlobalUserItem.dwUserID)
	sendMsgData:pushdword(dwTargetUserID)
	if not self:sendSocketData(sendMsgData) then
		print("发送查询位置失败！");
		sendMsgData:retain()
		table.insert(self.m_sendCache, sendMsgData)
	end
end

-- 发送分享
function FriendMgr:sendShareMessage(msgTab)
	local sendMsgData = ExternalFun.create_netdata(chat_cmd.CMD_GC_UserShare)
	sendMsgData:setcmdinfo(chat_cmd.MDM_GC_USER,chat_cmd.SUB_GC_USER_SHARE)
	sendMsgData:pushdword(GlobalUserItem.dwUserID)
	sendMsgData:pushdword(msgTab.dwSharedUserID)
	sendMsgData:pushstring(msgTab.szShareImageAddr, chat_cmd.LEN_MESSAGE_CONTENT)
	sendMsgData:pushstring(msgTab.szMessageContent, chat_cmd.LEN_MESSAGE_CONTENT)

	if not self:sendSocketData(sendMsgData) then
		print("发送分享失败！");
		--sendMsgData:retain()
		--table.insert(self.m_sendCache, sendMsgData)
		return false
	else
		-- 内容
		self:insertUserMsg(msgTab.dwSharedUserID, GlobalUserItem.dwUserID, msgTab.dwSharedUserID, msgTab.szMessageContent)
		-- 图片
		self:insertUserMsg(msgTab.dwSharedUserID, GlobalUserItem.dwUserID, msgTab.dwSharedUserID, msgTab.szImagePath, true)
		return true
	end
end

--发送数据
function FriendMgr:sendSocketData(pData)
	if self.m_Socket == nil then
		return false
	end
	--self:showPopWait()
	if not self.m_Socket:sendData(pData) then
		--self:dismissPopWait()
		self:onCloseSocket()
		return false
	end
	return true
end

--关闭网络
function FriendMgr:onCloseSocket()
	if self.m_Socket then
		self.m_Socket:relaseSocket()
		self.m_Socket = nil
	end

	--控制自动连接次数
	if self.m_nAutoLoginCount > 0 then
		print("auto connect")
		self.m_nAutoLoginCount = self.m_nAutoLoginCount - 1
		self.m_nAutoLoginCount = (self.m_nAutoLoginCount >= 0) and self.m_nAutoLoginCount or 0
		self:contentAndLogin()
	else
		print("auto connect ==> 0")
	end	
end

--判断是否有网络连接
function FriendMgr:isConnected(  )
	return nil ~= self.m_Socket
end

--重置并进行网络连接
function FriendMgr:reSetAndLogin( )
	--聊天缓存
	self.m_tabChatCache[GlobalUserItem.dwUserID] = {}
	--通知缓存
	self.m_tabNoticeCache[GlobalUserItem.dwUserID] = {}

	self.m_nAutoLoginCount = 5
	self:contentAndLogin()
	-- 获取位置
    self:getLocationInfo()
end

--重置并断开网络连接
function FriendMgr:reSetAndDisconnect(  )
	self.m_nAutoLoginCount = 0
	self:onCloseSocket()
	--清空好友数据
	self.m_FriendTab = {}
	self:setCallBackDelegate(nil, nil)
	--清空缓存
	for k,v in pairs(self.m_sendCache) do
		v:release()
	end
	self.m_sendCache = {}	
	GlobalUserItem.bUpdateCoordinate = false
	self.m_bRequestUpdateLocation = false
	self.m_bQueryNearUserInfo = false
end

--处理缓存未发送消息
function FriendMgr:popCacheMessage()
	--处理未发送的缓存数据
	for k,v in pairs(self.m_sendCache) do
		if self:sendSocketData(v) then
			print("cache send")
			v:release()
			self.m_sendCache[k] = nil
		end
	end
end

----------------
----好友数据处理
----------------
--缓存游戏通知
function FriendMgr:insertFriendNotify(Type,notifyTab)
	local filePath = MYFRIEND_FILE_PATH .. "FriendNotify.ry";
	if cc.FileUtils:getInstance():isFileExist(filePath) then			
		local data = cc.FileUtils:getInstance():getStringFromFile(filePath)
		local ok, dataTab = pcall(function()
			return cjson.decode(data)
		end)
		local isExist = false
		if true == ok then
			--去除重复消息
			for k,v in pairs(dataTab) do
				if Type == v.notifyType 
					and notifyTab.dwRequestID == v.notify.dwRequestID
					and not v.bRead then
					isExist = true
					break
				end
			end
		else
			dataTab = {}
		end

		if not isExist then
			-- 1:申请好友通知  2:回应通知  3：邀请通知  
			table.insert(dataTab,{notifyType = Type,notify = notifyTab, notifyId = os.time()})

			--控制容量
			if #dataTab > MAX_CHAT_CACHE then
				table.remove(dataTab,1)
			end

			self.m_tabNoticeCache[GlobalUserItem.dwUserID] = dataTab
			cc.FileUtils:getInstance():writeStringToFile(cjson.encode(dataTab),filePath);
		end		
	else
		self.m_tabNoticeCache[GlobalUserItem.dwUserID] = {{notifyType = Type ,notify =notifyTab, notifyId = os.time()}}
		cc.FileUtils:getInstance():writeStringToFile(cjson.encode(self.m_tabNoticeCache[GlobalUserItem.dwUserID]),filePath)
	end
end

--获取消息通知
function FriendMgr:getFriendNotify()
	local filePath = MYFRIEND_FILE_PATH .. "FriendNotify.ry";

	if nil == self.m_tabNoticeCache[GlobalUserItem.dwUserID] or 
		#self.m_tabNoticeCache[GlobalUserItem.dwUserID] == 0 then
		self.m_tabNoticeCache[GlobalUserItem.dwUserID] = {}
		if cc.FileUtils:getInstance():isFileExist(filePath) then			
			local data = cc.FileUtils:getInstance():getStringFromFile(filePath);
			local ok, dataTab =  pcall(function()
				return cjson.decode(data)
			end)
			if true == ok then
				self.m_tabNoticeCache[GlobalUserItem.dwUserID] = dataTab
			end
		end
	end
	table.sort(self.m_tabNoticeCache[GlobalUserItem.dwUserID], function(a, b)
		if a.bRead == true and b.bRead == true then
			return a.notifyId < b.notifyId
		else
			return (a.bRead == true)
		end
	end)
	return self.m_tabNoticeCache[GlobalUserItem.dwUserID]	
end

--获取未读通知
function FriendMgr:getUnReadNotify()
	local list = self:getFriendNotify()
	local unread = {}
	for i = 1, #list do
		if nil == list[i].bRead or false == list[i].bRead then
			table.insert(unread,list[i])
		end
	end
	return unread
end

--刪除消息通知
function FriendMgr:deleteFriendNotify(notifyId)
	local filePath = MYFRIEND_FILE_PATH .. "FriendNotify.ry";

	if cc.FileUtils:getInstance():isFileExist(filePath) then			
		local data = cc.FileUtils:getInstance():getStringFromFile(filePath)
		local ok, dataTab =  pcall(function()
			return cjson.decode(data)
		end)
		if true == ok then
			local idx = 1
			for k,v in pairs(dataTab) do
				if v.notifyId == notifyId then
					idx = k
					break
				end
			end
			table.remove(dataTab,idx)
			self.m_tabNoticeCache[GlobalUserItem.dwUserID] = dataTab
			cc.FileUtils:getInstance():writeStringToFile(cjson.encode(dataTab),filePath)
		end		
	end
end

--标记消息已读
function FriendMgr:markFriendNotifyRead(notifyId)
	local filePath = MYFRIEND_FILE_PATH .. "FriendNotify.ry";

	if cc.FileUtils:getInstance():isFileExist(filePath) then			
		local data = cc.FileUtils:getInstance():getStringFromFile(filePath)
		local ok, dataTab =  pcall(function()
			return cjson.decode(data)
		end)
		if true == ok then
			for k,v in pairs(dataTab) do
				if v.notifyId == notifyId then
					v.bRead = true
					--break
				end
			end
			--table.remove(dataTab,index)
			self.m_tabNoticeCache[GlobalUserItem.dwUserID] = dataTab
			cc.FileUtils:getInstance():writeStringToFile(cjson.encode(dataTab),filePath)
		end		
	end
end

--删除好友
function FriendMgr:deleteFriend(index)
	table.remove(self.m_FriendTab,index);
	if self.m_Delegate then		
		if self.m_Delegate.refreshFriendList then
			self.m_Delegate:refreshFriendList();
		end
	end
end

--获取缓存好友列表
function FriendMgr:getFriendList()
	return self.m_FriendTab or {};
end

-- 获取好友
function FriendMgr:getFriendByID( dwID )
	for k,v in pairs(self.m_FriendTab) do
		if v.dwUserID == dwID then
			return v
		end
	end
	return nil
end

--获取好友信息
function FriendMgr:getFriendInfoByID(dwUserID)
	if nil == dwUserID then
		return
	end
	for k,v in pairs(self.m_FriendTab) do
		if v.dwUserID == dwUserID then
			return v
		end
	end
	return nil
end

--插入一条聊天信息
function FriendMgr:insertUserMsg(userID, sendID, targetID, content, bImage)	
	local filePath = MYFRIEND_FILE_PATH .. tostring(userID)..".ry";

	if cc.FileUtils:getInstance():isFileExist(filePath) then			
		local data = cc.FileUtils:getInstance():getStringFromFile(filePath);
		local ok, dataTab = pcall(function()
			return cjson.decode(data)
		end)
		if true == ok then
			bImage = bImage or false
			table.insert(dataTab,{dwSenderID = sendID, dwTargetUserID = targetID, szMessageContent = content, bImage = bImage});

			--控制容量
			if #dataTab > MAX_CHAT_CACHE then
				local msg = dataTab[1]
				if msg.bImage then
					print("FriendMgr: removeChatImage")
					cc.FileUtils:getInstance():removeFile(msg.szMessageContent)
				end
				table.remove(dataTab,1)
			end
			self.m_tabChatCache[GlobalUserItem.dwUserID][userID] = dataTab
			cc.FileUtils:getInstance():writeStringToFile(cjson.encode(dataTab),filePath)
			return
		end
	end
	self.m_tabChatCache[GlobalUserItem.dwUserID][userID] = {{dwSenderID = sendID,dwTargetUserID = userID ,szMessageContent =content}}
	cc.FileUtils:getInstance():writeStringToFile(cjson.encode(self.m_tabChatCache[GlobalUserItem.dwUserID][userID]),filePath);
end

--获取用户聊天信息记录
function FriendMgr:getUserRecordMsg(userID)
	if nil == self.m_tabChatCache[GlobalUserItem.dwUserID][userID] or 0 == #self.m_tabChatCache[GlobalUserItem.dwUserID][userID] then
		local filePath = MYFRIEND_FILE_PATH .. tostring(userID)..".ry"
		self.m_tabChatCache[GlobalUserItem.dwUserID][userID] = {}
		if cc.FileUtils:getInstance():isFileExist(filePath) then		
			local data = cc.FileUtils:getInstance():getStringFromFile(filePath)
			local ok, dataTab = pcall(function()
				return cjson.decode(data)
			end)
			if true == ok then
				self.m_tabChatCache[GlobalUserItem.dwUserID][userID] = dataTab
			end
		end
	end
	return self.m_tabChatCache[GlobalUserItem.dwUserID][userID]
end

--显示等待
function FriendMgr:showPopWait(msg)
	local runScene = cc.Director:getInstance():getRunningScene()
	if nil == self._popWait and nil ~= runScene then
		self._popWait = PopWait:create()
		self._popWait:show(runScene,msg)			
	end
end

--关闭等待
function FriendMgr:dismissPopWait()
	if self._popWait then
		self._popWait:dismiss()
		self._popWait = nil
	end
end

local IMAGE_DOWNLOAD_NOTIFY = "friend_image_download_notify"
--全局通知函数
cc.exports.g_imageDownloadListener = function (ncode, msg, filename)
	print(msg)
	local event = cc.EventCustom:new(yl.RY_IMAGE_DOWNLOAD_NOTIFY)
	event.code = ncode
	event.msg = msg
	event.filename = filename

	cc.Director:getInstance():getEventDispatcher():dispatchEvent(event)
end
-- 下载分享图片
function FriendMgr:downloadShareImage( imageurl )
	local path = device.writablePath
	local downloader = CurlAsset:createDownloader("g_imageDownloadListener",imageurl)			
	if false == cc.FileUtils:getInstance():isDirectoryExist(path) then
		cc.FileUtils:getInstance():createDirectory(path)
	end
	downloader:downloadFile(path, "/shareimage.png")
	return path .. "/shareimage.png"
end