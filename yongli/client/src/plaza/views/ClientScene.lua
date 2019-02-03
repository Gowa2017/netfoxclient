--[[
	手游大厅界面
	2015_12_03 C.P
]]

local ClientScene = class("ClientScene", cc.BaseScene)
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local PopWait = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.PopWait")

--local GameListView	= appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.GameListLayer")

local RoomList = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.RoomListLayer")

local UserInfo = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.UserInfoLayer")

local Bank = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.BankLayer")

local RankingList = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.RankingListLayer")

local Task = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.TaskLayer")

local EveryDay = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.EveryDayLayer")


local Checkin = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.CheckinLayer")
local CheckinFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.CheckinFrame")
local LevelFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.LevelFrame")
local ShopDetailFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ShopDetailFrame")
local TaskFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.TaskFrame")

local BankRecord = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.BankRecordLayer")
local CashRecord = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.CashRecordLayer")

local OrderRecord = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.OrderRecordLayer")

local Agent = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.AgentLayer")

local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

local ShopDetail = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopDetailLayer")

local Bag = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.BagLayer")

local BagDetail = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.BagDetailLayer")

local BagTrans = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.BagTransLayer")

local Binding = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.BindingLayer")

local Friend = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.friend.FriendLayer")

local TableLayer = appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.plaza.TableLayer")

local FeedbackLayer = appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.plaza.FeedbackLayer")

local FaqLayer = appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.plaza.FaqLayer")

local GameFrameEngine = appdf.req(appdf.CLIENT_SRC.."plaza.models.GameFrameEngine")

local Room = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.room.RoomLayer")

local TrumpetSendLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.friend.TrumpetSendLayer")

local BindingRegisterLayer = appdf.req(appdf.CLIENT_SRC .. "plaza.views.layer.plaza.BindingRegisterLayer")

local HeadSprite = appdf.req(appdf.EXTERNAL_SRC .. "HeadSprite")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local chat_cmd = appdf.req(appdf.HEADER_SRC.."CMD_ChatServer")
local GameChatLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.game.GameChatLayer")

local PromoterInputLayer = import(".layer.plaza.PromoterInputLayer")
local ActivityLayer = import(".layer.plaza.ActivityLayer")
local EarnMoney = import(".layer.plaza.EarnMoney")

ClientScene.BT_EXIT 		= 1
ClientScene.BT_CONFIG 		= 2
ClientScene.BT_USER 		= 3
ClientScene.BT_RECHARGE		= 4
ClientScene.BT_SHOP			= 6
ClientScene.BT_ACTIVITY		= 7
ClientScene.BT_RECORD		= 8
ClientScene.BT_BANK			= 9
ClientScene.BT_BAG			= 10
ClientScene.BT_FRIEND		= 11
ClientScene.BT_TASK			= 12
ClientScene.BT_RANK			= 13
ClientScene.BT_QUICKSTART	= 14
ClientScene.BT_EXCHANGE		= 15
ClientScene.BT_BOX			= 16
ClientScene.BT_CHECKIN		= 17
ClientScene.BT_TRUMPET		= 18
ClientScene.BT_PERSON		= 19
ClientScene.BT_MOREMENU 	= 20
ClientScene.BT_SHARE 		= 21
ClientScene.BT_EARN			= 22
ClientScene.BT_MATCH 		= 23
ClientScene.BT_PRIBRAND 	= 24
ClientScene.BT_KEFU 		= 25
ClientScene.BT_KAIHU 		= 26
ClientScene.BT_GONGGAO		= 27
ClientScene.BT_INSURE_ADD	= 28
ClientScene.BT_BEAN		= 29
ClientScene.BT_RULE		= 30


local HELP_LAYER_NAME = "__introduce_help_layer__"
local HELP_BTN_NAME = "__introduce_help_button__"
local VOICE_BTN_NAME = "__voice_record_button__"
local VOICE_LAYER_NAME = "__voice_record_layer__"

local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")

--兼容性代码（需要添加修改BaseConfig文件的参数时）
if (not BaseConfig.BASE_C_BINVERSION or BaseConfig.BASE_C_BINVERSION == 1) then
	BaseConfig.ONLY_APPSTORE_GAME = {389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400}
end

-- 初始化界面
function ClientScene:ctor()
	self.super.ctor(self)
	self:cachePublicRes()
	--self.m_actBoxAni = nil
	--self.m_actStartBtnAni = nil
	--self.m_actChargeBtnAni = nil
		
	--注册触摸事件
	ExternalFun.registerTouchEvent(self, true)
	
	--保存进入的游戏记录信息
	GlobalUserItem.m_tabEnterGame = nil
	--喇叭发送界面
	self.m_trumpetLayer = nil	
	GlobalUserItem.bHasLogon = true
	self._gameFrame = GameFrameEngine:create(nil, function (code,result)
		self:onRoomCallBack(code,result)
	end)	
	
	if PriRoom then
		PriRoom:getInstance():onEnterPlaza(self, self._gameFrame)
	end
	
	local btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    -- 加载csb资源
	local csbPath = ""
	if (appConfigProxy._appStoreSwitch == 0) then
		csbPath = "plaza/PlazzLayer.csb"
	else
		csbPath = "plaza/PlazzLayer_" .. BaseConfig.AGENT_IDENTIFICATION .. ".csb"
		if (not cc.FileUtils:getInstance():isFileExist(csbPath)) then
			csbPath = "plaza/PlazzLayer.csb"
		end		
	end
	
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)

	self.m_plazaAni = self._csbNodeAni
	
	self._sceneLayer = display.newLayer()
		:setContentSize(yl.WIDTH,yl.HEIGHT)
		:addTo(csbNode)

	--返回键事件
	self._sceneLayer:registerScriptKeypadHandler(function(event)
		if event == "backClicked" then
			 if self._popWait == nil then
			 	if #self._sceneRecord > 0 then
				 	--游戏屏蔽返回键
				 	local curLayerTag = self._sceneRecord[#self._sceneRecord]
					if (curLayerTag == yl.SCENE_GAME) then
						return
					end

					local cur_layer =  self._sceneLayer:getChildByTag(curLayerTag)
					if cur_layer  and cur_layer.onKeyBack then
						if cur_layer:onKeyBack() == true then
							return
						end
					end
				end
				self:onKeyBack()
			end
		end
	end)
	self._sceneLayer:setKeyboardEnabled(true)
	
  
	--客服按钮
	local btKefu = ExternalFun.seekWigetByNameEx(csbNode, "btn_kefu")
	for _,v in ipairs(btKefu) do
		v:setTag(ClientScene.BT_KEFU)
		v:addTouchEventListener(btcallback)
	end

    --大厅下拉多功能菜单
	local btMoreMenu = ExternalFun.seekWigetByNameEx(csbNode, "btn_moremenu")
	for _,v in ipairs(btMoreMenu) do
		v:setTag(ClientScene.BT_MOREMENU)
		v:addTouchEventListener(btcallback)
	end
	
    --设置按钮
	local btSetting = ExternalFun.seekWigetByNameEx(csbNode, "Button_Setup")
	for _,v in ipairs(btSetting) do
		v:setTag(ClientScene.BT_CONFIG)
		v:addTouchEventListener(btcallback)
	end	
	
    --签到按钮
	local btSign = ExternalFun.seekWigetByNameEx(csbNode, "Button_Checkin")
	for _,v in ipairs(btSign) do
		v:setTag(ClientScene.BT_CHECKIN)
		v:addTouchEventListener(btcallback)
	end
	
	--约牌
	local btpribrand = ExternalFun.seekWigetByNameEx(csbNode, "btn_brand")
	for _,v in ipairs(btpribrand) do
		v:setTag(ClientScene.BT_PRIBRAND)
		v:addTouchEventListener(btcallback)
	end	
	
	--查看信息
	local btPersonInfo = ExternalFun.seekWigetByNameEx(csbNode, "bt_person")
	for _,v in ipairs(btPersonInfo) do
		v:setTag(ClientScene.BT_PERSON)
		v:addTouchEventListener(btcallback)
		
		--根据会员等级确定裁剪资源
		local vipIdx = GlobalUserItem.cbMemberOrder or 0
		--裁切头像
		local head = HeadSprite:createNormalCircle(GlobalUserItem, 100, "Circleframe.png")
		head:setCascadeOpacityEnabled(true)
		if nil ~= head then
			local personBtnContentSize = v:getContentSize()
			head:setPosition(personBtnContentSize.width / 2, personBtnContentSize.height / 2)
			v:addChild(head)
		end
		
		self:attach("M2V_Update_UserFace", function (e, obj)
			head:updateHead(obj)
		end)	
	end		


	--等级信息
	local text_level = ExternalFun.seekWigetByNameEx(csbNode, "atlas_levels")
	for _,v in ipairs(text_level) do
		v:setString(GlobalUserItem.wCurrLevelID.."")
	end

    --进度条
	local levelpro = ExternalFun.seekWigetByNameEx(csbNode, "bar_progress")
	for _,v in ipairs(levelpro) do
		v:setPercent(0)
	end	

    --用户ID
	local text_userid = ExternalFun.seekWigetByNameEx(csbNode, "text_userid")
	for _,v in ipairs(text_userid) do
		local str = GlobalUserItem.dwGameID  
		v:setString("ID: "..str)
	end		

	--金币
	local text_gold = ExternalFun.seekWigetByNameEx(csbNode, "atlas_coin")
	--初始化
	local str = string.formatNumberTThousands(GlobalUserItem.lUserScore)
	if string.len(str) > 11 then
		str = string.sub(str, 1, 11) .. "..."
	end
	if (GlobalUserItem.lUserScore < 10000) then
		for _,v in ipairs(text_gold) do
			v:setString(str)
		end	
	elseif (GlobalUserItem.lUserScore >= 10000 and GlobalUserItem.lUserScore < 100000000) then
		for _,v in ipairs(text_gold) do
			v:setString(str .. "万")
		end				
	elseif (GlobalUserItem.lUserScore >= 100000000) then
		for _,v in ipairs(text_gold) do
			v:setString(str .. "亿")
		end				
	end
	--注册UI监听事件
	self:attach("M2V_Update_UserScore", function (e, score)
		local str = string.formatNumberTThousands(score)
		if string.len(str) > 11 then
			str = string.sub(str, 1, 11) .. "..."
		end
		if (score < 10000) then
			for _,v in ipairs(text_gold) do
				v:setString(str)
			end	
		elseif (score >= 10000 and score < 100000000) then
			for _,v in ipairs(text_gold) do
				v:setString(str .. "万")
			end				
		elseif (score >= 100000000) then
			for _,v in ipairs(text_gold) do
				v:setString(str .. "亿")
			end				
		end
	end)	
	
	--银行
    local btn_take = ExternalFun.seekWigetByNameEx(csbNode, "btn_take")
	for _,v in ipairs(btn_take) do
		v:setTag(ClientScene.BT_BANK)
		v:addTouchEventListener(btcallback)
	end		

	--游戏豆
	local text_bean = ExternalFun.seekWigetByNameEx(csbNode, "atlas_bean")
	local str = string.formatNumberTThousands(GlobalUserItem.dUserBeans,true,"/")
	if string.len(str) > 11 then
		str = string.sub(str, 1, 11) .. "..."
	end
	for _,v in ipairs(text_bean) do
		v:setString(str)	
	end		

	--充值游戏币
    local btn_bean = ExternalFun.seekWigetByNameEx(csbNode, "btn_addcz")
	for _,v in ipairs(btn_bean) do
		v:setTag(ClientScene.BT_BEAN)
		v:addTouchEventListener(btcallback)
	end		
	
	--银行存取
	local btn_insure_add = ExternalFun.seekWigetByNameEx(csbNode, "btn_insure_add")
	for _,v in ipairs(btn_insure_add) do
		v:setTag(ClientScene.BT_INSURE_ADD)
		v:addTouchEventListener(btcallback)
	end		

	--元宝
	local text_ingot = ExternalFun.seekWigetByNameEx(csbNode, "atlas_ingot")
	local str = string.formatNumberTThousands(GlobalUserItem.lUserIngot,true,"/")
	if string.len(str) > 11 then
		str = string.sub(str, 1, 11) .. "..."
	end
	for _,v in ipairs(text_ingot) do
		v:setString(str)	
	end		
	
	--银行存取
	local btn_exchange = ExternalFun.seekWigetByNameEx(csbNode, "btn_exchange")
	for _,v in ipairs(btn_exchange) do
		v:setTag(ClientScene.BT_RECHARGE)
		v:addTouchEventListener(btcallback)
	end

	--银行
	local btn_bank = ExternalFun.seekWigetByNameEx(csbNode, "btn_bank")
	for _,v in ipairs(btn_bank) do
		v:setTag(ClientScene.BT_BANK)
		v:addTouchEventListener(btcallback)
		--处理动画
		local btn_bank_ani = v:getChildByName("bank_ani_node")
		if (btn_bank_ani) then
			local act = btn_bank_ani:getActionByTag(btn_bank_ani:getTag())
			act:play("light", true)
		end
	end
	
	--赚金
	local btn_earn = ExternalFun.seekWigetByNameEx(csbNode, "btn_earn")
	for _,v in ipairs(btn_earn) do
		v:setTag(ClientScene.BT_EARN)
		v:addTouchEventListener(btcallback)
	end	
	
    --银行金币
	local text_bankscore = ExternalFun.seekWigetByNameEx(csbNode, "atlas_bankscore")
	--初始化
	local str = string.formatNumberTThousands(GlobalUserItem.lUserInsure)
	if string.len(str) > 11 then
		str = string.sub(str, 1, 11) .. "..."
	end
	
	if (GlobalUserItem.lUserInsure < 10000) then
		for _,v in ipairs(text_bankscore) do
			v:setString(str)
		end					
	elseif (GlobalUserItem.lUserInsure >= 10000 and GlobalUserItem.lUserInsure < 100000000) then
		for _,v in ipairs(text_bankscore) do
			v:setString(str .. "万")
		end				
	elseif (GlobalUserItem.lUserInsure >= 100000000) then
		for _,v in ipairs(text_bankscore) do
			v:setString(str .. "亿")
		end				
	end		
	--注册UI监听事件
	self:attach("M2V_Update_InsureScore", function (e, score)
		local str = string.formatNumberTThousands(score)
		if string.len(str) > 11 then
			str = string.sub(str, 1, 11) .. "..."
		end
		
		if (score < 10000) then
			for _,v in ipairs(text_bankscore) do
				v:setString(str)
			end					
		elseif (score >= 10000 and score < 100000000) then
			for _,v in ipairs(text_bankscore) do
				v:setString(str .. "万")
			end				
		elseif (score >= 100000000) then
			for _,v in ipairs(text_bankscore) do
				v:setString(str .. "亿")
			end				
		end				
	end)		


	--商城
	local btn_shop = ExternalFun.seekWigetByNameEx(csbNode, "btn_shop")
	for _,v in ipairs(btn_shop) do
		v:setTag(ClientScene.BT_SHOP)
		v:addTouchEventListener(btcallback)
		
		local btn_shop_ani = v:getChildByName("shop_ani_node")
		if (btn_shop_ani) then
			local act = btn_shop_ani:getActionByTag(btn_shop_ani:getTag())
			act:play("light", true)
		end
	end		
	
	--开户豪礼
	local btn_kaihu = ExternalFun.seekWigetByNameEx(csbNode, "btn_kaihu")
	for _,v in ipairs(btn_kaihu) do
		v:setTag(ClientScene.BT_KAIHU)
		v:addTouchEventListener(btcallback)
				
		local kaihu_ani_node = v:getChildByName("kaihu_ani_node")
		if (kaihu_ani_node) then
			local act = kaihu_ani_node:getActionByTag(kaihu_ani_node:getTag())
			act:play("light", true)
		end
		--用户已绑定就隐藏
		v:setVisible(GlobalUserItem.szRegisterMobile == "")
	end		

	--好友
	local btn_friend = ExternalFun.seekWigetByNameEx(csbNode, "btn_friend")
	for _,v in ipairs(btn_friend) do
		v:setTag(ClientScene.BT_FRIEND)
		v:addTouchEventListener(btcallback)
	end		
	
	--任务	
	local btn_task = ExternalFun.seekWigetByNameEx(csbNode, "btn_task")
	for _,v in ipairs(btn_task) do
		v:setTag(ClientScene.BT_TASK)
		v:addTouchEventListener(btcallback)
	end		
	
	--排行	
	local btn_rank = ExternalFun.seekWigetByNameEx(csbNode, "btn_rank")
	for _,v in ipairs(btn_rank) do
		v:setTag(ClientScene.BT_RANK)
		v:addTouchEventListener(btcallback)
	end			
	--活动
	local btn_active = ExternalFun.seekWigetByNameEx(csbNode, "btn_active")
	for _,v in ipairs(btn_active) do
		v:setTag(ClientScene.BT_ACTIVITY)
		v:addTouchEventListener(btcallback)
	end
	
	--赚金	
	local btn_earn = ExternalFun.seekWigetByNameEx(csbNode, "btn_earn")
	for _,v in ipairs(btn_earn) do
		v:setTag(ClientScene.BT_EARN)
		v:addTouchEventListener(btcallback)
	end	
	
	--公告
	local btn_gonggao = ExternalFun.seekWigetByNameEx(csbNode, "btn_gonggao")
	for _,v in ipairs(btn_gonggao) do
		v:setTag(ClientScene.BT_GONGGAO)
		v:addTouchEventListener(btcallback)
	end	
	
	--分享
	local btn_share = ExternalFun.seekWigetByNameEx(csbNode, "btn_share")
	for _,v in ipairs(btn_share) do
		v:setTag(ClientScene.BT_SHARE)
		v:addTouchEventListener(btcallback)
	end	
	
	--赛事
	local btn_macth = ExternalFun.seekWigetByNameEx(csbNode, "btn_macth")
	for _,v in ipairs(btn_macth) do
		v:setTag(ClientScene.BT_MATCH)
		v:addTouchEventListener(btcallback)
	end		
	
	--兑换
	local btn_exchange = ExternalFun.seekWigetByNameEx(csbNode, "btn_exchange")
	for _,v in ipairs(btn_exchange) do
		v:setTag(ClientScene.BT_EXCHANGE)
		v:addTouchEventListener(btcallback)
	end			

	--喇叭
	local btn_trumpet = ExternalFun.seekWigetByNameEx(csbNode, "btn_trumpet")
	for _,v in ipairs(btn_trumpet) do
		v:setTag(ClientScene.BT_TRUMPET)
		v:addTouchEventListener(btcallback)
	end	

	--规则
	local btn_rule = ExternalFun.seekWigetByNameEx(csbNode, "btn_rule")
	for _,v in ipairs(btn_rule) do
		v:setTag(ClientScene.BT_RULE)
		v:addTouchEventListener(btcallback)
	end	
   
	--添加点击监听事件
	local moreBg = ExternalFun.seekWigetByName(csbNode, "more_bg")
	local moreBgListener = cc.EventListenerTouchOneByOne:create()
	moreBgListener:setSwallowTouches(true)
	moreBgListener:registerScriptHandler(function(touch, event)
		return moreBg:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	moreBgListener:registerScriptHandler(function(touch, event)
		local pos = touch:getLocation()
		pos = moreBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, moreBg:getContentSize().width, moreBg:getContentSize().height)
		local visible = moreBg:isVisible()
		if (moreBg:isVisible()) then
			if false == cc.rectContainsPoint(rec, pos) then
				if (not self.m_plazaAni:isPlaying()) then
					self.m_plazaAni:play("close_setting", false)
				end
			end
		else
			if (false == cc.Ref:getReferenceCount(rec, pos)) then
				if (not self.m_plazaAni:isPlaying()) then
					self.m_plazaAni:play("open_setting", false)
				end
			end
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED)
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(moreBgListener, moreBg)	

	--self._notifyText = ExternalFun.seekWigetByName(self._csbNode, "gg_text")

	self.m_bSingleGameMode = false
	if 1 == #appConfigProxy._gameList and yl.SINGLE_GAME_MODOLE then
		--默认使用第一个游戏
		local entergame = appConfigProxy._gameList[1]
		if nil ~= entergame then
			self.m_bSingleGameMode = true
			self:updateEnterGameInfo(entergame)
			GlobalUserItem.setCurGameKind(tonumber(entergame._KindID))
			
			if PriRoom and true == PriRoom:getInstance():isCurrentGameOpenPri(GlobalUserItem.getCurGameKind()) then
				self:onChangeShowMode(PriRoom.LAYTAG.LAYER_ROOMLIST)
			else
				self:onChangeShowMode(yl.SCENE_ROOMLIST)
			end
		else
			self:onChangeShowMode(yl.SCENE_GAMELIST)
		end	
	else
		--self:onChangeShowMode(yl.SCENE_GAMELIST)
	end

	local shopDetail = function(result, msg)
		if result == yl.SUB_GP_QUERY_BACKPACKET_RESULT then
			self._shopDetailFrame = nil
		end		
		
		-- 是否处理锁表
		local bHandleLockGame = true
		if PriRoom then
			-- 是否锁表、是否更新游戏、是否锁私人房
			local lockGame, updateGame, lockPriGame = PriRoom:getInstance():onEnterPlazaFinish()	
			if lockGame then
				if not updateGame and not lockPriGame then
					bHandleLockGame = false
				end
				if nil ~= self._checkInFrame then
					self._checkInFrame:onCloseSocket()
					self._checkInFrame = nil
				end

				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})	
			else
				-- 任务信息查询
				self:queryTaskInfo()
			end
		else
			-- 任务信息查询
			self:queryTaskInfo()
		end

		if not bHandleLockGame then
			-- 普通房锁表登陆
			print("ClinetScene normal lock login")
			local lockRoom = GlobalUserItem.GetGameRoomInfo(GlobalUserItem.dwLockServerID)
			if GlobalUserItem.dwLockKindID == GlobalUserItem.getCurGameKind() and nil ~= lockRoom then
				GlobalUserItem.nCurRoomIndex = lockRoom._nRoomIndex
				self:onStartGame()
			end
		end 
	end	
	self._shopDetailFrame = ShopDetailFrame:create(self, shopDetail)
	
	local checkInInfo = function(result, msg, subMessage)
		local bRes = false
		if result == 1 then
			if false == GlobalUserItem.bTodayChecked then
				if(appConfigProxy._appStoreSwitch == 0) then
					local _checkinlayer = Checkin:create(self)
					self._csbNode:addChild(_checkinlayer)
					self._checkInFrame = nil
				else
					self:dismissPopWait()
					self._checkInFrame = nil
				end
			elseif GlobalUserItem.cbMemberOrder ~= 0 then
				self._checkInFrame:sendCheckMemberGift()
				bRes = true
			else
				-- 显示广告
				if GlobalUserItem.isShowAdNotice() then
					--伟东需求 暂时屏蔽
					--if(appConfigProxy._appStoreSwitch == 0) then
					--	local webview = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.WebViewLayer"):create(self)
					--	self:addChild(webview)
					--end
				end
				self._checkInFrame = nil
			end
		elseif result == self._checkInFrame.QUERYMEMBERGIFT then

		else
			self._checkInFrame = nil
		end
		if nil ~= self._checkInFrame and self._checkInFrame.QUERYMEMBERGIFT == result then
			if true == subMessage then
				self:onChangeShowMode(yl.SCENE_CHECKIN)
			else
				-- 显示广告
				if GlobalUserItem.isShowAdNotice() then
					if(appConfigProxy._appStoreSwitch == 0) then
						local webview = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.WebViewLayer"):create(self)
						self:addChild(webview)
					end
				end
			end
			self._checkInFrame = nil
		end

		if nil == self._checkInFrame then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})	
		end
		return bRes
	end
	self._checkInFrame = CheckinFrame:create(self, checkInInfo)
	self.m_bFirstQueryCheckIn = true

	--添加后台前台监听
	local listener = cc.EventListenerCustom:create(BaseConfig.RY_APPLICATION_NOTIFY,function (event)
	    local msgWhat = event.obj
		if (msgWhat == yl.APP_BACKGROUND) then
			if type(self.onBackgroundCallBack) == "function" then
				self:onBackgroundCallBack(false)
			end
		elseif (msgWhat == yl.APP_FOREGROUND) then
			--进入前台，查询用户金币，更新数值，一些线下充值，和跳网站充值需要
			self:queryUserScoreInfo()
			if type(self.onBackgroundCallBack) == "function" then
				self:onBackgroundCallBack(true)
			end
		end
	end)
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)
	
	--兼容旧版游戏
	local listener = cc.EventListenerCustom:create(yl.RY_GET_GAME_VERSION_NOTIFY,function (e)
		assert(type(e.obj) == "table", "expected table but not!")
		if (type(e.obj) ~= "table") then
			return
		end
		
		local callback = e.obj.callback
		local kindID = e.obj.KindID
		
		assert(type(callback) == "function", "expected function but not!")
		if (type(callback) ~= "function") then
			return
		end	
		assert(type(kindID) == "number", "expected table but not!")
		if (type(kindID) ~= "number") then
			return
		end		
		
		callback(versionProxy:getResVersion(kindID))
	end)
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)	

	--快速开始
	self.m_bQuickStart = false	

	--游戏喇叭列表
	self.m_gameTrumpetList = {}
	self.m_spGameTrumpetBg = nil
	
	--插入式广播公告
	self.m_hallGongGaoMessage = {}

	-- 回退
	self.m_bEnableKeyBack = true

	-- 金币、金币动画
	self.m_nodeCoinAni = nil
	self.m_actCoinAni = nil	
	
	--时间文本
	local txtTimes = ExternalFun.seekWigetByNameEx(csbNode, "time_lab")
	local getTime=os.date("时间 : %H:%M", os.time())
	
	for _,v in ipairs(txtTimes) do
		v:setString(getTime)
	end		
	
	local repeatAction = cc.RepeatForever:create(cc.Sequence:create(cc.DelayTime:create(1), cc.CallFunc:create(function ()
		local timeDotShow = tonumber(os.date("%S")) % 2
		if (timeDotShow == 1) then
			local getTime=os.date("时间 : %H %M", os.time())
			for _,v in ipairs(txtTimes) do
				v:setString(getTime)
			end		
		else
			local getTime=os.date("时间 : %H:%M", os.time())
			for _,v in ipairs(txtTimes) do
				v:setString(getTime)
			end		
		end
	end)))
	self:runAction(repeatAction)
	
	--使用action 替代定时器 避免定时器未释放引发bug
	--self._clockTime = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
	-- end, 1, false)
	
	self:attach("M2V_ClientSceneToast", function (e, msg)
		showToast(self, msg, 2)
	end)
	
	--开始更新游戏 视图通知回调

	self._btnGameArrayCache = {} --缓存查找的对象，减少寻找对象的开销	
	self:attach("M2V_OnStartGameUpdate", function (e, gameinfo)
		local btnGame = nil
		
		if (self._btnGameArrayCache[gameinfo._KindID] == nil) then
			local gameListView = ExternalFun.seekWigetByName(self._csbNode, "GameListView")
			btnGame = ExternalFun.seekWigetByTag(gameListView, tonumber(gameinfo._KindID))
			self._btnGameArrayCache[gameinfo._KindID] = btnGame
		else
			btnGame = self._btnGameArrayCache[gameinfo._KindID]
		end
		
		if (btnGame ~= nil) then
			--转圈圈
			local spDownloadCycle = btnGame:getChildByName("game_waiting")
			--进度文本
			local textProcess = btnGame:getChildByName("progress")
			textProcess:setVisible(true)
			textProcess:setString("等待下载...")
			
			spDownloadCycle:setVisible(true)
			spDownloadCycle:stopAllActions()
			spDownloadCycle:runAction(cc.RepeatForever:create(cc.RotateBy:create(1.0, 360)))
		end
	end)
	
	--游戏更新进度 视图通知回调
	self:attach("M2V_OnGameUpdateProgress", function (e, progress, gameinfo)
		local btnGame = nil
		
		if (self._btnGameArrayCache[gameinfo._KindID] == nil) then
			local gameListView = ExternalFun.seekWigetByName(self._csbNode, "GameListView")
			btnGame = ExternalFun.seekWigetByTag(gameListView, tonumber(gameinfo._KindID))
			self._btnGameArrayCache[gameinfo._KindID] = btnGame
		else
			btnGame = self._btnGameArrayCache[gameinfo._KindID]
		end
	
		if (btnGame ~= nil) then
			--转圈圈
			local spDownloadCycle = btnGame:getChildByName("game_waiting")
			--进度文本
			local textProcess = btnGame:getChildByName("progress")
			textProcess:setString(string.format("%d", progress))
		end
	end)		
	
	--游戏更新完成 视图通知回调
	self:attach("M2V_OnGameUpdateFinished", function (e, code, gameinfo)
		local btnGame = nil
		
		if (self._btnGameArrayCache[gameinfo._KindID] == nil) then
			local gameListView = ExternalFun.seekWigetByName(self._csbNode, "GameListView")
			btnGame = ExternalFun.seekWigetByTag(gameListView, tonumber(gameinfo._KindID))
			self._btnGameArrayCache[gameinfo._KindID] = btnGame
		else
			btnGame = self._btnGameArrayCache[gameinfo._KindID]
		end
		
		if (code == 0) then
			--判断玩家是否在游戏中，在游戏中就不提示更新完毕的声音
			local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
			local findContext = contextProxy:findContextByName(VIEW_LIST.GAME_LAYER)
			if (findContext == nil) then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_EFFECT, {}, "sound/game_down_finished.mp3")
			end
			
			if (btnGame ~= nil) then
				--转圈圈
				local spDownloadCycle = btnGame:getChildByName("game_waiting")
				--进度文本
				local textProcess = btnGame:getChildByName("progress")
				textProcess:setVisible(false)
				
				--进度文本
				local updateTips = btnGame:getChildByName("update_tips")
				updateTips:setVisible(false)			
				
				spDownloadCycle:setVisible(false)
				spDownloadCycle:stopAllActions()
			end			
		else
			--进度文本
			if (btnGame ~= nil) then
				--转圈圈
				local spDownloadCycle = btnGame:getChildByName("game_waiting")
				
				local updateTips = btnGame:getChildByName("update_tips")
				updateTips:setVisible(true)		
				
				--进度文本
				local textProcess = btnGame:getChildByName("progress")
				textProcess:setVisible(false)
				
				spDownloadCycle:setVisible(false)
				spDownloadCycle:stopAllActions()				
			end				
		end
	end)	
	
	--开始进入游戏 视图通知回调
	self:attach("M2V_OnEnterGame", function (e, gameinfo)
		local btnGame = nil
		
		if (self._btnGameArrayCache[gameinfo._KindID] == nil) then
			local gameListView = ExternalFun.seekWigetByName(self._csbNode, "GameListView")
			btnGame = ExternalFun.seekWigetByTag(gameListView, tonumber(gameinfo._KindID))
			self._btnGameArrayCache[gameinfo._KindID] = btnGame
		else
			btnGame = self._btnGameArrayCache[gameinfo._KindID]
		end
		
		if (btnGame ~= nil) then
			local spDownloadCycle = btnGame:getChildByName("game_waiting")
			--转圈圈
			spDownloadCycle:setVisible(false)
			spDownloadCycle:stopAllActions()
			
			--进度文本
			local textProcess = btnGame:getChildByName("progress")
			textProcess:setVisible(false)			
		end
	end)
	
	--大厅固定公告消息视图监听
	self:attach("M2V_Update_GongGao_Message", function (e, data, msg)
		local text_gonggao = ExternalFun.seekWigetByName(self._csbNode, "gg_text")
		local mask_gonggao = ExternalFun.seekWigetByName(self._csbNode, "gg_mask")
		local mask_contentsize = mask_gonggao:getContentSize()
		local text_content = ""
		for _,v in ipairs(data) do
			text_content = text_content .. v.content .. "\t\t"
		end
		
		self:showGongGao(text_content, nil)
	end)

	--请求大厅公告
	appConfigProxy:requestNotice()
	
	if (appConfigProxy._appStoreSwitch == 0) then
		self:initGameList()
	else
		if (#BaseConfig.APPSTORE_GAME_LIST == 1) then
			self:initRoomList(BaseConfig.APPSTORE_GAME_LIST[1])
		else
			self:initGameList()
		end
	end	
	
	local undateScore = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onUserInfoChange))
	cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(undateScore, self)
end

-- 进入场景而且过渡动画结束时候触发。
function ClientScene:onEnterTransitionFinish()
	self:registerNotifyList()
	if not GlobalUserItem.bIsAngentAccount then		
		--查询赠送
		self._shopDetailFrame:onQuerySend()
	else
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})	
	end	

--[[	--显示任务状态专用
	self.m_curGameKind = 0
		--监听消息
	local listener = cc.EventListenerCustom:create(yl.RY_SHOW_VIEW_GAME_KIND,function (event)
	    local msgWhat = event.obj
		self.m_curGameKind = msgWhat or 0
	end)
    cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)--]]
		
	--玩家是否第一次进入大厅
	local bfristenter = cc.UserDefault:getInstance():getBoolForKey("fristenter",true)
	if bfristenter then
		--默认使用第一个金币游戏
		local entergame = appConfigProxy._gameList[1]
		if nil ~= entergame then
			local _kindID = tonumber(entergame._KindID)
			GlobalUserItem.setCurGameKind(_kindID)
			self:updateEnterGameInfo(entergame)
			--默认第一个金币游戏
			cc.UserDefault:getInstance():setIntegerForKey("goldkindid", _kindID)
		end
		if nil ~= entergame then
		--self.m_actStartTargetGameAni:play(string.format("%d", entergame._KindID),true)
		end
		cc.UserDefault:getInstance():setBoolForKey("fristenter",GlobalUserItem.bFristAble)  
	else
		local _kindid = cc.UserDefault:getInstance():getIntegerForKey("goldkindid")
		GlobalUserItem.setCurGameKind(_kindid)
		self:loadGameList(_kindid)
		--self.m_actStartTargetGameAni:play(string.format("%d", _kindid),true)
	end
				
	--默认使用第一个游戏
	--[[local entergame = self:getEnterGameInfo()
	if nil == entergame then
		entergame = appConfigProxy._gameList[1]
		self:updateEnterGameInfo(entergame)
	end
	if nil ~= entergame then
		self.m_actStartTargetGameAni:play(string.format("%d", entergame._KindID),true)
	end--]]

    return self
end

-- 退出场景而且开始过渡动画时候触发。
function ClientScene:onExitTransitionStart()
	self._sceneLayer:unregisterScriptKeypadHandler()

    return self
end

function ClientScene:onExit()
	if self._gameFrame:isSocketServer() then
		self._gameFrame:onCloseSocket()
	end
	self:disconnectFrame()	

--[[	ExternalFun.SAFE_RELEASE(self.m_actBoxAni)
	self.m_actBoxAni = nil--]]
--[[	ExternalFun.SAFE_RELEASE(self.m_actStartBtnAni)
	self.m_actStartBtnAni = nil--]]
--[[	ExternalFun.SAFE_RELEASE(self.m_actChargeBtnAni)
	self.m_actChargeBtnAni = nil--]]
	ExternalFun.SAFE_RELEASE(self.m_actCoinAni)
	self.m_actCoinAni = nil
	
--[[	ExternalFun.SAFE_RELEASE(self.m_actStartTargetGameAni)
	self.m_actStartTargetGameAni = nil
--]]
	self:releasePublicRes()

	self:unregisterNotify()		
	removeBackGroundCallback()

	if PriRoom then
		PriRoom:getInstance():onExitPlaza()
	end			
	return self
end

--显示公告
function ClientScene:showGongGao(text, callback)
	local text_gonggao = ExternalFun.seekWigetByName(self._csbNode, "gg_text")
	local mask_gonggao = ExternalFun.seekWigetByName(self._csbNode, "gg_mask")
	local mask_contentsize = mask_gonggao:getContentSize()
	local text_content = text
	
	text_gonggao:setString(text_content)
	local time_speed = 80
	local speed = (mask_contentsize.width + text_gonggao:getContentSize().width) / time_speed
	local startPosition = cc.p(mask_contentsize.width, mask_contentsize.height / 2)
	local endPosition = cc.p(-text_gonggao:getContentSize().width, mask_contentsize.height / 2)
	
	text_gonggao:setPosition(startPosition)
	
	text_gonggao:stopAllActions()
	local action = cc.RepeatForever:create(cc.Sequence:create(
	cc.MoveTo:create(speed, endPosition),
	cc.CallFunc:create(function()
		text_gonggao:setPosition(startPosition)
		if (callback) then
			callback()
		end
	end)))
	text_gonggao:runAction(action)
end

function ClientScene:initGameList()
	----------------------------pageView初始化----------------------------------
	local mediumLayout = self._csbNode:getChildByName("medium_layout")
	local pageView = mediumLayout:getChildByName("GameListView")
	local pageViewTouchEventMask = mediumLayout:getChildByName("GameListViewClickTouchMask")
	local pageLayout = nil
	
	--审核模式 显示不同个数的游戏
	local MAX_SHOW_LIST = 0
	--要显示的游戏列表
	local gamelist = {}
	--只显示的游戏
	local gameFilter = BaseConfig.APPSTORE_GAME_LIST
	if (appConfigProxy._appStoreSwitch == 0) then
		MAX_SHOW_LIST = 8
		
		for i = 1, #appConfigProxy._gameList do
			local bOnlyAppstoreGame = false
			for _, v in pairs(BaseConfig.ONLY_APPSTORE_GAME) do
				if (v == tonumber(appConfigProxy._gameList[i]._KindID)) then
					bOnlyAppstoreGame = true
					break
				end
			end
			
			if (not bOnlyAppstoreGame) then
				table.insert(gamelist, appConfigProxy._gameList[i])
			end
		end	
	elseif (appConfigProxy._appStoreSwitch == 1) then
		MAX_SHOW_LIST = #gameFilter
		
		--过滤
		for i = 1, #appConfigProxy._gameList do
			local gameinfo = appConfigProxy._gameList[i]
			for j = 1, #gameFilter do
				if (tonumber(gameinfo._KindID) == gameFilter[j]) then
					table.insert(gamelist, gameinfo)
				end
			end
		end	
	end
	
	local pageCount = 0

	--获取 游戏 引用次数 并更新排序
	local gameListAccessProxy = AppFacade:getInstance():retrieveProxy("GameListAccessProxy")
	
	local backGameList = clone(gamelist)
	for i=#gameListAccessProxy._gameList, 1, -1 do
		for j=1, #backGameList do
			if (backGameList[j]._KindID == gameListAccessProxy._gameList[i].kinid) then
				local Item = table.remove(backGameList, j)
				table.insert(backGameList, 1, Item)
				break
			end
		end
	end	
	
	for i = 1, #backGameList do
		local index = (i - 1) % MAX_SHOW_LIST
		if (index == 0) then
			pageLayout = mediumLayout:getChildByName("GameListItem"):clone()
			pageView:pushBackCustomItem(pageLayout)
			pageCount = pageCount + 1
		end
		
		assert(pageLayout ~= nil, "can't find game item")
		
		local gameinfo = backGameList[i]
	
		local unsel_img = string.format("game_%d_%d.png", gameinfo._KindID, 0)
		local sel_img = string.format("game_%d_%d.png", gameinfo._KindID, 1)
		
		local unsel_frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(unsel_img)
		local sel_frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(sel_img)
		
		--图片存在检查
		if unsel_frame == nil then
			unsel_img = "game_default.png"
			unsel_frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(unsel_img)
			assert(unsel_frame ~= nil, "can't find imageframe")
		end
		
		if sel_frame == nil then
			sel_img = "game_default.png"
			sel_frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(sel_img)
			assert(sel_frame ~= nil, "can't find imageframe")
		end
		

		local btnGame = pageLayout:getChildByName(string.format("btn_game_%d", index + 1))
		btnGame:addClickEventListener(handler(self, self.onGameClickEvent))
		
		btnGame:loadTextures(unsel_img, sel_img, "", ccui.TextureResType.plistType)
		
		btnGame:setTag(tonumber(gameinfo._KindID))
		
		btnGame:setVisible(true)
		
		--检测是否有更新
		local version = versionProxy:getResVersion(gameinfo._KindID)
		
		if version then
			if gameinfo._ServerResVersion > version then
				if (appConfigProxy._appStoreSwitch == 0) then
					btnGame:getChildByName("update_tips"):setVisible(true)
				end
			end
		else
			if (appConfigProxy._appStoreSwitch == 0) then
				btnGame:getChildByName("update_tips"):setVisible(true)
			end
		end
		
		--审核模式 只显示一页
		if (appConfigProxy._appStoreSwitch == 1) then
			if (i == MAX_SHOW_LIST) then
				break
			end
		end
	end
	
	--附加更新大厅视图事件
	self:attach("M2V_Update_Plaza_Game_List", function (e, obj)
		local curIndexBak = pageView:getCurrentPageIndex()
		pageView:removeAllItems()
		self._btnGameArrayCache = {}
		
		local gameListAccessProxy = AppFacade:getInstance():retrieveProxy("GameListAccessProxy")
		local backGameList = clone(gamelist)
		for i=#gameListAccessProxy._gameList, 1, -1 do
			for j=1, #backGameList do
				if (backGameList[j]._KindID == gameListAccessProxy._gameList[i].kinid) then
					local Item = table.remove(backGameList, j)
					table.insert(backGameList, 1, Item)
					break
				end
			end
		end
		
		pageCount = 0
		for i = 1, #backGameList do
			local index = (i - 1) % MAX_SHOW_LIST
			if (index == 0) then
				pageLayout = mediumLayout:getChildByName("GameListItem"):clone()
				pageView:pushBackCustomItem(pageLayout)
				pageCount = pageCount + 1
			end
			
			assert(pageLayout ~= nil, "can't find game item")
			
			local gameinfo = backGameList[i]
		
			local unsel_img = string.format("game_%d_%d.png", gameinfo._KindID, 0)
			local sel_img = string.format("game_%d_%d.png", gameinfo._KindID, 1)
			
			local unsel_frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(unsel_img)
			local sel_frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(sel_img)
			
			--图片存在检查
			if unsel_frame == nil then
				unsel_img = "game_default.png"
				unsel_frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(unsel_img)
				assert(unsel_frame ~= nil, "can't find imageframe")
			end
			
			if sel_frame == nil then
				sel_img = "game_default.png"
				sel_frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(sel_img)
				assert(sel_frame ~= nil, "can't find imageframe")
			end
			

			local btnGame = pageLayout:getChildByName(string.format("btn_game_%d", index + 1))
			btnGame:addClickEventListener(handler(self, self.onGameClickEvent))
			
			btnGame:loadTextures(unsel_img, sel_img, "", ccui.TextureResType.plistType)
			
			--更新数据
			self._btnGameArrayCache[gameinfo._KindID] = btnGame
			
			btnGame:setTag(tonumber(gameinfo._KindID))
			
			btnGame:setVisible(true)
			
			--检测是否有更新
			local version = versionProxy:getResVersion(gameinfo._KindID)
			
			if version then
				if gameinfo._ServerResVersion > version then
					if (appConfigProxy._appStoreSwitch == 0) then
						btnGame:getChildByName("update_tips"):setVisible(true)
					end
				end
			else
				if (appConfigProxy._appStoreSwitch == 0) then
					btnGame:getChildByName("update_tips"):setVisible(true)
				end
			end
			
			--审核模式 只显示一页
			if (appConfigProxy._appStoreSwitch == 1) then
				if (i == MAX_SHOW_LIST) then
					break
				end
			end			
		end
		
		pageView:scrollToPage(curIndexBak)
	end)	

	--------------------------------------------------------------
	
	--------------------pageview 小点点提示-----------------------------
	--单选按钮
	local radioGroupLayout = mediumLayout:getChildByName("PageRadioGroup")
    local radioButtonGroup = ccui.RadioButtonGroup:create()
    radioButtonGroup:setAllowedNoSelection(false)
    self:addChild(radioButtonGroup)

	--设置小点点的位置
	local radioGroupLayoutSize = radioGroupLayout:getContentSize()
	if (pageCount == 1) then
		local radioBtn = ccui.RadioButton:create("btnUnsel.png","btnUnsel.png","btnSel.png","btnUnsel.png","btnUnsel.png", ccui.TextureResType.plistType)
		radioBtn:move(radioGroupLayoutSize.width / 2, radioGroupLayoutSize.height / 2)
			:addTo(radioGroupLayout)
		radioButtonGroup:addRadioButton(radioBtn)
	else
		local aveLength = radioGroupLayoutSize.width / (pageCount + 1)
		for i = 1, pageCount do
			local radioBtn = ccui.RadioButton:create("btnUnsel.png","btnUnsel.png","btnSel.png","btnUnsel.png","btnUnsel.png", ccui.TextureResType.plistType)
			radioBtn:move(aveLength * i, radioGroupLayoutSize.height / 2)
				:addTo(radioGroupLayout)
			radioButtonGroup:addRadioButton(radioBtn)
		end
	end
	-------------------------------------------------
	local btnArrowLeft = mediumLayout:getChildByName("btn_left")
	local btnArrowRight = mediumLayout:getChildByName("btn_right")
	
	
	pageView:scrollToPage(0)
	
	btnArrowLeft:addClickEventListener(function (sender)
		ExternalFun.playClickEffect()
		local index = pageView:getCurrentPageIndex()
		local newindex = index - 1
		if (newindex >= 0) then
			if (pageView:getCurrentPageIndex() ~= newindex) then
				pageView:scrollToPage(newindex)
			end
		else
			if (pageView:getCurrentPageIndex() ~= pageCount - 1) then
				pageView:scrollToPage(pageCount - 1)
			end
		end
	end)
	
	btnArrowRight:addClickEventListener(function (sender)
		ExternalFun.playClickEffect()
		local index = pageView:getCurrentPageIndex()
		local newindex = index + 1
		
		if (newindex < pageCount) then
			if (pageView:getCurrentPageIndex() ~= newindex) then
				pageView:scrollToPage(newindex)
				--radioButtonGroup:setSelectedButton(newindex)
			end
		else
			if (pageView:getCurrentPageIndex() ~= 0) then
				pageView:scrollToPage(0)
				--radioButtonGroup:setSelectedButton(0)
			end
		end
	end)

	-------------------------关联两个ui的事件-------------------------
	pageView:addEventListener(function (sender, eventType)
		--更新状态
		if (eventType == ccui.PageViewEventType.turning) then
			local pageIndex = pageView:getCurrentPageIndex()
			local radioIndex = radioButtonGroup:getSelectedButtonIndex()
			if (pageIndex ~= radioIndex) then
				
				radioButtonGroup:setSelectedButton(pageView:getCurrentPageIndex())
			end
		end
	end)
	
	--滑动 点击事件 播放音效
	--添加点击监听事件
	--专门做用户点击响应声音的点击遮罩层
	local listener = cc.EventListenerTouchOneByOne:create()
	listener:setSwallowTouches(false)
	listener:registerScriptHandler(function(touch, event)
		local pos = touch:getLocation()
		pos = pageViewTouchEventMask:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pageViewTouchEventMask:getContentSize().width, pageViewTouchEventMask:getContentSize().height)
		if true == cc.rectContainsPoint(rec, pos) then
			ExternalFun.playClickEffect()
		end
		return true
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	--listener:registerScriptHandler(function(touch, event)
	--end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, pageViewTouchEventMask)

	
	radioButtonGroup:addEventListener(function (sender, index, eventType)
		if (ccui.RadioButtonEventType.selected == eventType) then
			ExternalFun.playClickEffect()
			local pageIndex = pageView:getCurrentPageIndex()
			local radioIndex = radioButtonGroup:getSelectedButtonIndex()
			if (pageIndex ~= radioIndex) then
				pageView:scrollToPage(index)
			end
		end
	end)
	--------------------------------------------------
end

function ClientScene:initRoomList(kinid)
	local tabRoomListInfo = {}
	for k,v in pairs(GlobalUserItem.roomlist) do
		if tonumber(v[1]) == kinid then
			local listinfo = v[2]
			if type(listinfo) ~= "table" then
				break
			end
			local normalList = {}
			for k,v in pairs(listinfo) do
				if v.wServerType ~= yl.GAME_GENRE_PERSONAL then
					table.insert( normalList, v)
				end
			end
			tabRoomListInfo = normalList
			break
		end
	end	
	
	local findGameinfo = nil
	--过滤
	for i = 1, #appConfigProxy._gameList do
		local gameinfo = appConfigProxy._gameList[i]
		if (tonumber(gameinfo._KindID) == kinid) then
			findGameinfo = gameinfo
		end
	end	
	
	if (findGameinfo == nil) then
		showToast(self, "查找游戏失败", 1)
		return
	end

	----------------------------pageView初始化----------------------------------
	local mediumLayout = self._csbNode:getChildByName("medium_layout")
	local pageView = mediumLayout:getChildByName("GameListView")
	local pageLayout = nil

	for i = 1, #tabRoomListInfo do
		local iteminfo = tabRoomListInfo[i]
		local wLv = (iteminfo == nil and 0 or iteminfo.wServerLevel)

		if 8 == wLv then
			--比赛场单独处理
		else
			local rule = (iteminfo == nil and 0 or iteminfo.dwServerRule)
			--wLv = (bit.band(yl.SR_ALLOW_AVERT_CHEAT_MODE, rule) ~= 0) and 10 or iteminfo.wServerLevel
			wLv = (wLv ~= 0) and wLv or 1
			local wRoom = math.mod(wLv, 6)--math.mod(wLv, 3)--bit.band(wLv, 3)
			local szName = (iteminfo == nil and "房间名称" or iteminfo.szServerName)
			local szCount = (iteminfo == nil and "0" or(iteminfo.dwOnLineCount..""))
			local szServerScore = (iteminfo == nil and "0" or string.format("%0.2f", iteminfo.lCellScore))
			local szOnLineCount = (iteminfo == nil and "0" or iteminfo.dwOnLineCount)
			local dwAndroidCount = (iteminfo == nil and "0" or iteminfo.dwAndroidCount)
			local dwDummyCount = (iteminfo == nil and "0" or iteminfo.dwDummyCount)
			local szEnterScore = (iteminfo == nil and "0" or string.format("%0.2f", iteminfo.lEnterScore))
			local enterGame = GlobalUserItem.m_tabEnterGame

			--房间btn
			if (i % 3) == 1 then
				pageLayout = mediumLayout:getChildByName("RoomListItem"):clone()
				pageView:pushBackCustomItem(pageLayout)
			end
			
			local btnRoom = pageLayout:getChildByName(string.format("btn_room_%d", wLv))
			assert(btnRoom ~= nil)
			
			--加载图片
			local normalPath = string.format("RoomList/btn_level%d_0.png", wLv)
			local selectedPath = string.format("RoomList/btn_level%d_1.png", wLv)
			btnRoom:loadTextures(normalPath, selectedPath)
			
			--准入分数
			local textEnterScore = btnRoom:getChildByName("difen")
			textEnterScore:setString("入场:" .. szEnterScore)
			
			--成绩
			local textServerScore = btnRoom:getChildByName("zunru")
			textServerScore:setString(szServerScore)
			
			--点击回调函数
			btnRoom:addClickEventListener(function ()
				GlobalUserItem.setCurGameKind(tonumber(kinid))
				GlobalUserItem.m_tabEnterGame = findGameinfo
				self:onGameBtnCallBack(btnRoom, iteminfo)
			end)
		end
	end
end

function ClientScene:onGameBtnCallBack(sender, gameinfo)
	local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
	local findContext = contextProxy:findContextByName(VIEW_LIST.CLIENT_SCENE)
	
	if (findContext == nil) then
		assert(false, "can't find ClientScene!")
	end
	
	local viewCompoment = findContext:getView()
	
	GlobalUserItem.nCurRoomIndex = gameinfo._nRoomIndex
	GlobalUserItem.bPrivateRoom = (gameinfo.wServerType == yl.GAME_GENRE_PERSONAL)
	if viewCompoment:roomEnterCheck() then
		viewCompoment:onStartGame()
	end	
end

function ClientScene:onGameClickEvent(sender)
	local kinid = sender:getTag()
	--检测游戏更新
	self:emit("V2M_CheckGameUpdate", kinid)
end

function ClientScene:onViewLoad()
	--只预加载该页面可能打开的页面
	
	--异步加载个人信息缓存图
	appdf.loadImage("client/res/Userinfo/userinfo.plist","client/res/Userinfo/userinfo.png", function ()
		printf("userinfo load finished\n")
	end)
	--异步加载推广赚钱缓存图
	appdf.loadImage("client/res/Earn/earn.plist","client/res/Earn/earn.png", function ()
		printf("earn load finished\n")
	end)
	--异步加载银行缓存图
	appdf.loadImage("client/res/Bank/bank.plist","client/res/Bank/bank.png", function ()
		printf("bank load finished\n")
	end)
	--异步加载充值缓存图
	appdf.loadImage("client/res/Shop/Shop.plist","client/res/Shop/Shop.png", function ()
		printf("bank load finished\n")
	end)
	--异步开户豪礼
	appdf.loadImage("client/res/VisitorBind/visitor.plist","client/res/VisitorBind/visitor.png", function ()
		printf("visitor load finished\n")
	end)	

	self:loadFinished()
end

function ClientScene:onWillViewEnter()
	local musicPath = string.format("sound/client_in_%d.mp3", math.random(1,2))
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, musicPath)
	self:enterViewFinished()
	
	if(appConfigProxy._appStoreSwitch == 0) then
		
		--if not GlobalUserItem.bVistor then
		--	self:getVerificationCode()
		--else
		--	showToast(self, "游客状态下，无法修改密码，请绑定手机号！", 1)
		--end
		
		--判断游客是否绑定手机
		self:checkVisitorBind()
	end		
end

function ClientScene:onWillViewExit()
	self:resetClientData()
	self:exitViewFinished()
end

function ClientScene:checkVisitorBind()
	if GlobalUserItem.szRegisterMobile ~= "" then
		return
	else
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.VISITOR_BIND_LAYER)
	end
end

function ClientScene:registerNotifyList()
	-- 代理帐号不显示
	if GlobalUserItem.bIsAngentAccount then
		return
	end

	--聊天
	local notify = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_CHAT_NOTIFY)
	notify.name = "client_friend_chat"
	notify.group = "client_friend"
	notify.fun = handler(self,self.onNotify)
	NotifyMgr:getInstance():registerNotify(notify)
	--申请好友通知
	local notify2 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_APPLYFOR_NOTIFY)
	notify2.name = "client_friend_apply"
	notify2.group = "client_friend"
	notify2.fun = handler(self,self.onNotify)
	NotifyMgr:getInstance():registerNotify(notify2)
	--回应通知
	local notify3 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_RESPOND_NOTIFY)
	notify3.name = "client_friend_response"
	notify3.group = "client_friend"
	notify3.fun = handler(self,self.onNotify)
	NotifyMgr:getInstance():registerNotify(notify3)
	--邀请通知
	local notify4 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_GAME_NOTIFY)
	notify4.name = "client_friend_invite"
	notify4.group = "client_friend"
	notify4.fun = handler(self,self.onNotify)
	NotifyMgr:getInstance():registerNotify(notify4)
	--私人房邀请
	local notify5 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_PERSONAL_NOTIFY)
	notify5.name = "client_pri_friend_invite"
	notify5.fun = handler(self,self.onNotify)
	notify5.group = "client_friend"
	NotifyMgr:getInstance():registerNotify(notify5)
	--分享通知
	local notify6 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_SHARE_NOTIFY)
	notify6.name = "client_friend_share"
	notify6.fun = handler(self,self.onNotify)
	notify6.group = "client_friend"
	NotifyMgr:getInstance():registerNotify(notify6)

	--喇叭通知
	local notify7 = NotifyMgr:getInstance():createNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_TRUMPET_NOTIFY)
	notify7.name = "trumpet"
	notify7.group = "client_trumpet"
	notify7.fun = handler(self,self.onNotify)
	NotifyMgr:getInstance():registerNotify(notify7)

	-- 任务
	local notify8 = NotifyMgr:getInstance():createNotify(yl.MDM_GP_USER_SERVICE, yl.SUB_GP_TASK_INFO)
	notify8.name = "client_task_info"
	notify8.group = "client_task"
	notify8.fun = handler(self,self.onNotify)
	NotifyMgr:getInstance():registerNotify(notify8)

	--判断是否有未读消息通知(针对消息通知界面)
    local tmp = FriendMgr:getInstance():getUnReadNotify()
    if #tmp > 0 then
        NotifyMgr:getInstance():excute(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_APPLYFOR_NOTIFY, nil)
    end
end

function ClientScene:unregisterNotify()
	-- 代理帐号不显示
	if GlobalUserItem.bIsAngentAccount then
		return
	end

	NotifyMgr:getInstance():unregisterNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_CHAT_NOTIFY, "client_friend_chat")
	NotifyMgr:getInstance():unregisterNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_APPLYFOR_NOTIFY, "client_friend_apply")
	NotifyMgr:getInstance():unregisterNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_RESPOND_NOTIFY, "client_friend_response")
	NotifyMgr:getInstance():unregisterNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_GAME_NOTIFY, "client_friend_invite")
	NotifyMgr:getInstance():unregisterNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_USER_SHARE_NOTIFY, "client_friend_share")
	NotifyMgr:getInstance():unregisterNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_INVITE_PERSONAL_NOTIFY, "client_pri_friend_invite")
	NotifyMgr:getInstance():unregisterNotify(chat_cmd.MDM_GC_USER, chat_cmd.SUB_GC_TRUMPET_NOTIFY, "trumpet")
	NotifyMgr:getInstance():unregisterNotify(yl.MDM_GP_USER_SERVICE, yl.SUB_GP_TASK_INFO, "client_task_info")
end

function ClientScene:onBackgroundCallBack(bEnter)
	if not bEnter then
		print("onBackgroundCallBack not bEnter")
 
        local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
        if contextProxy:findContextByName(VIEW_LIST.GAME_LAYER) == nil then 
			 
		    if nil ~= self._gameFrame and self._gameFrame:isSocketServer() and GlobalUserItem.bAutoConnect then			
			    self._gameFrame:onCloseSocket()
		    end

		    self:disconnectFrame()

		    --关闭好友服务器
		    FriendMgr:getInstance():reSetAndDisconnect()

		    self:dismissPopWait()

		    -- 关闭介绍
		    if self:getChildByName(HELP_LAYER_NAME) then
			    self:removeChildByName(HELP_LAYER_NAME)
		    end
        end 
	else
		local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
		--查找是否游添加过游戏视图
		local currentContext = contextProxy:findContextByName(VIEW_LIST.GAME_LAYER)
		if (currentContext ~= nil) then
			if self._gameFrame:isSocketServer() == false and GlobalUserItem.bAutoConnect then
				--重置游戏消息框架
				self._gameFrame:OnResetGameEngine()
				
				local QueryData = ExternalFun.create_netdata(logincmd.CMD_MB_QueryGameLockInfo)
				QueryData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_QUERY_GAMELOCKINFO)
				QueryData:pushdword(GlobalUserItem.dwUserID)
				--短连查询 用户是否已经锁在游戏中
				appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, QueryData, function (Datatable, Responce)
					if (Responce.code == 0) then
						if (Datatable.sub == logincmd.SUB_GP_QUERY_GAMELOCKINFO_RESULT) then
							local datatable =  ExternalFun.read_netdata(logincmd.CMD_MB_QueryGameLockInfoResult, Datatable.data)
							if (datatable.dwKindID ~= 0) then
								-- 游戏锁表处理
								local lockRoom = GlobalUserItem.GetGameRoomInfo(datatable.dwServerID)
								if nil ~= lockRoom then
									GlobalUserItem.nCurRoomIndex = lockRoom._nRoomIndex
									self:onStartGame()
								end
							else
								--检查是否为百人场 类型游戏，是则直接进入游戏房间
								local kindlist = GlobalUserItem.kindlist
								local bFind = false
								for _, v in ipairs(kindlist) do
									if tonumber(GlobalUserItem.getCurGameKind()) == v.wKindID then
										if (bit.band(v.dwSuportType, yl.KIND_SUPPORT_BAIREN) ~= 0) then
											bFind = true
											self:onStartGame()
										end
									end
								end
								
								if (bFind == false) then
									AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.GAME_LAYER})
								end
							end
						end
					else
						showToast(runScene, "游戏信息查询失败, 网络连接超时！", 1)
					end
				end)
			end
		end		
		
		self:dismissNetWait() --MXM关闭网络连接等待动画
		--连接好友服务器
		FriendMgr:getInstance():reSetAndLogin()
	end
end

function ClientScene:onRoomCallBack(code,message)
	if (code ~= 0 and self._scheduleKeepAlive) then
		return
	end
	
	print("onRoomCallBack:"..code)
	if message then
		showToast(self,message,1)
	end
	if code == -1  then
		self:dismissGameWait()
		--MXM网络断线重连
		local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
		--查找是否游添加过游戏视图
		local findContext = contextProxy:findContextByName(VIEW_LIST.GAME_LAYER)
		
		if findContext then
			self:showNetWait()

			self._scheduleKeepAlive = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
				--定时器 不会马上停
				if(code == 0) then
					return
				end
				--这里判断游戏结束后要退出房间
				self._gameFrame:OnResetGameEngine()
				self:onStartGame()
				
				--if not self._gameFrame:onCreateSocket(self._gameFrame._roomInfo.szServerAddr,self._gameFrame._roomInfo.wServerPort) and nil ~= self._callBack then
				--	self._callBack(-1,"建立连接失败！")
				--end
				--self._gameFrame:onInitData()
				--self._gameFrame:setKindInfo(GlobalUserItem.getCurGameKind(), entergame._KindVersion)
				--local curScene = self._sceneRecord[#self._sceneRecord]
				--self._gameFrame:setViewFrame(self)
				--self._gameFrame:onCloseSocket()
				--self._gameFrame:onLogonRoom()
			end, 5, false)
		end
		
		local findContext = contextProxy:findContextByName(VIEW_LIST.ROOM_TABLE_LAYER)
		
		if findContext then
			print("onRoomCallBack curscene is "..curScene)
			local curLayer = findContext:getView()
			if curLayer and curLayer.onExitRoom then
				curLayer:onExitRoom()
			else
				--self:onChangeShowMode(yl.SCENE_ROOMLIST)
				assert(false, "???")
			end
		end
	elseif(code == 0) then
		self:stopKeepAlive()
	end
end

function ClientScene:onReQueryFailure(code, msg)
	self:dismissPopWait()
	if nil ~= msg and type(msg) == "string" then
		showToast(self,msg,2)
	end
end

function ClientScene:onEnterRoom()
	print("client onEnterRoom")
	self:dismissGameWait()
	 --开始保持活动连接
    --self:startKeepAlive()
	-- 防作弊房间
	if GlobalUserItem.isAntiCheat() then
		print("防作弊")
		if self._gameFrame:SitDown(yl.INVALID_TABLE,yl.INVALID_CHAIR) then
			self:showPopWait()
		end
		return
	end

	--如果是快速游戏
	local entergame = self:getEnterGameInfo()
	if self.m_bQuickStart and nil ~= entergame then
		self.m_bQuickStart = false
		local t,c = yl.INVALID_TABLE,yl.INVALID_CHAIR
		-- 找桌
		local bGet = false
		for k,v in pairs(self._gameFrame._tableStatus) do
			-- 未锁 未玩			
			if v.cbTableLock == 0 and v.cbPlayStatus == 0 then
				local st = k - 1
				local chaircount = self._gameFrame._wChairCount
				for i = 1, chaircount  do					
					local sc = i - 1
					if nil == self._gameFrame:getTableUserItem(st, sc) then
						t = st
						c = sc
						bGet = true
						break
					end
				end
			end

			if bGet then
				break
			end
		end
		print( " fast enter " .. t .. " ## " .. c)
		if self._gameFrame:SitDown(t,c) then
			self:showGameWait()
		end
	else
		
		--MXM自定义房间界面处理登陆成功消息
		local entergame = self:getEnterGameInfo()
		if nil ~= entergame then
			local modulestr = string.gsub(entergame._KindName, "%.", "/")
			local targetPlatform = cc.Application:getInstance():getTargetPlatform()
			local customRoomFile = ""
			if cc.PLATFORM_OS_WINDOWS == targetPlatform then
				customRoomFile = "game/" .. modulestr .. "src/views/GameRoomListLayer.lua"
			else
				customRoomFile = "game/" .. modulestr .. "src/views/GameRoomListLayer.luac"
			end
			if cc.FileUtils:getInstance():isFileExist(customRoomFile) then
				if (appdf.req(customRoomFile):onEnterRoom(self._gameFrame)) then
					self:showGameWait()
					return
				else
					--断网、退出房间
					if nil ~= self._gameFrame then
						self._gameFrame:onCloseSocket()
						GlobalUserItem.nCurRoomIndex = -1
					end
				end
			end
		end

		local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
	
		if contextProxy:findContextByName(VIEW_LIST.GAME_LAYER) then
			--防作弊房间
			if GlobalUserItem.isAntiCheat() then
				self._gameFrame:onCloseSocket()
			-- 私人房
			elseif GlobalUserItem.bPrivateRoom then
				if PriRoom then
					PriRoom:getInstance():exitGame()
				end
				self._gameFrame:onCloseSocket()		
			end

			-- 任务查询
			self:queryTaskInfo()
			-- 游戏币查询
			self:queryUserScoreInfo()
			
			--修改游戏提示
			local entergame = self:getEnterGameInfo()
			if nil == entergame then
				entergame = appConfigProxy._gameList[1]
				self:updateEnterGameInfo(entergame)
			end

			-- 游戏喇叭关闭
			if nil ~= self.m_spGameTrumpetBg then
				self.m_spGameTrumpetBg:stopAllActions()
				self.m_spGameTrumpetBg:removeFromParent()
			end
			-- 玩法按钮
			if self:getChildByName(HELP_BTN_NAME) then
				self:removeChildByName(HELP_BTN_NAME)
			end
			-- 移除语音按钮
			if self:getChildByName(VOICE_BTN_NAME) then
				self:removeChildByName(VOICE_BTN_NAME)
			end
			-- 移除语音
			self:cancelVoiceRecord()
			if nil ~= self._gameFrame and type(self._gameFrame.clearVoiceQueue) == "function" then
				self._gameFrame:clearVoiceQueue()
			end
			
		elseif contextProxy:findContextByName(VIEW_LIST.ROOM_TABLE_LAYER) then
			local findContext = contextProxy:findContextByName(VIEW_LIST.ROOM_TABLE_LAYER)
			local viewCompoment = findContext:getView()
			self._gameFrame:setViewFrame(viewCompoment)
		else
			--self:onChangeShowMode(yl.SCENE_ROOM, self.m_bQuickStart)
			--self.m_bQuickStart = false
			--MXM点击初中高场次 直接进入游戏
			local t,c = yl.INVALID_TABLE,yl.INVALID_CHAIR
			-- 找桌
			local bGet = false
			for k,v in pairs(self._gameFrame._tableStatus) do
				-- 未锁 未玩			
				if v.cbTableLock == 0 and v.cbPlayStatus == 0 then
					local st = k - 1
					local chaircount = self._gameFrame._wChairCount
					for i = 1, chaircount  do					
						local sc = i - 1
						if nil == self._gameFrame:getTableUserItem(st, sc) then
							t = st
							c = sc
							bGet = true
							break
						end
					end
				end

				if bGet then
					break
				end
			end
			print( " fast enter " .. t .. " ## " .. c)
			if self._gameFrame:SitDown(t,c) then
				self:showGameWait()
			end
			
		end
	end
end

--开始保持活动连接
function ClientScene:startKeepAlive()

    if self._scheduleKeepAlive then
        return
    end

    print("开始保持活动连接")
    --定时发送内核检测数据（手机版在收到内核检测后，没有主动回复，所以在这里定时发送防止掉线）
    self._scheduleKeepAlive = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()

        if self._gameFrame:isSocketServer() then

            print("发送心跳检测包...")
			self:dismissNetWait()
			self:dismissGameWait()
            local dataBuffer = CCmd_Data:create(0)
	        dataBuffer:setcmdinfo(0,1)
            self._gameFrame:sendSocketData(dataBuffer)
        end
    end, 5, false)
end

--停止保持活动连接
function ClientScene:stopKeepAlive()
    
    if self._scheduleKeepAlive then
        
        print("停止保持活动连接")
		self:dismissNetWait()
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._scheduleKeepAlive) 
        self._scheduleKeepAlive = nil
    end
end

function ClientScene:onEnterTable()
	print("ClientScene onEnterTable")

	if PriRoom and GlobalUserItem.bPrivateRoom then
		-- 动作记录
		PriRoom:getInstance().m_nLoginAction = PriRoom.L_ACTION.ACT_ENTERTABLE
	end
	
	local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
	--查找是否游添加过游戏视图
	local findContext = contextProxy:findContextByName(VIEW_LIST.GAME_LAYER)
	if (findContext ~= nil) then
		self._gameFrame:setViewFrame(findContext:getView())
	else
		local entergame = self:getEnterGameInfo()
		if nil ~= entergame then
			local viewClassPath = appdf.GAME_SRC.. entergame._KindName .. "src.views.GameLayer"
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {viewClassPath = viewClassPath, ctor = {self._gameFrame, self}}, VIEW_LIST.GAME_LAYER)
--[[			if viewClass then
				dst = viewClass:create(self._gameFrame,self)				
			end
			if PriRoom and nil ~= dst and true == GlobalUserItem.bPrivateRoom then
				PriRoom:getInstance():enterGame(dst, self)
			end--]]
		else
			assert(false, "游戏记录错误")
		end
	end		
end

--启动游戏
function ClientScene:onStartGame()
	local entergame = self:getEnterGameInfo()
	if nil == entergame then
		showToast(self, "游戏信息获取失败", 3)
		return
	end
	self:getEnterGameInfo().nEnterRoomIndex = GlobalUserItem.nCurRoomIndex
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})	
	
--[[	--MXM判断私人房跟普通金币房加载等待不一样
	if PriRoom and true == GlobalUserItem.bPrivateRoom then
		self:showPopWait()
	end--]]
	
	self._gameFrame:onInitData()
	self._gameFrame:setKindInfo(GlobalUserItem.getCurGameKind(), entergame._KindVersion)
	--self._gameFrame:setViewFrame(self)
	self._gameFrame:onCloseSocket()
	self._gameFrame:onLogonRoom()
end

function ClientScene:onCleanPackage(name)
	if not name then
		return
	end
	for k ,v in pairs(package.loaded) do
		if k ~= nil then 
			if type(k) == "string" then
				if string.find(k,name) ~= nil or string.find(k,name) ~= nil then
					print("package kill:"..k) 
					package.loaded[k] = nil
				end
			end
		end
	end
end

function ClientScene:onLevelCallBack(result,msg)
	if type(msg) == "string" and "" ~= msg then
		showToast(self, msg, 2)
	end

	self:dismissPopWait()
	self._level:setString(GlobalUserItem.wCurrLevelID.."")

	local levelpro = ExternalFun.seekWigetByNameEx(csbNode, "bar_progress")

	if GlobalUserItem.dwUpgradeExperience > 0 then
		local scalex = GlobalUserItem.dwExperience/GlobalUserItem.dwUpgradeExperience
		if scalex > 1 then
			scalex = 1
		end
		for _,v in ipairs(levelpro) do
			v:setPercent(100 * scalex)
		end			
	else
		for _,v in ipairs(levelpro) do
			v:setPercent(1)
		end				
	end

	if 1 == result then
		if nil ~= self._levelFrame and self._levelFrame:isSocketServer() then
			self._levelFrame:onCloseSocket()
			self._levelFrame = nil
		end
	end
end

function ClientScene:onUserInfoChange( event  )
	print("----------userinfo change notify------------")

	local msgWhat = event.obj
	
	if nil ~= msgWhat and msgWhat == yl.RY_MSG_USERWEALTH then
		--兼容旧版更新写法
		
		--通知更新UI银行金币
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_INSURE_SCORE, GlobalUserItem.lUserInsure)
		--通知更新UI用户金币
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_USER_SCORE, GlobalUserItem.lUserScore)
	end
end
function ClientScene:onKeyBack()
	if not self.m_bEnableKeyBack then
		return
	end
	
	local curScene = self._sceneRecord[#self._sceneRecord-1] 
	if curScene then
		if curScene == yl.SCENE_ROOM then
			self._gameFrame:onCloseSocket()
			GlobalUserItem.nCurRoomIndex = -1
		elseif curScene == yl.SCENE_GAMELIST then
			self._gameFrame:onCloseSocket()--MXM
			GlobalUserItem.nCurRoomIndex = -1 --MXM
			if PriRoom then
				PriRoom:getInstance():exitRoom()
			end
		elseif curScene == yl.SCENE_ROOMLIST then --MXM为了清除卡线锁表
			self._gameFrame:onCloseSocket()
			GlobalUserItem.nCurRoomIndex = -1
			--停止保持活动连接
			self:stopKeepAlive()
			cc.UserDefault:getInstance():setIntegerForKey("goldkindid", GlobalUserItem.nCurGameKind)--]]
		end
	end
	
	
	
	self:onChangeShowMode()
end

function ClientScene:resetClientData()
	self._sceneLayer:setKeyboardEnabled(false)
	GlobalUserItem.nCurRoomIndex = -1
	self:updateEnterGameInfo(nil)
	--self:getApp():enterSceneEx(appdf.CLIENT_SRC.."plaza.views.LogonScene","FADE",1)
	--AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.CLIENT_SCENE})	
	GlobalUserItem.reSetData()
	--读取配置
	GlobalUserItem.LoadData()
	--断开好友服务器
	FriendMgr:getInstance():reSetAndDisconnect()
	--通知管理
	NotifyMgr:getInstance():clear()
	-- 私人房数据
	if PriRoom then
		PriRoom:getInstance():reSet()
	end

	--移除极光推送
	MultiPlatform:getInstance():deleteAlias()
end

--按钮事件
function ClientScene:onButtonClickedEvent(tag,ref)
	ExternalFun.playClickEffect()
	if tag == ClientScene.BT_EXIT then
		self:onKeyBack()		
	elseif tag == ClientScene.BT_QUICKSTART then
		if GlobalUserItem.isAngentAccount() then
			return
		end
		--判断当前场景
		local curScene = self._sceneRecord[#self._sceneRecord]
		if PriRoom and not PriRoom.enableQuickStart( curScene ) then
			--showToast(self, "房卡房间不支持快速开始！", 2)
			return
		end

		--默认使用第一个游戏
		local entergame = self:getEnterGameInfo()
		if nil == entergame then
			entergame = appConfigProxy._gameList[1]
			self:updateEnterGameInfo(entergame)
		end
		if nil == entergame then
			print("未找到游戏信息")
			return
		end
		--快速开始
		self.m_bQuickStart = true	

		--游戏列表
		if curScene == yl.SCENE_GAMELIST then			
			self:quickStartGame()
		elseif curScene == yl.SCENE_ROOMLIST then 			--游戏房间列表	
			GlobalUserItem.nCurRoomIndex = entergame.nEnterRoomIndex or GlobalUserItem.normalRoomIndex(entergame._KindID)
			local roominfo = GlobalUserItem.GetRoomInfo(GlobalUserItem.nCurRoomIndex)
			if nil == roominfo then
				showToast(self, "房间信息获取失败！", 2)
				return
			end
			if roominfo.wServerType == yl.GAME_GENRE_PERSONAL then
				--showToast(self, "房卡房间不支持快速开始！", 2)
				return
			end
			if self:roomEnterCheck() then
				--进入房间
				self:onStartGame()
			end
		elseif curScene == yl.SCENE_ROOM then 				--房间桌子列表
			--坐下
			self:onEnterRoom()
		end
	else
		if tag ~= ClientScene.BT_CONFIG and tag ~= ClientScene.BT_PERSON then
			if GlobalUserItem.isAngentAccount() then
				return
			end
		end

--[[		if #self._sceneRecord > 0 then
			local curScene = self._sceneRecord[#self._sceneRecord]
			if curScene == yl.SCENE_ROOM then
				if tag == ClientScene.BT_QUICKSTART  then --自动找位
					local room = self._sceneLayer:getChildByTag(curScene)
					if room then
						room:onQuickStart()
					end
					return					
				end
			end
		end--]]
        
   
		if tag == ClientScene.BT_CONFIG then
			--self:onChangeShowMode(yl.SCENE_OPTION)
--[[			local _optionlayer = Option:create(self)
			self._csbNode:addChild(_optionlayer)	--]]
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SETTING_LAYER)
			
        elseif tag == ClientScene.BT_MOREMENU  	then --按钮：更多菜单
			if (not self.m_plazaAni:isPlaying()) then
				local moreBg = ExternalFun.seekWigetByName(self._csbNode, "more_bg")
				if (moreBg:isVisible()) then
					self.m_plazaAni:play("close_setting", false)
				else
					self.m_plazaAni:play("open_setting", false)
				end
			end
		elseif tag == ClientScene.BT_KEFU  then --按钮：客服
			--userid,signature,time,url 
			--userid=用户ID,signature=签名,time=签名时间,url = weburl
			--headurl
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, "sound/kefu.mp3")
			local strUserID = string.format("%d", GlobalUserItem.dwUserID)
			local time = os.time()
			local strTime = string.format("%d", time)
			MultiPlatform:getInstance():presentServiceViewController(strUserID,GlobalUserItem:getSignature(time),strTime,BaseConfig.WEB_HTTP_URL,"")

 		elseif tag == ClientScene.BT_USER then
			self:onChangeShowMode(yl.SCENE_USERINFO)
		elseif tag == ClientScene.BT_BANK then
			-- 当前金币场
			local rom = GlobalUserItem.GetRoomInfo()
			if nil ~= rom then
				if rom.wServerType ~= yl.GAME_GENRE_GOLD then
					showToast(self, "当前房间禁止操作银行!", 2)
					return
				end
			end
			
			if 0 == GlobalUserItem.cbInsureEnabled then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {parentName = VIEW_LIST.BANK_LAYER}, VIEW_LIST.BANK_OPEN_LAYER)
			else
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.BANK_LAYER)
			end			
			--self:onChangeShowMode(yl.SCENE_BANK)		
		elseif tag == ClientScene.BT_RANK then
			--local param = self.m_nPreTag
			self:onChangeShowMode(yl.SCENE_RANKINGLIST, param)
		elseif tag == ClientScene.BT_TASK then
			--[[if false == GlobalUserItem.bEnableTask then
				showToast(self, "当前功能暂未开放,敬请期待!", 2)
				return
			end
			NotifyMgr:getInstance():hideNotify(self.m_btnTask)--]]
			if true == GlobalUserItem.bEnableTask then
				--AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {self._gameFrame,self.m_curGameKind}, canrepeat = false}, VIEW_LIST.TASKLAYER)
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {self._gameFrame,0}, canrepeat = false}, VIEW_LIST.TASKLAYER)
			end
			--NotifyMgr:getInstance():hideNotify(self.m_btnTask)
			--self:onChangeShowMode(yl.SCENE_TASK)
		elseif self.cur_Scene == yl.SCENE_BANKRECORD then
			self:onChangeShowMode(yl.SCENE_BANK)
        elseif self.cur_Scene == yl.SCENE_CASHRECORD then
			self:onChangeShowMode(yl.SCENE_CASH)
		elseif tag == ClientScene.BT_CHECKIN then
			--self:onChangeShowMode(yl.SCENE_CHECKIN)
			local _checkinlayer = Checkin:create(self)
			self._csbNode:addChild(_checkinlayer)
		elseif tag == ClientScene.BT_RECHARGE then
			--self:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_ENTITY)
			if (appConfigProxy._appStoreSwitch == 0) then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_LAYER)
			else
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_APPSTORE_LAYER)
			end			
		elseif tag == ClientScene.BT_BEAN then
			if (appConfigProxy._appStoreSwitch == 0) then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_LAYER)
			else
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_APPSTORE_LAYER)
			end		
		elseif tag == ClientScene.BT_EXCHANGE then
			if 0 == GlobalUserItem.cbInsureEnabled then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.BANK_OPEN_LAYER)
			else
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {3}}, VIEW_LIST.BANK_LAYER)
			end	
		elseif tag == ClientScene.BT_BOX then
			--self:onChangeShowMode(yl.SCENE_EVERYDAY)--屏蔽宝箱里功能
		elseif tag == ClientScene.BT_BAG then
			self:onChangeShowMode(yl.SCENE_BAG)			
		elseif tag == ClientScene.BT_FRIEND then
			NotifyMgr:getInstance():hideNotify(self.m_btnFriend)
			self:onChangeShowMode(yl.SCENE_FRIEND)
		elseif tag == ClientScene.BT_TRUMPET then			
			--self:getTrumpetSendLayer()--不调用喇叭
		elseif tag == ClientScene.BT_RULE then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.GAME_RULE)
		elseif tag == ClientScene.BT_PERSON then
			--self:onChangeShowMode(yl.SCENE_USERINFO)
			if(appConfigProxy._appStoreSwitch == 0) then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.PERSON_LAYER)
			end		
		elseif tag == ClientScene.BT_SHOP then
			--self:onChangeShowMode(yl.SCENE_SHOP)
			if (appConfigProxy._appStoreSwitch == 0) then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_LAYER)
			else
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_APPSTORE_LAYER)
			end		
		elseif tag == ClientScene.BT_SHARE then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHARE_LAYER)
			--self:onChangeShowMode(yl.SCENE_SHARE)
			--self:showMenu()
		elseif tag == ClientScene.BT_ACTIVITY then
			--AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW,{transition = "FADE", time = 1})
			if(appConfigProxy._appStoreSwitch == 0) then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.ACTIVITY_LAYER)
			else
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.ACTIVITY_IOS_LAYER)
			end					
		elseif tag == ClientScene.BT_EARN then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.EARN_LAYER)
		elseif tag == ClientScene.BT_MATCH then		
			--local activityLayer = ActivityLayer:create(self._csbNode)
			--self._csbNode:addChild(activityLayer)
		elseif tag == ClientScene.BT_PRIBRAND then
			if PriRoom then
				--玩家是否第一次进入私人房游戏
				local bprifristenter = cc.UserDefault:getInstance():getBoolForKey("prifristenter",true)
				if bprifristenter then
					--默认使用第一个游戏
					local entergame = appConfigProxy._gameList[1]
					if nil ~= entergame then
						local _kindID = tonumber(entergame._KindID)
						GlobalUserItem.setCurGameKind(_kindID)
						--默认第一个私人房游戏
						cc.UserDefault:getInstance():setIntegerForKey("prikindid", _kindID)
					end
					
					cc.UserDefault:getInstance():setBoolForKey("prifristenter",GlobalUserItem.bpriFristAble)  
				else
					local prikindid = cc.UserDefault:getInstance():getIntegerForKey("prikindid")
					GlobalUserItem.setCurGameKind(prikindid)
					
					--设置三个牛牛用同一个创建界面
					if prikindid == 27 or prikindid == 28 or prikindid == 36 then
						prikindid = 27
					end
					self:loadPriGameList(prikindid)
				end
				self.m_actStartTargetGameAni:play(string.format("%d", GlobalUserItem.nCurGameKind),true)
				self:onChangeShowMode(PriRoom.LAYTAG.LAYER_ROOMLIST)
			end
		elseif tag == ClientScene.BT_GONGGAO then
			if(appConfigProxy._appStoreSwitch == 0) then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.GONGGAO_LAYER)
			else
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.GONGGAO_IOS_LAYER)
			end				
		elseif tag == ClientScene.BT_INSURE_ADD then
			if 0 == GlobalUserItem.cbInsureEnabled then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.BANK_OPEN_LAYER)
			else
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.BANK_LAYER)
			end	
		elseif tag == ClientScene.BT_KAIHU then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, "sound/first_bind.mp3")
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {canrepeat = false}, VIEW_LIST.VISITOR_BIND_LAYER)
		else
			showToast(self,"功能尚未开放，敬请期待！",2)
		end
	end
end

--切换页面
function ClientScene:onChangeShowMode(nTag, param)
	assert(false, "function desprecated!")
end

--获取页面
function ClientScene:getTagLayer(tag, param)
	local dst
	if tag == yl.SCENE_GAMELIST then
		--dst =  GameListView:create(appConfigProxy._gameList)
	elseif tag == yl.SCENE_ROOMLIST then
		--是否有自定义房间列表
		local entergame = self:getEnterGameInfo()
		if nil ~= entergame then
			local modulestr = string.gsub(entergame._KindName, "%.", "/")
			local targetPlatform = cc.Application:getInstance():getTargetPlatform()
			local customRoomFile = ""
			if cc.PLATFORM_OS_WINDOWS == targetPlatform then
				customRoomFile = "game/" .. modulestr .. "src/views/GameRoomListLayer.lua"
			else
				customRoomFile = "game/" .. modulestr .. "src/views/GameRoomListLayer.luac"
			end
			if cc.FileUtils:getInstance():isFileExist(customRoomFile) then
				dst = appdf.req(customRoomFile):create(self, self._gameFrame, param)
			end
		end
		if nil == dst then
			dst = RoomList:create(self, param)
		end		
	elseif tag == yl.SCENE_USERINFO then
		dst = UserInfo:create(self)
	elseif tag == yl.SCENE_OPTION then
		dst = Option:create(self)
	elseif tag == yl.SCENE_BANK then
		dst = Bank:create(self, self._gameFrame)
	elseif tag == yl.SCENE_RANKINGLIST then
		dst = RankingList:create(self, param)
	elseif tag == yl.SCENE_TASK then
		dst = Task:create(self, self._gameFrame, self.m_curGameKind)
	elseif tag == yl.SCENE_ROOM then
		dst = Room:create(self._gameFrame,self, param)
	elseif tag == yl.SCENE_GAME then
		local entergame = self:getEnterGameInfo()
		if nil ~= entergame then
			local modulestr = entergame._KindName
			local gameScene = appdf.req(appdf.GAME_SRC.. modulestr .. "src.views.GameLayer")
			if gameScene then
				dst = gameScene:create(self._gameFrame,self)				
			end
			if PriRoom and nil ~= dst and true == GlobalUserItem.bPrivateRoom then
				PriRoom:getInstance():enterGame(dst, self)
			end
		else
			print("游戏记录错误")
		end			

    elseif tag == yl.SCENE_CHECKIN then
		dst = Checkin:create(self)
	elseif tag == yl.SCENE_BANKRECORD then
		dst = BankRecord:create(self)
    elseif tag == yl.SCENE_CASHRECORD then
		dst = CashRecord:create(self)
	elseif tag == yl.SCENE_ORDERRECORD then
		dst = OrderRecord:create(self)
	elseif tag == yl.SCENE_EVERYDAY then
		dst = EveryDay:create(self)
	elseif tag == yl.SCENE_AGENT then
		dst = Agent:create(self)
	elseif tag == yl.SCENE_SHOP then
		dst = Shop:create(self, param)
	elseif tag == yl.SCENE_SHOPDETAIL then
		dst = ShopDetail:create(self, self._gameFrame)
	elseif tag == yl.SCENE_BAG then
		dst = Bag:create(self, self._gameFrame)
	elseif tag == yl.SCENE_BAGDETAIL then
		dst = BagDetail:create(self, self._gameFrame)
	elseif tag == yl.SCENE_BAGTRANS then
		dst = BagTrans:create(self, self._gameFrame)
	elseif tag == yl.SCENE_BINDING then
		dst = Binding:create(self)
	elseif tag == yl.SCENE_FRIEND then
		dst = Friend:create(self)
	elseif tag == yl.SCENE_MODIFY then				
		dst = ModifyPasswd:create(self)
	elseif tag == yl.SCENE_TABLE then
		dst = TableLayer:create(self)
	elseif tag == yl.SCENE_FEEDBACK then
		dst = FeedbackLayer:create(self)
	elseif tag == yl.SCENE_FEEDBACKLIST then
		dst = FeedbackLayer.createFeedbackList(self)
	elseif tag == yl.SCENE_FAQ then
		dst = FaqLayer:create(self)
	elseif tag == yl.SCENE_BINDINGREG then
		dst = BindingRegisterLayer:create(self)
	elseif tag == yl.SCENE_ACTIVITY then
		dst = ActivityLayer:create(self)
	elseif tag == yl.SCENE_EARN then
		dst = EarnMoney:create(self)
	elseif tag == yl.SCENE_SHARE then
		dst = PromoterInputLayer:create(self._csbNode)
	elseif PriRoom then
		dst = PriRoom:getInstance():getTagLayer(tag, param, self)
	end
	if dst then
		dst:setTag(tag)
	end
	return dst
end

--显示等待
function ClientScene:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function ClientScene:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

--显示等待
function ClientScene:showGameWait()
--[[	if not self._gameWait then
		local csbPath = "EnterGame/EnterGameNode.csb"
		self._gameWait = ExternalFun.loadCSB(csbPath, self)
		self._gameWait:setPosition(yl.WIDTH / 2, yl.HEIGHT / 2)
		self._gameWaitAni = ExternalFun.loadTimeLine(csbPath)
		self._gameWait:runAction(self._gameWaitAni)
		self._gameWaitAni:setLastFrameCallFunc(function() 
			self._gameWaitAni:play("repeatGameWatiting", true)
			self._gameWaitAni:clearLastFrameCallFunc()
		end)
		self._gameWaitAni:play("showGameWait", false)
		self._gameWait:setLocalZOrder(yl.MAX_INT)
	end--]]
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {canrepeat = false}, VIEW_LIST.GAME_WAIT_LAYER)	
end

--关闭等待
function ClientScene:dismissGameWait()
--[[	if self._gameWait then
		self._gameWaitAni:setLastFrameCallFunc(function() 
			self._gameWaitAni:clearLastFrameCallFunc()
			self._gameWait:removeFromParent()
			self._gameWait = nil
		end)
		self._gameWaitAni:play("hideGameWait", false)
	end--]]
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.GAME_WAIT_LAYER})	
end

--显示网络连接等待
function ClientScene:showNetWait()
	if not self._netWait then
		local csbPath = "NetWait/NetWaitNode.csb"
		self._netWait = ExternalFun.loadCSB(csbPath, self)
		self._netWait:setPosition(yl.WIDTH / 2, yl.HEIGHT / 2)
		self._netWaitAni = ExternalFun.loadTimeLine(csbPath)
		self._netWait:runAction(self._netWaitAni)
		self._netWaitAni:setLastFrameCallFunc(function() 
			self._netWaitAni:play("repeatNetWatiting", true)
			self._netWaitAni:clearLastFrameCallFunc()
		end)
		self._netWaitAni:play("showNetWait", false)
		self._netWait:setLocalZOrder(yl.MAX_INT)
	end
end

--关闭网络连接等待
function ClientScene:dismissNetWait()
	if self._netWait then
		self._netWaitAni:setLastFrameCallFunc(function() 
			self._netWaitAni:clearLastFrameCallFunc()
			self._netWait:removeFromParent()
			self._netWait = nil
		end)
		self._netWaitAni:play("hideNetWait", false)
	end
end

--更新进入游戏记录
function ClientScene:updateEnterGameInfo( info )
	GlobalUserItem.m_tabEnterGame = info
end

function ClientScene:getEnterGameInfo(  )
	return GlobalUserItem.m_tabEnterGame
end

--获取游戏信息
function ClientScene:getGameInfo(wKindID)
	for k,v in pairs(appConfigProxy._gameList) do
		if tonumber(v._KindID) == tonumber(wKindID) then
			return v
		end
	end
	return nil
end

--获取喇叭发送界面
function ClientScene:getTrumpetSendLayer()
	if nil == self.m_trumpetLayer then
		self.m_trumpetLayer = TrumpetSendLayer:create(self)
		self._csbNode:addChild(self.m_trumpetLayer)
	end
	self.m_trumpetLayer:showLayer(true)
end

function ClientScene:getSceneRecord(  )
	return self._sceneRecord
end

--缓存公共资源
function ClientScene:cachePublicRes(  )
	cc.SpriteFrameCache:getInstance():addSpriteFrames("public/public.plist")
	local dict = cc.FileUtils:getInstance():getValueMapFromFile("public/public.plist")

	local framesDict = dict["frames"]
	if nil ~= framesDict and type(framesDict) == "table" then
		for k,v in pairs(framesDict) do
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
			if nil ~= frame then
				frame:retain()
			end
		end
	end

	cc.SpriteFrameCache:getInstance():addSpriteFrames("plaza/plaza.plist")	
	dict = cc.FileUtils:getInstance():getValueMapFromFile("plaza/plaza.plist")
	framesDict = dict["frames"]
	if nil ~= framesDict and type(framesDict) == "table" then
		for k,v in pairs(framesDict) do
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
			if nil ~= frame then
				frame:retain()
			end
		end
	end
end

--释放公共资源
function ClientScene:releasePublicRes(  )
	local dict = cc.FileUtils:getInstance():getValueMapFromFile("public/public.plist")
	local framesDict = dict["frames"]
	if nil ~= framesDict and type(framesDict) == "table" then
		for k,v in pairs(framesDict) do
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
			if nil ~= frame and frame:getReferenceCount() > 0 then
				frame:release()
			end
		end
	end

	dict = cc.FileUtils:getInstance():getValueMapFromFile("plaza/plaza.plist")
	framesDict = dict["frames"]
	if nil ~= framesDict and type(framesDict) == "table" then
		for k,v in pairs(framesDict) do
			local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame(k)
			if nil ~= frame and frame:getReferenceCount() > 0 then
				frame:release()
			end
		end
	end
	cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("public/public.plist")
	cc.Director:getInstance():getTextureCache():removeTextureForKey("public/public.png")
	cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("plaza/plaza.plist")
	cc.Director:getInstance():getTextureCache():removeTextureForKey("plaza/plaza.png")
	cc.Director:getInstance():getTextureCache():removeUnusedTextures()
	cc.SpriteFrameCache:getInstance():removeUnusedSpriteFrames()
end

--输出日志
function ClientScene:logData(msg, addExtral)
	addExtral = addExtral or false
	local logtable = {}
	local entergame = self:getEnterGameInfo()
	if nil ~= entergame then
		logtable.name = entergame._KindName
		logtable.id = entergame._KindID
	end
	logtable.msg = msg
	local jsonStr = cjson.encode(logtable)
	LogAsset:getInstance():logData(jsonStr,true)
end

--快速登录逻辑
function ClientScene:quickStartGame()	
	--进入游戏房间/进入第一个桌子
	local gamelist = self._sceneLayer:getChildByTag(yl.SCENE_GAMELIST)
	local gameinfo = self:getEnterGameInfo()
	if nil ~= gamelist and nil ~= gameinfo then
		--进入游戏第一个房间
		GlobalUserItem.nCurRoomIndex = gameinfo.nEnterRoomIndex
		if nil == GlobalUserItem.nCurRoomIndex or -1 == GlobalUserItem.nCurRoomIndex then
			GlobalUserItem.nCurRoomIndex = GlobalUserItem.normalRoomIndex(gameinfo._KindID)
		end
		if not GlobalUserItem.nCurRoomIndex then
			showToast(self, "未找到房间信息!", 2)
			return
		end

		--获取更新
		if not self:updateGame() then
			--获取房间信息
			local roomCount = GlobalUserItem.GetGameRoomCount(gameinfo._KindID)
			if not roomCount or 0 == roomCount then
				--gamelist:onLoadGameList(gameinfo._KindID)
				print("ClientScene:quickStartGame 房间列表为空")
			end
			GlobalUserItem.setCurGameKind(tonumber(gameinfo._KindID))
			GlobalUserItem.szCurGameName = gameinfo._KindName			

			local roominfo = GlobalUserItem.GetRoomInfo(GlobalUserItem.nCurRoomIndex)
			if roominfo.wServerType == yl.GAME_GENRE_PERSONAL then
				--showToast(self, "房卡房间不支持快速开始！", 2)
				return
			end
			if self:roomEnterCheck() then
				--启动游戏
				self:onStartGame()
			end
		end	
	end
end

-- 游戏更新处理
function ClientScene:updateGame(dwKindID)
	local gameinfo = self:getEnterGameInfo()
	if nil ~= dwKindID then
		gameinfo = self:getGameInfo(dwKindID)
	end
	if nil == gameinfo then
		return false
	end
	local gamelist = self._sceneLayer:getChildByTag(yl.SCENE_GAMELIST)
	if nil == gamelist then
		return false
	end

	--获取更新
	local version = versionProxy:getResVersion(gameinfo._KindID)

	if version then
		if gameinfo._ServerResVersion > version then
			gamelist:updateGame(gameinfo, gameinfo.gameIndex)
			return true
		else
			gamelist:onEnterGame(gameinfo, false)
		end
	else
		if (app._appStoreSwitch == 0) then
			gamelist:updateGame(gameinfo, gameinfo.gameIndex)
			return true
		else
			gamelist:onEnterGame(gameinfo, false)
		end
	end	
	
	return false
end

-- 链接游戏
function ClientScene:loadGameList(dwKindID)
	local gameinfo = self:getEnterGameInfo()
	if nil ~= dwKindID then
		gameinfo = self:getGameInfo(dwKindID)
	end
	if nil == gameinfo then
		return false
	end
	local gamelist = self._sceneLayer:getChildByTag(yl.SCENE_GAMELIST)
	if nil == gamelist then
		return false
	end

	self:updateEnterGameInfo(gameinfo)
	local roomCount = GlobalUserItem.GetGameRoomCount(gameinfo._KindID)
	if not roomCount or 0 == roomCount then
		--gamelist:onLoadGameList(gameinfo._KindID)
		return true
	end
	return false
end

-- 链接私人游戏
function ClientScene:loadPriGameList(dwKindID)
	local gameinfo = self:getEnterGameInfo()
	if nil ~= dwKindID then
		gameinfo = self:getGameInfo(dwKindID)
	end
	if nil == gameinfo then
		return false
	end
	--[[local gamelist = self._sceneLayer:getChildByTag(yl.SCENE_GAMELIST)
	if nil == gamelist then
		return false
	end--]]

	self:updateEnterGameInfo(gameinfo)
	local roomCount = GlobalUserItem.GetGameRoomCount(gameinfo._KindID)
	if not roomCount or 0 == roomCount then
		--gamelist:onLoadGameList(gameinfo._KindID)
		return true
	end
	return false
end

function ClientScene:roomEnterCheck()
	local roominfo = GlobalUserItem.GetRoomInfo(GlobalUserItem.nCurRoomIndex)

	-- 密码
	if bit.band(roominfo.wServerKind, yl.SERVER_GENRE_PASSWD) ~= 0 then
		self.m_bEnableKeyBack = false
		self:createPasswordEdit("请输入房间密码", function(pass)
			self.m_bEnableKeyBack = true
			GlobalUserItem.szRoomPasswd = pass
			self:onStartGame()
		end, "RoomList/sp_pwroom_title.png")
		return false
	end

	-- 比赛
	if bit.band(roominfo.wServerType ,yl.GAME_GENRE_MATCH )  ~= 0 then
		showToast(self,"暂不支持比赛房间！",1)
		return false
	end
	return true
end

--网络通知
function ClientScene:onNotify(msg)
	local bHandled = false
	local main = msg.main or 0
	local sub = msg.sub or 0
	local name = msg.name or ""
	local param = msg.param
	local group = msg.group
	
	if group == "client_trumpet" 
		and type(msg.param) == "table" then
		--喇叭消息获取		--
		bHandled = true

		--当前场景为游戏
		local curScene = self._sceneRecord[#self._sceneRecord]		
		if curScene == yl.SCENE_GAME then
			table.insert(self.m_gameTrumpetList, item.str)

			local chat = {}
			chat.szNick = msg.param.szNickName
			chat.szChatString = msg.param.szMessageContent
			GameChatLayer.addChatRecordWith(chat)

			if nil == self.m_spGameTrumpetBg then
				local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_trumpet_bg.png")
				if nil ~= frame then
					local trumpetBg = cc.Sprite:createWithSpriteFrame(frame)
					local runScene = cc.Director:getInstance():getRunningScene()
					if nil ~= runScene and nil ~= trumpetBg then 
						self.m_spGameTrumpetBg = trumpetBg
						trumpetBg:setScaleX(0.0001)
						runScene:addChild(trumpetBg)
						local trumpetsize = trumpetBg:getContentSize()
						trumpetBg:setPosition(appdf.WIDTH * 0.5, appdf.HEIGHT - trumpetsize.height * 0.5)
						local stencil  = display.newSprite()
							:setAnchorPoint(cc.p(0,0.5))
						stencil:setTextureRect(cc.rect(0,0,700,50))
						local notifyClip = cc.ClippingNode:create(stencil)
							:setAnchorPoint(cc.p(0,0.5))
						notifyClip:setInverted(false)
						notifyClip:move(50,30)
						notifyClip:addTo(trumpetBg)
						local notifyText = cc.Label:createWithTTF("", "base/fonts/round_body.ttf", 24)
													:addTo(notifyClip)
													:setTextColor(cc.c4b(255,191,123,255))
													:setAnchorPoint(cc.p(0,0.5))
													:enableOutline(cc.c4b(79,48,35,255), 1)
													:move(700,0)
						self.m_spGameTrumpetBg.trumpetText = notifyText

						trumpetBg:runAction(cc.Sequence:create(cc.ScaleTo:create(0.5,1.0), cc.CallFunc:create(function()
							self:onGameTrumpet()
						end)))	
					end
				end
			end		
		else
			self.m_gameTrumpetList = {}
		end	
	elseif group == "client_task" then 		-- 任务
		bHandled = true
		--当前场景中不存在任务界面
		local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
		local currentContext = contextProxy:findContextByName(VIEW_LIST.TASKLAYER)
		if (currentContext ~= nil and GlobalUserItem.bEnableTask) then
			--NotifyMgr:getInstance():showNotify(self.m_btnTask, msg, cc.p(254, 88))
		end		
	elseif group == "client_friend" then
		bHandled = true
		local curScene = self._sceneRecord[#self._sceneRecord]		
		if curScene ~= yl.SCENE_FRIEND then
			NotifyMgr:getInstance():showNotify(self.m_btnFriend, msg, cc.p(254, 88))
		end
	end
	return bHandled
end

function ClientScene:onGameTrumpet()
	if nil ~= self.m_spGameTrumpetBg and 0 ~= #self.m_gameTrumpetList then
		local str = self.m_gameTrumpetList[1]
		table.remove(self.m_gameTrumpetList,1)		
		local text = self.m_spGameTrumpetBg.trumpetText						
		if nil ~= text then
			text:setString(str)
			text:setPosition(cc.p(700, 0))
			text:stopAllActions()
			local tmpWidth = text:getContentSize().width
			text:runAction(cc.Sequence:create(cc.MoveTo:create(16 + (tmpWidth / 172),cc.p(0-text:getContentSize().width,0)),cc.CallFunc:create(function()
				if 0 ~= #self.m_gameTrumpetList then
					self:onGameTrumpet()
				else
					self.m_spGameTrumpetBg:runAction(cc.Sequence:create(cc.ScaleTo:create(0.5, 0.0001, 1.0), cc.CallFunc:create(function()
						self.m_spGameTrumpetBg:removeFromParent()
						self.m_spGameTrumpetBg = nil
					end)))
				end														
			end)))
		end
	end
end

local BT_CLOSE_DLG = 1
local BT_CONFIRM = 2
function ClientScene:createPasswordEdit(placeholder, confirmFun)
	local runScene = cc.Director:getInstance():getRunningScene()
	local layout = ccui.Layout:create()
	layout:setContentSize(cc.size(appdf.WIDTH, appdf.HEIGHT))
	layout:setTouchEnabled(true)
	layout:setSwallowTouches(true)
	runScene:addChild(layout)

	local bg = display.newSprite("plaza/plaza_query_bg.png")
		:move(appdf.WIDTH/2,appdf.HEIGHT/2)
		:addTo(layout)
	local bg_size = bg:getContentSize()

	--编辑框
	local editpass = ccui.EditBox:create(cc.size(490,67), "RoomList/sp_pwedit_bg.png")
		:move(bg_size.width * 0.5,180)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(32)
		:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder(placeholder)
		:setFontColor(cc.c4b(254,164,107,255))	
		:addTo(bg)

	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			local tag = sender:getTag()
			if BT_CLOSE_DLG == tag then
				layout:removeFromParent()
				self.m_bEnableKeyBack = true
			elseif  BT_CONFIRM == tag then
				local editText = string.gsub(editpass:getText(), " ", "")
				if ExternalFun.stringLen(editText) < 1 then
					showToast(runScene, "密码不能为空", 2)
					return
				end

				if type(confirmFun) == "function" then
					confirmFun(editText)
				end
				layout:removeFromParent()
			end
		end
	end

	--标题
	local sp = cc.Sprite:create("RoomList/sp_pwtitle_room.png")
	bg:addChild(sp)
	sp:setPosition(bg_size.width * 0.5, bg_size.height - 50)

	ccui.Button:create("p_bt_close_0.png", "p_bt_close_1.png", "p_bt_close_0.png", UI_TEX_TYPE_PLIST)
		:move(bg_size.width -  50, bg_size.height - 50)
		:setTag(BT_CLOSE_DLG)
		:addTo(bg)
		:addTouchEventListener(btnEvent)

	ccui.Button:create("General/bt_confirm_0.png", "General/bt_confirm_1.png")
		:move(bg_size.width * 0.5, 50)
		:setTag(BT_CONFIRM)
		:addTo(bg)
		:addTouchEventListener(btnEvent)
end

function ClientScene:queryUserScoreInfo(queryCallBack)
	local ostime = os.time()
    local url = BaseConfig.WEB_HTTP_URL .. "/WS/MobileInterface.ashx"

    self:showPopWait()
    appdf.onHttpJsionTable(url ,"GET","action=GetScoreInfo&userid=" .. GlobalUserItem.dwUserID .. "&time=".. ostime .. "&signature=".. GlobalUserItem:getSignature(ostime),function(sjstable,sjsdata)
        self:dismissPopWait()
        dump(sjstable, "sjstable", 5)
        if type(sjstable) == "table" then
            local data = sjstable["data"]
            if type(data) == "table" then
                local valid = data["valid"]
                if true == valid then
                    local score = tonumber(data["Score"]) or 0
                    local bean = tonumber(data["Currency"]) or 0
                    local ingot = tonumber(data["UserMedal"]) or 0
                    local roomcard = tonumber(data["RoomCard"]) or 0
                    local insurescore = tonumber(data["InsureScore"]) or 0 --MXM增加银行金币

                    local needupdate = false
                    if score ~= GlobalUserItem.lUserScore 
                    	or bean ~= GlobalUserItem.dUserBeans
                    	or ingot ~= GlobalUserItem.lUserIngot
                    	or roomcard ~= GlobalUserItem.lRoomCard 
                        or insurescore ~= GlobalUserItem.lUserInsure then
                    	GlobalUserItem.dUserBeans = bean
                    	GlobalUserItem.lUserScore = score
                    	GlobalUserItem.lUserIngot = ingot
                    	GlobalUserItem.lRoomCard = roomcard
                        GlobalUserItem.lUserInsure = insurescore
                        needupdate = true
                    end
                    if needupdate then
                        print("update score")
                        --通知更新        
                        local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
                        eventListener.obj = yl.RY_MSG_USERWEALTH
                        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
                    end 
                    if type(queryCallBack) == "function" then
                    	queryCallBack(needupdate)
                    end                   
                end
            end
        end
    end)
end

function ClientScene:queryTaskInfo()--MXM签到是否一开始显示签到
	local taskResult = function(result, msg)
		if result == 1 then -- 获取到 SUB_GP_TASK_INFO
			if self.m_bFirstQueryCheckIn and nil ~= self._checkInFrame then
				self.m_bFirstQueryCheckIn = false
				if true == GlobalUserItem.bEnableCheckIn then
					--签到页面
					self._checkInFrame:onCheckinQuery()
				else
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
					-- 显示广告
					if GlobalUserItem.isShowAdNotice() then
						if(appConfigProxy._appStoreSwitch == 0) then
							local webview = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.WebViewLayer"):create(self)
							self:addChild(webview)
						end
					end
				end
			end
			--self:queryLevelInfo()--暂时屏蔽用户等级提示
			if nil ~= self._taskFrame._gameFrame then
                self._taskFrame._gameFrame._shotFrame = nil
                self._taskFrame._gameFrame = nil
            end
			self._taskFrame = nil
		end
	end
	self._taskFrame = TaskFrame:create(self, taskResult, 0)
	self._taskFrame._gameFrame = self._gameFrame
    if nil ~= self._gameFrame then
        self._gameFrame._shotFrame = self._taskFrame
    end
	self._taskFrame:onTaskLoad()
end

function ClientScene:queryLevelInfo()
	local levelCallBack = function(result,msg)
			self:onLevelCallBack(result,msg)
		end
	self._levelFrame = LevelFrame:create(self,levelCallBack)	
	self._levelFrame:onLoadLevel()
end

function ClientScene:disconnectFrame()
	if nil ~= self._shopDetailFrame and self._shopDetailFrame:isSocketServer() then
		self._shopDetailFrame:onCloseSocket()
		self._shopDetailFrame = nil
	end

	if nil ~= self._levelFrame and self._levelFrame:isSocketServer() then
		self._levelFrame:onCloseSocket()
		self._levelFrame = nil
	end

	if nil ~= self._checkInFrame and self._checkInFrame:isSocketServer() then
		self._checkInFrame:onCloseSocket()
		self._checkInFrame = nil
	end

	if nil ~= self._taskFrame and self._taskFrame:isSocketServer() then
		self._taskFrame:onCloseSocket()		

		if nil ~= self._taskFrame._gameFrame then
            self._taskFrame._gameFrame._shotFrame = nil
            self._taskFrame._gameFrame = nil
        end
        self._taskFrame = nil
	end
end

function ClientScene:coinDropDownAni( funC )	
	local runScene = cc.Director:getInstance():getRunningScene()
	if nil == runScene then
		return
	end
	ExternalFun.popupTouchFilter(1, false)
	if nil == self.m_nodeCoinAni then		
		self.m_nodeCoinAni = ExternalFun.loadCSB( "plaza/CoinAni.csb", runScene)
		self.m_nodeCoinAni:setPosition(30, yl.HEIGHT + 30)

		self.m_actCoinAni = ExternalFun.loadTimeLine( "plaza/CoinAni.csb" )
		ExternalFun.SAFE_RETAIN(self.m_actCoinAni)	
	end
	local function onFrameEvent( frame )
		if nil == frame then
            return
        end
        local str = frame:getEvent()
        print("frame event ==> "  .. str)
        if str == "drop_over" then
        	self.m_nodeCoinAni:setVisible(false)
        	self.m_nodeCoinAni:stopAllActions()
        	if type(funC) == "function" then
        		funC()
        	end
        	ExternalFun.dismissTouchFilter()
        end
	end
	self.m_actCoinAni:setFrameEventCallFunc(onFrameEvent)
		
	local child = runScene:getChildren() or {}
	local childCount = #child
	self.m_nodeCoinAni:setVisible(true)
	self.m_nodeCoinAni:setLocalZOrder(childCount + 1)
	self.m_nodeCoinAni:stopAllActions()
	self.m_actCoinAni:gotoFrameAndPlay(0,false)
	self.m_nodeCoinAni:runAction(self.m_actCoinAni)
end

function ClientScene:popTargetShare( callback )
	local TargetShareLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.TargetShareLayer")
	local lay = TargetShareLayer:create(callback)
	self:addChild(lay)
	lay:setLocalZOrder(yl.MAX_INT - 3)
end

function ClientScene:createHelpBtn(pos, zorder, url, parent)
	parent = parent or self
	zorder = zorder or (yl.MAX_INT - 2)
	url = url or BaseConfig.WEB_HTTP_URL
	local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:popHelpLayer(url, zorder)
        end
    end
	pos = pos or cc.p(100, 100)
	local btn = ccui.Button:create("pub_btn_introduce_0.png", "pub_btn_introduce_1.png", "pub_btn_introduce_0.png", UI_TEX_TYPE_PLIST)
	btn:setPosition(pos)
	btn:setName(HELP_BTN_NAME)
	btn:addTo(parent)
	btn:setLocalZOrder(zorder)
    btn:addTouchEventListener(btncallback)
end

function ClientScene:popHelpLayer( url, zorder)
	zorder = zorder or yl.MAX_INT - 1
	local IntroduceLayer = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.IntroduceLayer")
	local lay = IntroduceLayer:create(self, url)
	lay:setName(HELP_LAYER_NAME)
	self:addChild(lay)
	lay:setLocalZOrder(zorder)
end

function ClientScene:createVoiceBtn(pos, zorder, parent)
	parent = parent or self
	zorder = zorder or (yl.MAX_INT - 4)
	local function btncallback(ref, tType)
		if tType == ccui.TouchEventType.began then
			self:startVoiceRecord()
        elseif tType == ccui.TouchEventType.ended 
        	or tType == ccui.TouchEventType.canceled then
            self:stopVoiceRecord()
        end
    end
	pos = pos or cc.p(100, 100)
	local btn = ccui.Button:create("btn_voice_chat_0.png", "btn_voice_chat_1.png", "btn_voice_chat_0.png", UI_TEX_TYPE_PLIST)
	btn:setPosition(pos)
	btn:setName(VOICE_BTN_NAME)
	btn:addTo(parent)
	btn:setLocalZOrder(zorder)
    btn:addTouchEventListener(btncallback)
end

function ClientScene:startVoiceRecord()
	--防作弊不聊天
	if GlobalUserItem.isAntiCheat() then
		local runScene = cc.Director:getInstance():getRunningScene()
		showToast(runScene, "防作弊房间禁止聊天", 3)
		return
	end
	
	local lay = VoiceRecorderKit.createRecorderLayer(self, self._gameFrame)
	if nil ~= lay then
		lay:setName(VOICE_LAYER_NAME)
		self:addChild(lay)
	end
end

function ClientScene:stopVoiceRecord()
	local voiceLayer = self:getChildByName(VOICE_LAYER_NAME)
	if nil ~= voiceLayer then
		voiceLayer:removeRecorde()
	end
end

function ClientScene:cancelVoiceRecord()
	local voiceLayer = self:getChildByName(VOICE_LAYER_NAME)
	if nil ~= voiceLayer then
		voiceLayer:cancelVoiceRecord()
	end
end

-- 好友邀请
function ClientScene:inviteFriend(inviteFriend, gameKind, serverId, tableId, inviteMsg)
	local tab = {}
    tab.dwInvitedUserID = inviteFriend
    tab.wKindID = gameKind
    tab.wServerID = serverId
    tab.wTableID = tableId
    tab.szInviteMsg = inviteMsg
    FriendMgr:getInstance():sendInviteGame(tab)
end

-- 好友截图分享
function ClientScene:imageShareToFriend( toFriendId, imagepath, shareMsg )
	local param = imagepath
    if cc.FileUtils:getInstance():isFileExist(param) then
    	self:showPopWait()
        --发送上传头像
        local url = BaseConfig.WEB_HTTP_URL .. "/WS/Account.ashx?action=uploadshareimage"
        local uploader = CurlAsset:createUploader(url,param)
        if nil == uploader then
            showToast(self, "分享图上传异常", 2)
            return
        end
        local nres = uploader:addToFileForm("file", param, "image/png")
        --用户标示
        nres = uploader:addToForm("userID", GlobalUserItem.dwUserID)
        --分享用户
        nres = uploader:addToForm("suserID", toFriendId)
        --登陆时间差
        local delta = tonumber(currentTime()) - tonumber(GlobalUserItem.LogonTime)
        print("time delta " .. delta)
        nres = uploader:addToForm("time", delta .. "")
        --客户端ip
        local ip = MultiPlatform:getInstance():getClientIpAdress() or "192.168.1.1"
        nres = uploader:addToForm("clientIP", ip)
        --机器码
        local machine = GlobalUserItem.szMachine or "A501164B366ECFC9E249163873094D50"
        nres = uploader:addToForm("machineID", machine)
        --会话签名
        nres = uploader:addToForm("signature", GlobalUserItem:getSignature(delta))
        if 0 ~= nres then
            showToast(self, "上传表单提交异常,error code ==> " .. nres, 2)
            return
        end

        uploader:uploadFile(function(sender, ncode, msg)
            local logtable = {}
            logtable.msg = msg
            logtable.ncode = ncode
            local jsonStr = cjson.encode(logtable)
            LogAsset:getInstance():logData(jsonStr,true)

            local ok, msgTab = pcall(function()
				return cjson.decode(msg)
			end)
            if ok then
            	local dataTab = msgTab["data"]
            	if type(dataTab) == "table" then
            		local address = ""
            		if true == dataTab["valid"] then
            			address = dataTab["ShareUrl"] or ""
            		end
            		local tab = {}
		            tab.dwSharedUserID = toFriendId
		            tab.szShareImageAddr = address
		            tab.szMessageContent = shareMsg
		            tab.szImagePath = imagepath
		            if FriendMgr:getInstance():sendShareMessage(tab) then
		            	showToast(self, "分享成功!", 2)
		           	end
            	end
            else

            end
            self:dismissPopWait()
        end)
    else
    	showToast(self, "您要分享的图片不存在, 请重试", 2)
    end
end

return ClientScene