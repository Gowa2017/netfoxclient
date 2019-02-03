--[[
  登录界面
      2015_12_03 C.P
      功能：登录/注册
--]]
if not yl then
	appdf.req(appdf.CLIENT_SRC.."plaza.models.yl")
end
if not GlobalUserItem then
	appdf.req(appdf.CLIENT_SRC.."plaza.models.GlobalUserItem")
end

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

local LogonScene = class("LogonScene", cc.load("mvc").ViewBase)

local PopWait = appdf.req(appdf.BASE_SRC.."app.views.layer.other.PopWait")
local QueryExit = appdf.req(appdf.BASE_SRC.."app.views.layer.other.QueryDialog")

local LogonFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.LogonFrame")
local LogonView = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.logon.LogonView")
local RegisterView = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.logon.RegisterView")
local ServiceLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.other.ServiceLayer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
appdf.req(appdf.CLIENT_SRC.."plaza.models.FriendMgr")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

LogonScene.BT_EXIT 			= 1
LogonScene.DG_QUERYEXIT 	= 2

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
end
LogonScene.onceExcute()

local FIRST_LOGIN = true
-- 进入场景而且过渡动画结束时候触发。
function LogonScene:onEnterTransitionFinish()
	if nil ~= self._logonView then
		self._logonView:onReLoadUser()
	end
	-- 默认登陆游戏信息
	if #self:getApp()._gameList > 0 then
		local info = self:getApp()._gameList[1]
		GlobalUserItem.nCurGameKind = tonumber(info._KindID)
	end
		
	if FIRST_LOGIN then
		FIRST_LOGIN = false

		if GlobalUserItem.bAutoLogon then
			GlobalUserItem.bVisitor = false
			self:onLogon(GlobalUserItem.szAccount,GlobalUserItem.szPassword,true,true)
		end
		GlobalUserItem.m_tabOriginGameList = self:getApp()._gameList
	end
    return self
end
-- 退出场景而且开始过渡动画时候触发。
function LogonScene:onExitTransitionStart()
	self._backLayer:unregisterScriptKeypadHandler()
	self._backLayer:setKeyboardEnabled(false)
    return self
end

-- 初始化界面
function LogonScene:onCreate()
	print("LogonScene:onCreate")
	local this = self

	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			this:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			this:onExitTransitionStart()
		elseif eventType == "exit" then
			if self._logonFrame:isSocketServer() then
				self._logonFrame:onCloseSocket()
			end
		end
	end)

	local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    local logonCallBack = function (result,message)
		this:onLogonCallBack(result,message)
	end

	self._logonFrame = LogonFrame:create(self,logonCallBack)

	self._backLayer = display.newLayer()
		:addTo(self)
	
	--返回键事件
	self._backLayer:registerScriptKeypadHandler(function(event)
		if event == "backClicked" then
			if this._popWait == nit then
				this:onButtonClickedEvent(LogonScene.BT_EXIT)
			end
		end
	end)
	self._backLayer:setKeyboardEnabled(true)

	self._topLayer = display.newLayer()
		:addTo(self)

	--背景
    display.newSprite("background_1.jpg")
        :move(yl.WIDTH/2,yl.HEIGHT/2)
        :addTo(self._backLayer)
 
 	--平台logo
	display.newSprite("Logon/logon_logo.png")
		:move(yl.WIDTH/2,yl.HEIGHT-150)
		:addTo(self._backLayer)
		:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH/2,yl.HEIGHT-150)))

	--返回
	if  device.platform ~= "mac" and device.platform ~= "ios" then
		ccui.Button:create("bt_return_0.png","bt_return_1.png")
			:move(75,yl.HEIGHT-51)
			:setTag(LogonScene.BT_EXIT)
			:addTo(self._backLayer)
			:addTouchEventListener(btcallback)
	end

	self._txtTips = cc.Label:createWithTTF("同步服务器信息中...", "fonts/round_body.ttf", 24)
		:setTextColor(cc.c4b(0,250,0,255))
		:setAnchorPoint(cc.p(1,0))
		:setVisible(false)
		:enableOutline(cc.c4b(0,0,0,255), 1)
		:move(yl.WIDTH,0)
		:addTo(self._topLayer)

	--读取配置
	GlobalUserItem.LoadData()

	--背景音乐
	ExternalFun.playPlazzBackgroudAudio( )

	-- 激活房卡
	GlobalUserItem.bEnableRoomCard = (self:getApp()._serverConfig["isOpenCard"] == 0)
	--创建登录界面
	self._logonView = LogonView:create(self:getApp()._serverConfig)
		:move(0,0)
		:addTo(self._backLayer)
end

--按钮事件
function LogonScene:onButtonClickedEvent(tag,ref)
	--退出按钮
	if tag == LogonScene.BT_EXIT then
			local a =  Integer64.new()

	print(a:getstring())
	
		if self:getChildByTag(LogonScene.DG_QUERYEXIT) then
			return
		end
		QueryExit:create("确认退出APP吗？",function(ok)
				if ok == true then
					os.exit(0)
				end
			end)
			:setTag(LogonScene.DG_QUERYEXIT)
			:addTo(self)
		
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
function LogonScene:onShowRegister()
	--取消登录界面
	if self._logonView ~= nil then
		self._logonView:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
	end

	if self._serviceView ~= nil then
		self._serviceView:runAction(cc.MoveTo:create(0.3,cc.p(yl.WIDTH,0)))
	end

	--创建注册界面
	if self._registerView == nil then
		self._registerView = RegisterView:create()
			:move(-yl.WIDTH,0)
			:addTo(self._backLayer)
	else
		self._registerView:stopAllActions()
	end
	self._registerView:runAction(cc.MoveTo:create(0.3,cc.p(0,0)))
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
		showToast(self,"游戏帐号必须为6~31个字符，请重新输入!",2,cc.c4b(250,0,0,255));
		return
	end

	len = ExternalFun.stringLen(szPassword)--#szPassword
	if len<6 then
		showToast(self,"密码必须大于6个字符，请重新输入！",2,cc.c4b(250,0,0,255));
		return
	end

	--参数记录
	self._szAccount = szAccount
	self._szPassword = szPassword
	self._bAuto = bAuto
	self._bSave = bSave

	--调用登录
	self:showPopWait()
	self._Operate = 0
	self._logonFrame:onLogonByAccount(szAccount,szPassword)
end

--游客登录
function LogonScene:onVisitor()
	--调用登录
	self:showPopWait()
	self._Operate = 2
	self._logonFrame:onLogonByVisitor()
end

--微信登陆
function LogonScene:thirdPartyLogin(plat)
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
				self._Operate = 3
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
function LogonScene:onRegister(szAccount,szPassword,szRePassword,bAgreement,szSpreader)
	--输入检测
	if szAccount == nil or szPassword == nil or szRePassword == nil then
		return
	end	

	if bAgreement == false then
		showToast(self,"请先阅读并同意《游戏中心服务条款》！",2,cc.c4b(250,0,0,255));
		return
	end
	local len = ExternalFun.stringLen(szAccount)--#szAccount
	if len < 6 or len > 31 then
		showToast(self,"游戏帐号必须为6~31个字符，请重新输入！",2,cc.c4b(250,0,0,255));
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
		showToast(self,"密码必须为6~26个字符，请重新输入！",2,cc.c4b(250,0,0,255));
		return
	end	

	if szPassword ~= szRePassword then
		showToast(self,"二次输入密码不一致，请重新输入！",2,cc.c4b(250,0,0,255));
		return
	end

	-- 与帐号不同
	if string.lower(szPassword) == string.lower(szAccount) then
		showToast(self,"密码不能与帐号相同，请重新输入！",2,cc.c4b(250,0,0,255));
		return
	end

	--[[-- 首位为字母
	if 1 ~= string.find(szPassword, "%a") then
		showToast(self,"密码首位必须为字母，请重新输入！",2,cc.c4b(250,0,0,255));
		return
	end]]

	--参数记录
	self._szAccount = szAccount
	self._szPassword = szPassword
	self._bAuto = true
	self._bSave = true
	self._gender = math.random(1)
	--调用注册
	self:showPopWait()
	self._Operate = 1
	self._logonFrame:onRegister(szAccount,szPassword, self._gender,szSpreader)
end

--登录注册回调
function LogonScene:onLogonCallBack(result,message)
	if result == 1 then --成功
		--本地保存
		if self._Operate == 2 then 					--游客登录
			GlobalUserItem.bAutoLogon = false
			GlobalUserItem.bSavePassword = false
			GlobalUserItem.szPassword = "WHYK@foxuc.cn"
			--GlobalUserItem.szAccount = GlobalUserItem.szNickName
		elseif self._Operate == 3 then 				--微信登陆
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
			appdf.onHttpJsionTable(yl.HTTP_URL .. "/WS/MobileInterface.ashx","GET","action=GetMobileShareConfig&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime),function(jstable,jsdata)
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

							--推广链接
							GlobalUserItem.szSpreaderURL = data["SpreaderUrl"]
							if nil == GlobalUserItem.szSpreaderURL or "" == GlobalUserItem.szSpreaderURL then
								GlobalUserItem.szSpreaderURL = yl.HTTP_URL ..  "/Mobile/Register.aspx"
							else
								GlobalUserItem.szSpreaderURL = string.gsub(GlobalUserItem.szSpreaderURL, " ", "")
							end
							-- 微信平台推广链接
							GlobalUserItem.szWXSpreaderURL = data["WxSpreaderUrl"]
							if nil == GlobalUserItem.szWXSpreaderURL or "" == GlobalUserItem.szWXSpreaderURL then
								GlobalUserItem.szWXSpreaderURL = yl.HTTP_URL ..  "/Mobile/Register.aspx"
							else
								GlobalUserItem.szWXSpreaderURL = string.gsub(GlobalUserItem.szWXSpreaderURL, " ", "")
							end

							--整理代理游戏列表
							if table.nums(self._logonFrame.m_angentServerList) > 0 then
								self:arrangeGameList(self._logonFrame.m_angentServerList)
							else
								self:getApp()._gameList = GlobalUserItem.m_tabOriginGameList
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

							--进入游戏列表
							self:getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.ClientScene","FADE",1)
							FriendMgr:getInstance():reSetAndLogin()
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
				self:getApp()._gameList = GlobalUserItem.m_tabOriginGameList
			end

			--进入游戏列表
			self:getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.ClientScene","FADE",1)
			--FriendMgr:getInstance():reSetAndLogin()
		end		
	elseif result == -1 then --失败
		self:dismissPopWait()
		if type(message) == "string" and message ~= "" then
			showToast(self._topLayer,message,2,cc.c4b(250,0,0,255));
		end
	elseif result == 10 then --重复绑定
		if self._logonView ~= nil and nil ~= self._logonView.refreshBtnList then
			self._logonView:refreshBtnList()
		end
	end
end

--显示等待
function LogonScene:showPopWait()
	if not self._popWait  then
		self._popWait = PopWait:create()
			:show(self._topLayer,"请稍候！")
	end
end

--关闭等待
function LogonScene:dismissPopWait()
	if self._popWait ~= nil then
		self._popWait:dismiss()
		self._popWait = nil
	end
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
	self:getApp()._gameList = newList
end

return LogonScene