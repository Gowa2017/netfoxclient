--[[
  登录界面
      2015_12_03 C.P
      功能：登录/注册
--]]

local url = require(appdf.CLIENT_SRC.."external.url")

if not yl then
	appdf.req(appdf.CLIENT_SRC.."plaza.models.yl")
end
if not GlobalUserItem then
	appdf.req(appdf.CLIENT_SRC.."plaza.models.GlobalUserItem")
end

appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.game.VoiceRecorderKit")

local targetPlatform = cc.Application:getInstance():getTargetPlatform()
local privatemgr = ""
if cc.PLATFORM_OS_WINDOWS == targetPlatform then
	privatemgr = "client/src/privatemode/plaza/src/models/PriRoom.lua"
else
	privatemgr = "client/src/privatemode/plaza/src/models/PriRoom.luac"
end
if cc.FileUtils:getInstance():isFileExist(privatemgr) then
	if not PriRoom then
		appdf.req(appdf.CLIENT_SRC.."privatemode.plaza.src.models.PriRoom")
		PriRoom:getInstance()
	end
end

local LogonScene = class("LogonScene", cc.BaseScene)

local PopWait = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.PopWait")
local QueryExit = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.QueryDialog")

local LogonFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.LogonFrame")
local LogonView = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.logon.LogonView")
local PhoneRegisterView = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.logon.PhoneRegisterView")
local AccountRegisterView = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.logon.AccountRegisterView")
local ServiceLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.other.ServiceLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
appdf.req(appdf.CLIENT_SRC.."plaza.models.FriendMgr")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local chat_cmd = appdf.req(appdf.HEADER_SRC.."CMD_ChatServer")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")

LogonScene.BT_EXIT 			= 1
LogonScene.DG_QUERYEXIT 	= 2

LogonScene.PROCESS_POPWAIT = "PROCESS_POPWAIT"
LogonScene.SHOW_POPWAIT = "SHOW_POPWAIT"
LogonScene.HIDE_POPWAIT = "HIDE_POPWAIT"

--全局处理lua错误
cc.exports.g_LuaErrorHandle = function ()
	cc.exports.bHandlePopErrorMsg = true
	if isDebug() then
		print("debug return")
		return true
	else
		print("release return")
		return false
	end
end

--加载配置
function LogonScene.onceExcute()
	local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
	--文件日志
	LogAsset:getInstance():init(MultiPlatform:getInstance():getExtralDocPath(), true, true)
	--配置微信
	MultiPlatform:getInstance():thirdPartyConfig(yl.ThirdParty.WECHAT, yl.WeChat)
	--配置支付宝
	MultiPlatform:getInstance():thirdPartyConfig(yl.ThirdParty.ALIPAY, yl.AliPay)
	--配置竣付通
	MultiPlatform:getInstance():thirdPartyConfig(yl.ThirdParty.JFT, yl.JFT)
	--配置分享
	MultiPlatform:getInstance():configSocial(yl.SocialShare)
	--配置高德
	MultiPlatform:getInstance():thirdPartyConfig(yl.ThirdParty.AMAP, yl.AMAP)
	--IOS 前后台通知
	
--[[yl.APP_NONE =			0			--初始状态
yl.APP_FOCUS =			1			--其他顶层activity在覆盖cocos2d activity
yl.APP_UNFOCUS =		2			--cocos2d activity跳转到其他层activity(暂未实现)
yl.APP_FOREGROUND =		3			--进入前台
yl.APP_BACKGROUND =		4			--进入后台--]]
	MultiPlatform:getInstance():registerActiveStatusCallBack(function (code)
		local targetPlatform = cc.Application:getInstance():getTargetPlatform()
		if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_MAC == targetPlatform) then
			local customEvent = cc.EventCustom:new(BaseConfig.RY_APPLICATION_NOTIFY)
			if (code == "Active") then
				customEvent.obj = yl.APP_FOREGROUND
				cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
			elseif (code == "ResignActive") then
				--AudioEngine.destroyInstance()
				--ccexp.AudioEngine:destroyInstance()
				customEvent.obj = yl.APP_BACKGROUND
				cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
			end
		end
	end)
end
LogonScene.onceExcute()

local FIRST_LOGIN = true
-- 进入场景而且过渡动画结束时候触发。
function LogonScene:onEnterTransitionFinish()
	if (self:hotFixCheck()) then
		return
	end
	
	if nil ~= self._logonView then
		self._logonView:onReLoadUser()
	end

	--监听消息
	local listener = cc.EventListenerCustom:create(yl.RY_CHAT_LOGIN_SUCCESS_NOTIFY,function (e)
		--MDM_GC_LOGON,SUB_GC_LOGON_SUCCESS
		if (e.obj.msgid == chat_cmd.SUB_GC_LOGON_SUCCESS) then
			if GlobalUserItem.cbLogonMode == yl.LOGON_MODE_ACCOUNTS then
				self._logonFrame:onLogonByAccount(self._szAccount,self._szPassword)
			elseif (GlobalUserItem.cbLogonMode == yl.LOGON_MODE_VISITORS) then
				if (self.isRegister) then
					self._logonFrame:onLogonByVisitor()
				else
					self.isRegister = true
				end
			end
		elseif (e.obj.msgid == chat_cmd.SUB_GC_LOGON_FAILURE) then
			local runScene = cc.Director:getInstance():getRunningScene()
			showToast(runScene, e.obj.msgtxt, 1)
			self:dismissPopWait()
		elseif (e.obj.msgid == yl.SUB_SOCKET_ERROR) then
			--local runScene = cc.Director:getInstance():getRunningScene()
			--showToast(runScene, "网络连接错误！", 1)
		end
	end)
	cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)
	
	if FIRST_LOGIN then
		FIRST_LOGIN = false
		if GlobalUserItem.bAutoLogon then
			GlobalUserItem.bVisitor = false
			GlobalUserItem.bSavePassword = true
			GlobalUserItem.bAutoLogon = true
			self:onLogon(GlobalUserItem.szAccount,GlobalUserItem.szPassword,true,true)
		end
		local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
		GlobalUserItem.m_tabOriginGameList = appConfigProxy._gameList
	end
	
    return self
end
-- 退出场景而且开始过渡动画时候触发。
function LogonScene:onExitTransitionStart()
	self._backLayer:unregisterScriptKeypadHandler()
	self._backLayer:setKeyboardEnabled(false)
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function LogonScene:onExit()
	if self._logonFrame:isSocketServer() then
		self._logonFrame:onCloseSocket()
	end
end

-- 初始化界面
function LogonScene:ctor()
	print("LogonScene:onCreate")
	self.super.ctor(self)
	
	--添加后台前台监听
	local listener = cc.EventListenerCustom:create(LogonScene.PROCESS_POPWAIT,function (event)
	    local msgWhat = event.obj
		if (msgWhat == LogonScene.SHOW_POPWAIT) then
			self:showPopWait()
		elseif (msgWhat == LogonScene.HIDE_POPWAIT) then
			self:dismissPopWait()
		end
	end)
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
	
	self._logonFrame = LogonFrame:create(self,handler(self, self.onLogonCallBack))

	self._backLayer = display.newLayer()
		:addTo(self)
	
	--返回键事件
	self._backLayer:registerScriptKeypadHandler(function(event)
		if event == "backClicked" then
			if self._popWait == nit then
				self:onButtonClickedEvent(LogonScene.BT_EXIT)
			end
		end
	end)
	self._backLayer:setKeyboardEnabled(true)

	self._topLayer = display.newLayer()
		:addTo(self)

	self._txtTips = cc.Label:createWithTTF("同步服务器信息中...", "base/fonts/round_body.ttf", 24)
		:setTextColor(cc.c4b(0,250,0,255))
		:setAnchorPoint(cc.p(1,0))
		:setVisible(false)
		:enableOutline(cc.c4b(0,0,0,255), 1)
		:move(yl.WIDTH,0)
		:addTo(self._topLayer)
		
	--版本号提示
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	local resVersion = versionProxy:getResVersion()
	local packageVersion = BaseConfig.BASE_C_VERSION
	self._txtTips = cc.Label:createWithTTF(string.format("当前版本：%d.%d", packageVersion, resVersion), "base/fonts/round_body.ttf", 24)
		:setTextColor(cc.c4b(255,250,255,255))
		:setAnchorPoint(cc.p(1,0))
		:enableOutline(cc.c4b(0,0,0,255), 1)
		:move(yl.WIDTH,0)
		:addTo(self._topLayer)

	--读取配置
	GlobalUserItem.LoadData()

	--背景音乐
	ExternalFun.playPlazzBackgroudAudio()

	-- 激活房卡
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
	GlobalUserItem.bEnableRoomCard = (appConfigProxy._serverConfig["isOpenCard"] == 0)
	--创建登录界面
	self._logonView = LogonView:create(appConfigProxy._serverConfig, self)
		:move(0,0)
		:addTo(self._backLayer)
	
	--是否为游客登录
	self.isRegister = true
end

function LogonScene:onViewLoad()	
--[[	appdf.loadImage("client/res/public/public.plist","client/res/public/public.png", function ()
		printf("public load finished\n")
	end)--]]
	
	--在没有热挂载之前，这里只能写原生方法
	local plistFileName = "client/res/public/public.plist"
	local imageFilename = "client/res/public/public.png"
	local textureCache = cc.Director:getInstance():getTextureCache()
	local spriteFrameCache = cc.SpriteFrameCache:getInstance()	
	textureCache:addImageAsync(imageFilename, function ()
		spriteFrameCache:addSpriteFrames(plistFileName, imageFilename)
	end)

	self:loadFinished()
end

function LogonScene:onWillViewEnter()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, "sound/logon_music.mp3")
	self:enterViewFinished()
end

function LogonScene:hotFixCheck()
	--获取更新代理
	local hotFixUpdateProxy = AppFacade:getInstance():retrieveProxy("HotFixUpdateProxy")
	if (hotFixUpdateProxy) then
		local tbl = hotFixUpdateProxy:getUpdateFiles()
		
		local updateCount = #tbl
		
		if (updateCount ~= 0) then
			--执行类重新挂载
			for i = 1, updateCount do
				local file = string.gsub(tbl[i], "client.src.", "")
				for k ,v in pairs(package.loaded) do
					--只重载本地Lua文件对象，不重载c++ c注册到Lua的对象
					if (string.find(k, file) ~= nil) then
						require("external.hotupdate").reload(k)
						break
					end
				end
			end
			
			hotFixUpdateProxy:setUpdateFiles(nil)
			AppFacade.restartup()
			return true
		end
	end
	
	return false
end

function LogonScene:onWillViewExit()
	self:exitViewFinished()
end

--按钮事件
function LogonScene:onButtonClickedEvent(tag,ref)
	ExternalFun.playClickEffect()
	--退出按钮
	if tag == LogonScene.BT_EXIT then
		if self:getChildByTag(LogonScene.DG_QUERYEXIT) then
			return
		end
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.QUERY_DIALOG_LAYER})
		
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"确认退出APP吗？",function(ok)
			if ok == true then
				os.exit(0)
			end
		end}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)
		
	end
end

--显示登录 bCheckAutoLogon-是否自动登录
function LogonScene:onShowLogon(bCheckAutoLogon)
	local this = self
	--取消注册界面
	if self._registerView ~= nil then
		self._registerView:runAction(cc.MoveTo:create(0.3,cc.p(-yl.WIDTH,0)))
	end

	if nil == self._logonView then
		self._logonView = LogonView:create()
			:move(yl.WIDTH,0)
			:addTo(self._backLayer)
	end
	
	--自动登录判断
	bCheckAutoLogon = false
	if not bCheckAutoLogon then
		self._logonView:runAction(
			cc.Sequence:create(
			cc.MoveTo:create(0.3,cc.p(0,0)),
			cc.CallFunc:create(function()
					this._logonView:onReLoadUser()
					end
			)))
	else
		self._logonView:runAction(
			cc.Sequence:create(
				cc.MoveTo:create(0.3,cc.p(0,0)),
				cc.CallFunc:create(function()
										this._logonView:onReLoadUser()
										local szAccount = GlobalUserItem.szAccount
										local szPassword = GlobalUserItem.szPassword
										this:onLogon(szAccount,szPassword,true,true) 
								end)
		))
			
	end
end

--显示注册界面
function LogonScene:onShowPhoneRegister()
	if self._logonView ~= nil then
		self._logonView:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
	end

	if self._serviceView ~= nil then
		self._serviceView:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
	end

	--创建注册界面
	if self._registerView == nil then
		self._registerView = PhoneRegisterView:create()
			:move(-yl.WIDTH,0)
			:addTo(self._backLayer)
	else
		self._registerView:stopAllActions()
	end
	self._registerView:runAction(cc.MoveTo:create(0.3,cc.p(0,0)))
end

--显示注册界面
function LogonScene:onShowAccountRegister()
--[[	if self._logonView ~= nil then
		self._logonView:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
	end

	if self._serviceView ~= nil then
		self._serviceView:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
	end

	--创建注册界面
	if self._registerView == nil then
		self._registerView = AccountRegisterView:create()
			:move(-yl.WIDTH,0)
			:addTo(self._backLayer)
	else
		self._registerView:stopAllActions()
	end
	self._registerView:runAction(cc.MoveTo:create(0.3,cc.p(0,0)))--]]
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.ACCOUNT_REGISTER_LAYER)
end

--显示注册网页注册界面
function LogonScene:onShowWebRegister()
	--取消登录界面
	local urlResult = url.parse(BaseConfig.WEB_HTTP_URL)
	
	local isIp = false
	for i = 1, 4 do
		local p = "%" .. i
		local c = string.gsub(urlResult.host, "(.*)%.(.*)%.(.*)%.(.*)", p)
		if (tonumber(c)) then
			isIp = true
			break
		end
	end
	
	if (isIp) then
		printf("不支持ip注册，无法绑定代理")
		return
	end
	
	local domain = string.gsub(urlResult.host, "(.*)%.(.*)%.(.*)", "%2.%3")
	
	local angent_domain = ""
	if (BaseConfig.AGENT_IDENTIFICATION ~= "") then
		angent_domain = urlResult.scheme .. "://" .. BaseConfig.AGENT_IDENTIFICATION .. "." .. domain
	else
		angent_domain = BaseConfig.WEB_HTTP_URL
	end
	
	cc.Application:getInstance():openURL(angent_domain .. "/m/register.html")
	
	return
end

--显示用户条款界面
function LogonScene:onShowService()
	--取消注册界面
	if self._registerView ~= nil then
		self._registerView:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
	end

	if self._serviceView == nil then
		self._serviceView = ServiceLayer:create()
			:move(yl.WIDTH,0)
			:addTo(self._backLayer)
	else
		self._serviceView:stopAllActions()
	end
	self._serviceView:runAction(cc.MoveTo:create(0.3,cc.p(0,0)))
end

--登录大厅
function LogonScene:onLogon(szAccount,szPassword,bSave,bAuto)
	--输入检测
	if szAccount == nil or szPassword == nil or bSave == nil or bAuto == nil then
		return
	end
	local len = ExternalFun.stringLen(szAccount)--#szAccount
	if len < 6 or len > 31 then
		showToast(self,"游戏帐号必须为6~31个字符，请重新输入!",2,cc.c4b(250,255,255,255));
		return
	end

	len = ExternalFun.stringLen(szPassword)--#szPassword
	if len<6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",2,cc.c4b(250,255,255,255));
		return
	end

	--参数记录
	self._szAccount = szAccount
	self._szPassword = szPassword
	self._bAuto = bAuto
	self._bSave = bSave

	--调用登录
	self:showPopWait()
	GlobalUserItem.cbLogonMode = yl.LOGON_MODE_ACCOUNTS

	--先短连查询UserID，在登录聊天服务器，登录聊天服务器成功后 在登录登录服务器
	local QueryData = ExternalFun.create_netdata(logincmd.CMD_MB_QueryUserData)
	QueryData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_QUERY_USERDATA)
	QueryData:pushbyte(0)
	QueryData:pushstring(szAccount, yl.LEN_ACCOUNTS)

	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, QueryData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == logincmd.SUB_GP_QUERY_USERDATA_RESULT) then
				--返回成功 登录聊天服务器
				GlobalUserItem.cbLogonMode = yl.LOGON_MODE_ACCOUNTS
				local UserID = Datatable.data:readdword()
				GlobalUserItem.dwUserID = UserID
				GlobalUserItem.szPassword = self._szPassword
				FriendMgr:getInstance():reSetAndLogin()
			elseif (Datatable.sub == logincmd.SUB_GP_OPERATE_FAILURE) then
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				--为什么用消息？异步回调禁止调用对象成员（例如self）你无法保证其生存周期
				local eventListener = cc.EventCustom:new(LogonScene.PROCESS_POPWAIT)
				eventListener.obj = LogonScene.HIDE_POPWAIT
				cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)		

				local runScene = cc.Director:getInstance():getRunningScene()
				showToast(runScene, szDescribe, 1)
			else
				local runScene = cc.Director:getInstance():getRunningScene()
				showToast(runScene, "网络连接超时！", 1)
				--为什么用消息？异步回调禁止调用对象成员（例如self）你无法保证其生存周期
				local eventListener = cc.EventCustom:new(LogonScene.PROCESS_POPWAIT)
				eventListener.obj = LogonScene.HIDE_POPWAIT
				cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)		
			end
		else
			local runScene = cc.Director:getInstance():getRunningScene()
			showToast(runScene, "网络连接失败！", 1)
			--为什么用消息？异步回调禁止调用对象成员（例如self）你无法保证其生存周期
			local eventListener = cc.EventCustom:new(LogonScene.PROCESS_POPWAIT)
			eventListener.obj = LogonScene.HIDE_POPWAIT
			cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)		
		end
	end)
end

--游客登录
function LogonScene:onVisitor()
	--播放音效
    ExternalFun.playClickEffect()
	--调用登录
	self:showPopWait()
	GlobalUserItem.cbLogonMode = yl.LOGON_MODE_VISITORS

	--先短连查询UserID，在登录聊天服务器，登录聊天服务器成功后 在登录登录服务器
	local QueryData = ExternalFun.create_netdata(logincmd.CMD_MB_QueryUserData)
	QueryData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_QUERY_USERDATA)
	QueryData:pushbyte(2)
	QueryData:pushstring(MultiPlatform:getInstance():getMachineId(), yl.LEN_MACHINE_ID)

	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, QueryData, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == logincmd.SUB_GP_QUERY_USERDATA_RESULT) then
				--返回成功 登录聊天服务器
				GlobalUserItem.cbLogonMode = yl.LOGON_MODE_VISITORS
				local UserID = Datatable.data:readdword()
				GlobalUserItem.dwUserID = UserID
				GlobalUserItem.szPassword = self._szPassword
				FriendMgr:getInstance():reSetAndLogin()
			elseif (Datatable.sub == logincmd.SUB_GP_OPERATE_FAILURE) then
				--找不到该游客账号，未注册，调用游客登录注册账号
				local lResultCode = Datatable.data:readint()
				local szDescribe = Datatable.data:readstring()
				--showToast(self, szDescribe, 1)
				self._logonFrame:onLogonByVisitor()
				self.isRegister = false
				--为什么用消息？异步回调禁止调用对象成员（例如self）你无法保证其生存周期
				local eventListener = cc.EventCustom:new(LogonScene.PROCESS_POPWAIT)
				eventListener.obj = LogonScene.HIDE_POPWAIT
				cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)		
			else
				local runScene = cc.Director:getInstance():getRunningScene()
				showToast(runScene, "网络连接超时！", 1)
				--为什么用消息？异步回调禁止调用对象成员（例如self）你无法保证其生存周期
				local eventListener = cc.EventCustom:new(LogonScene.PROCESS_POPWAIT)
				eventListener.obj = LogonScene.HIDE_POPWAIT
				cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)		
			end
		else
			local runScene = cc.Director:getInstance():getRunningScene()
			showToast(runScene, "网络连接失败！", 1)
			--为什么用消息？异步回调禁止调用对象成员（例如self）你无法保证其生存周期
			local eventListener = cc.EventCustom:new(LogonScene.PROCESS_POPWAIT)
			eventListener.obj = LogonScene.HIDE_POPWAIT
			cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)		
		end
	end)
end

--微信登陆
function LogonScene:thirdPartyLogin(plat)
	--播放音效
    ExternalFun.playClickEffect()
	
	self._tThirdData = {}
	self:showPopWait()
	self:runAction(cc.Sequence:create(cc.DelayTime:create(5), cc.CallFunc:create(function()
			self:dismissPopWait()
		end)))
	local function loginCallBack ( param )
		self:dismissPopWait()
		if type(param) == "string" and string.len(param) > 0 then
			local ok, datatable = pcall(function()
					return cjson.decode(param)
			end)
			if ok and type(datatable) == "table" then
				--dump(datatable, "微信数据", 5)
				
				local account = datatable["unionid"] or ""
				local nick = datatable["screen_name"] or ""
				self._szHeadUrl = datatable["profile_image_url"] or ""
				local gender = datatable["gender"] or "0"
				gender = tonumber(gender)
				self:showPopWait()
				GlobalUserItem.cbLogonMode = yl.LOGON_MODE_WECHAT
				self._tThirdData = 
				{
					szAccount = account,
					szNick = nick,
					cbGender = gender,
					platform = yl.PLATFORM_LIST[plat],
				}

				self._logonFrame:onLoginByThirdParty(account, nick, gender, yl.PLATFORM_LIST[plat])
			end
		end
	end
	MultiPlatform:getInstance():thirdPartyLogin(plat, loginCallBack)
end

--注册账号
function LogonScene:onRegisterByPhone(szPhoneNumber,szPassword,szVerifyCode,bAgreement,szSpreader)
	--输入检测
	if szPhoneNumber == nil or szPassword == nil or szVerifyCode == nil then
		return
	end	

	if bAgreement == false then
		showToast(self,"请先阅读并同意《游戏中心服务条款》！",2,cc.c4b(250,255,255,255));
		return
	end
	
	if false == ExternalFun.checkPhone(szPhoneNumber) then
		showToast(self, "手机号不符合规范,不能注册", 2,cc.c4b(250,255,255,255))
		return
	end

	len = ExternalFun.stringLen(szPassword)
	if len < 6 or len > 26 then
		showToast(self,"密码必须为6~26个字符，请重新输入！",2,cc.c4b(250,255,255,255));
		return
	end	
	
	--判断验证码
	local len = ExternalFun.stringLen(szVerifyCode)
    if len == 0 then
        showToast(self, "验证码不能为空", 2)
        return
    end

	-- 与帐号不同
	if string.lower(szPassword) == string.lower(szPhoneNumber) then
		showToast(self,"密码不能与手机号相同，请重新输入！",2,cc.c4b(250,255,255,255));
		return
	end

	--[[-- 首位为字母
	if 1 ~= string.find(szPassword, "%a") then
		showToast(self,"密码首位必须为字母，请重新输入！",2,cc.c4b(250,255,255,255));
		return
	end]]

	--参数记录
	self._szAccount = szPhoneNumber
	self._szPassword = szPassword
	self._bAuto = true
	self._bSave = true
	self._gender = math.random(1)
	self._verifyCode = szVerifyCode
	--调用注册
	self:showPopWait()
	GlobalUserItem.cbLogonMode = yl.LOGON_MODE_REGISTER_PHONE
	self._logonFrame:onRegisterByPhone(szPhoneNumber,szPassword, szVerifyCode, self._gender,szSpreader)
end

--注册账号
function LogonScene:onRegisterByAccount(szAccount,szPassword,szRePassword,bAgreement,szSpreader)
	--输入检测
	if szAccount == nil or szPassword == nil or szRePassword == nil then
		return
	end	

	if bAgreement == false then
		showToast(self,"请先阅读并同意《游戏中心服务条款》！",2,cc.c4b(250,255,255,255));
		return
	end
	local len = ExternalFun.stringLen(szAccount)--#szAccount
	if len < 6 or len > 31 then
		showToast(self,"游戏帐号必须为6~31个字符，请重新输入！",2,cc.c4b(250,255,255,255));
		return
	end

	--判断emoji
    if ExternalFun.isContainEmoji(szAccount) then
        showToast(self, "帐号包含非法字符,请重试", 2)
        return
    end

	--判断是否有非法字符
	if true == ExternalFun.isContainBadWords(szAccount) then
		showToast(self, "帐号中包含敏感字符,不能注册", 2)
		return
	end

	len = ExternalFun.stringLen(szPassword)
	if len < 6 or len > 26 then
		showToast(self,"密码必须为6~26个字符，请重新输入！",2,cc.c4b(250,255,255,255));
		return
	end	
	
	if szPassword ~= szRePassword then
		showToast(self,"二次输入密码不一致，请重新输入！",2,cc.c4b(250,255,255,255));
        return
    end

	-- 与帐号不同
	if string.lower(szPassword) == string.lower(szAccount) then
		showToast(self,"密码不能与帐号相同，请重新输入！",2,cc.c4b(250,255,255,255));
		return
	end
	

	-- 首位为字母
	if 1 ~= string.find(szAccount, "%a") then
		showToast(self,"账号首位必须为字母，请重新输入！",2,cc.c4b(250,255,255,255));
		return
	end
	
	-- 含有中文
	local lenInByte = string.len(szAccount)
	local chinese_count = 0
	local index = 0
	while index < lenInByte do
		index = index + 1
		
		local curByte = string.byte(szAccount, index)
		local byteCount = 1;
		if curByte>0 and curByte<=127 then
			byteCount = 1
		elseif curByte>=192 and curByte<223 then
			byteCount = 2
			chinese_count = chinese_count + 1
		elseif curByte>=224 and curByte<239 then
			byteCount = 3
			chinese_count = chinese_count + 1
		elseif curByte>=240 and curByte<=247 then
			byteCount = 4
			chinese_count = chinese_count + 1
		end
		 
		local char = string.sub(szAccount, index, index+byteCount-1)
		index = index + byteCount -1
	end
	
	if (chinese_count ~= 0) then
		showToast(self,"账号不能包含中文，请重新输入！",2,cc.c4b(250,255,255,255));
		return
	end

	--参数记录
	self._szAccount = szAccount
	self._szPassword = szPassword
	self._bAuto = true
	self._bSave = true
	self._gender = math.random(1)
	self._verifyCode = szVerifyCode
	--调用注册
	self:showPopWait()
	GlobalUserItem.cbLogonMode = yl.LOGON_MODE_REGISTER_ACCOUNTS	
	self._logonFrame:onRegisterByAccount(szAccount,szPassword, szVerifyCode, self._gender,szSpreader)
end

--登录注册回调
function LogonScene:onLogonCallBack(result,message)
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
	if result == 1 then --成功
		--发送地理位置
		FriendMgr:getInstance():updateLocation()
		--本地保存
		if GlobalUserItem.cbLogonMode == yl.LOGON_MODE_VISITORS then 					--游客登录
			GlobalUserItem.bAutoLogon = false
			GlobalUserItem.bSavePassword = false
			GlobalUserItem.szPassword = "WHYK@foxuc.cn"
			if (self.isRegister == false) then
				--登录聊天服务器
				FriendMgr:getInstance():reSetAndLogin()
			end
			--GlobalUserItem.szAccount = GlobalUserItem.szNickName
		elseif GlobalUserItem.cbLogonMode == yl.LOGON_MODE_WECHAT then 				--微信登陆
			GlobalUserItem.szThirdPartyUrl = self._szHeadUrl
			GlobalUserItem.szPassword = "WHYK@foxuc.cn"
			GlobalUserItem.bThirdPartyLogin = true
			GlobalUserItem.thirdPartyData = self._tThirdData
			--GlobalUserItem.szAccount = GlobalUserItem.szNickName
		else
			GlobalUserItem.bAutoLogon = self._bAuto
			GlobalUserItem.bSavePassword = self._bSave
			GlobalUserItem.onSaveAccountConfig()
		end

		if yl.HTTP_SUPPORT then
			local ostime = os.time()
           
           local ss=GlobalUserItem:getSignature(ostime)
			appdf.onHttpJsionTable(BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=GetMobileShareConfig&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime),function(jstable,jsdata)
             
				--LogAsset:getInstance():logData("action=GetMobileShareConfig&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime))
				--dump(jstable, "GetMobileShareConfig", 6)
				local msg = nil
				if type(jstable) == "table" then
					local data = jstable["data"]
					msg = jstable["msg"]
					if type(data) == "table" then
						local valid = data["valid"]
						if valid then
							local count = data["FreeCount"] or 0
							GlobalUserItem.nTableFreeCount = tonumber(count)
							local sharesend = data["SharePresent"] or 0
							GlobalUserItem.nShareSend = tonumber(sharesend)

							-- 微信平台推广链接
							--GlobalUserItem.szWXShareURL = data["ShareUrl"]
							GlobalUserItem.setShareUrl(data["ShareUrl"])
							GlobalUserItem.szWXShareTitle = data["ShareTitle"]
							GlobalUserItem.szWXShareContent	= data["ShareContent"]
							GlobalUserItem.szEarnShareContent = data["EarnShareContent"]
							--整理代理游戏列表
							if table.nums(self._logonFrame.m_angentServerList) > 0 then
								self:arrangeGameList(self._logonFrame.m_angentServerList)
							else
								local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
								appConfigProxy._gameList = GlobalUserItem.m_tabOriginGameList
							end

							-- 每日必做列表
							GlobalUserItem.tabDayTaskCache = {}
							local dayTask = data["DayTask"]
							if type(dayTask) == "table" then
								for k,v in pairs(dayTask) do
									if tonumber(v) == 0 then
										GlobalUserItem.tabDayTaskCache[k] = 1
										GlobalUserItem.bEnableEveryDay = true
									end
								end
							end
							GlobalUserItem.bEnableCheckIn = (GlobalUserItem.tabDayTaskCache["Field1"] ~= nil)
							GlobalUserItem.bEnableTask = (GlobalUserItem.tabDayTaskCache["Field6"] ~= nil)
							
							-- 邀请送金
							local sendcount = data["RegGold"]
							GlobalUserItem.nInviteSend = tonumber(sendcount) or 0
							
							self:dismissPopWait()
							--进入游戏列表
							AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.ACCOUNT_REGISTER_LAYER})
							AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {transition = "FADE", time = 1}, VIEW_LIST.CLIENT_SCENE)
							--self:getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.ClientScene","FADE",1)
							return
						end
					end
				end
				self:dismissPopWait()
				local str = "游戏登陆异常"
				if type(msg) == "string" then
					str = str .. ":" .. msg
				end
				showToast(self, str, 3, cc.c3b(250,0,0))
			end)
		else
			--整理代理游戏列表
			if table.nums(self._logonFrame.m_angentServerList) > 0 then
				self:arrangeGameList(self._logonFrame.m_angentServerList)
			else
				appConfigProxy._gameList = GlobalUserItem.m_tabOriginGameList
			end

			--进入游戏列表
			self:dismissPopWait()
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.ACCOUNT_REGISTER_LAYER})
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {transition = "FADE", time = 1}, VIEW_LIST.CLIENT_SCENE)
			--self:getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.ClientScene","FADE",1)
		end		
	elseif result == -1 then --失败
		self:dismissPopWait()
		if type(message) == "string" and message ~= "" then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {message,nil, true, 1}, canrepeat = false}, VIEW_LIST.QUERY_DIALOG_LAYER)	
		end
	elseif result == 10 then --重复绑定
		if self._logonView ~= nil and nil ~= self._logonView.refreshBtnList then
			self._logonView:refreshBtnList()
		end
	end
end

--显示等待
function LogonScene:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function LogonScene:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})	
end

--整理游戏列表
--[[
serverList = 
{
	KindID = {KindID, SortID},
	KindID2 = {KindID2, SortID},
}
]]
function LogonScene:arrangeGameList(serverList)	
	local originList = GlobalUserItem.m_tabOriginGameList
	local newList = {}
	for k,v in pairs(originList) do
		local serverGame = serverList[tonumber(v._KindID)]
		if nil ~= serverGame then
			v._SortId = serverGame.SortID
			table.insert(newList, v)
		end
	end
	table.sort(newList,	function(a, b)
		return a._SortId > b._SortId
	end)
	
	local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")
	appConfigProxy._gameList = newList
end

return LogonScene