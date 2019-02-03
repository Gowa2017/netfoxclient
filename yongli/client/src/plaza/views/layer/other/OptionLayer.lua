--[[
设置界面
2017_8_31 MXM
功能：音乐音量震动等
]]

local OptionLayer = class("OptionLayer", cc.BaseLayer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local g_var = ExternalFun.req_var
local WebViewLayer = appdf.CLIENT_SRC .. "plaza.views.layer.plaza.WebViewLayer"
appdf.req(appdf.CLIENT_SRC.."plaza.models.FriendMgr")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")
local ModifyFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.ModifyFrame")

OptionLayer.CBT_SILENCE 	= 1
OptionLayer.CBT_SOUND   	= 2
OptionLayer.BT_EXIT			= 7

OptionLayer.BT_QUESTION		= 8
OptionLayer.BT_COMMIT		= 9
OptionLayer.BT_MODIFY		= 10
OptionLayer.BT_EXCHANGE		= 11
OptionLayer.BT_LOCK         = 12
OptionLayer.BT_UNLOCK       = 13
OptionLayer.BT_EXITPLAZA   	= 14

OptionLayer.PRO_WIDTH		= yl.WIDTH

function OptionLayer:ctor()
	self.super.ctor(self)
	
	local cbtlistener = function (sender,eventType)
		self:onSelectedEvent(sender:getTag(),sender,eventType)
	end
	local  btcallback = function (ref, type)
		if type == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(ref:getTag(),ref)
		end
	end
	
    -- 加载csb资源
	local csbPath = "Option/OptionLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._rootLayer = rootLayer
	self._csbNode = csbNode
	self.m_optionBg = csbNode:getChildByName("Option_Bg")
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)	
	
	--[[--显示背景
	display.newSprite("Option/bg_option.png")
	:move(yl.WIDTH/2,yl.HEIGHT/2)
	:addTo(self)
	
	--上方背景
	local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_top_bg.png")
	if nil ~= frame then
		local sp = cc.Sprite:createWithSpriteFrame(frame)
		sp:setPosition(yl.WIDTH/2,yl.HEIGHT-51)
		self:addChild(sp)
	end
	--标题
	display.newSprite("Option/title_option.png")
	:move(areaWidth/2,yl.HEIGHT-51)
	:addTo(self)
	--返回
	ccui.Button:create("bt_return_0.png","bt_return_1.png")
	:move(75,yl.HEIGHT-51)
	:setTag(OptionLayer.BT_EXIT)
	:addTo(self)
	:addTouchEventListener(btcallback)--]]
	
	--返回
	self._return = self.m_optionBg:getChildByName("Button_return")
	:setTag(OptionLayer.BT_EXIT)
	:addTouchEventListener(btcallback)
	
	--音效开关
	--[[display.newSprite("Option/frame_option_0.png")
	:move(1000,510)
	:addTo(self)
	display.newSprite("Option/text_sound.png")
	:move(837,510)
	:addTo(self)
	
	self._cbtSilence = ccui.CheckBox:create("Option/bt_option_switch_0.png","","Option/bt_option_switch_1.png","","")
	:move(1175,510)
	:setSelected(GlobalUserItem.bSoundAble)
	:addTo(self)
	:setTag(self.CBT_SOUND)
	self._cbtSilence:addEventListener(cbtlistener)
	
	--音乐开关
	display.newSprite("Option/frame_option_0.png")
	:move(330,510)
	:addTo(self)
	display.newSprite("Option/text_music.png")
	:move(170,510)
	:addTo(self)
	self._cbtSound = ccui.CheckBox:create("Option/bt_option_switch_0.png","","Option/bt_option_switch_1.png","","")
	:move(500,510)
	:setSelected(GlobalUserItem.bVoiceAble)
	:addTo(self)
	:setTag(self.CBT_SILENCE)
	self._cbtSound:addEventListener(cbtlistener)--]]
	
	--背景音乐开关
	self._cbtSilence = self.m_optionBg:getChildByName("CheckBox_Sound")
	:setSelected(GlobalUserItem.bSoundAble)
	:setTag(self.CBT_SOUND)
	self._cbtSilence:addEventListener(cbtlistener)
	
	--音效开关
	self._cbtSound = self.m_optionBg:getChildByName("CheckBox_Silence")
	:setSelected(GlobalUserItem.bVoiceAble)
	:setTag(self.CBT_SILENCE)
	self._cbtSound:addEventListener(cbtlistener)
	
	--常见问题
	self._option_check = self.m_optionBg:getChildByName("btn_option_check")
	:setTag(OptionLayer.BT_QUESTION)
	:addTouchEventListener(btcallback)
	
	--游戏反馈
	self._option_commit = self.m_optionBg:getChildByName("btn_option_commit")
	:setTag(OptionLayer.BT_COMMIT)
	:addTouchEventListener(btcallback)
	
	--当前账号
	self._nickname = self.m_optionBg:getChildByName("Text_nickname")
	self._nicknametext = self._nickname:getChildByName("Text_nickname")
	self._nicknametext:setString(GlobalUserItem.szNickName.."")
	
	--[[--常见问题
	display.newSprite("Option/frame_option_0.png")
	:move(330,338)
	:addTo(self)
	display.newSprite("Option/text_question.png")
	:move(170,338)
	:addTo(self)
	ccui.Button:create("Option/bt_option_check_0.png","Option/bt_option_check_1.png")
	:move(500,338)
	:setTag(OptionLayer.BT_QUESTION)
	:addTo(self)
	:addTouchEventListener(btcallback)
	
	--游戏反馈
	display.newSprite("Option/frame_option_0.png")
	:move(1000,338)
	:addTo(self)
	display.newSprite("Option/text_feedback.png")
	:move(837,338)
	:addTo(self)
	ccui.Button:create("Option/bt_option_commit_0.png","Option/bt_option_commit_1.png")
	:move(1173,338)
	:setTag(OptionLayer.BT_COMMIT)
	:addTo(self)
	:addTouchEventListener(btcallback)
	
	--帐号信息
	display.newSprite("Option/frame_option_1.png")
	:move(yl.WIDTH/2,166)
	:addTo(self)
	display.newSprite("Option/text_account.png")
	:move(240,166)
	:addTo(self)
	
	local testen = cc.Label:createWithSystemFont("A","Arial", 24)
	self._enSize = testen:getContentSize().width
	local testcn = cc.Label:createWithSystemFont("游","Arial", 24)
	self._cnSize = testcn:getContentSize().width
	self._nickname = cc.Label:createWithTTF(string.stringEllipsis(GlobalUserItem.szNickName,self._enSize,self._cnSize,200), "base/fonts/round_body.ttf", 24)
	:move(435,166)
	:setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
	:setAnchorPoint(cc.p(0.5,0.5))
	:setWidth(210)
	:setHeight(25)
	:setLineBreakWithoutSpace(false)
	:setTextColor(cc.c4b(240,240,240,255))
	:addTo(self)
	
	-- 锁定
	if 1 == GlobalUserItem.cbLockMachine then
		self.m_btnLock = ccui.Button:create("Option/btn_unlockmachine_0.png","Option/btn_unlockmachine_1.png","Option/btn_unlockmachine_0.png")
		self.m_btnLock:setTag(OptionLayer.BT_UNLOCK)
	else
		self.m_btnLock = ccui.Button:create("Option/btn_lockmachine_0.png","Option/btn_lockmachine_1.png","Option/btn_lockmachine_0.png")
		self.m_btnLock:setTag(OptionLayer.BT_LOCK)
	end
	self.m_btnLock:move(631,166)
	:addTo(self)
	:addTouchEventListener(btcallback)
	
	--修改密码
	ccui.Button:create("Option/bt_option_modify_0.png","Option/bt_option_modify_1.png")
	:move(861,166)
	:setTag(OptionLayer.BT_MODIFY)
	:addTo(self)
	:addTouchEventListener(btcallback)
	--切换帐号
	ccui.Button:create("Option/bt_option_change_0.png","Option/bt_option_change_1.png")
	:move(1091,166)
	:setTag(OptionLayer.BT_EXCHANGE)
	:addTo(self)
	:addTouchEventListener(btcallback)
	
	local mgr = self._scene:getApp():getVersionMgr()
	-- 版本号
	cc.Label:createWithTTF("版本号:" .. BaseConfig.BASE_C_VERSION .. "." .. mgr:getResVersion(), "base/fonts/round_body.ttf", 24)
	:move(yl.WIDTH,0)
	:setAnchorPoint(cc.p(1,0))
	:addTo(self)--]]
	
	--锁定本机
	self.m_unbtnLock = self.m_optionBg:getChildByName("btn_unlockmachine")
	self.m_btnLock = self.m_optionBg:getChildByName("btn_lockmachine")
	if 1 == GlobalUserItem.cbLockMachine then
		self.m_unbtnLock:setTag(OptionLayer.BT_UNLOCK)
		--[[self.m_btnLock:setVisible(false)
		self.m_unbtnLock:setVisible(true)--]]
	else
		self.m_btnLock:setTag(OptionLayer.BT_LOCK)
		--[[self.m_unbtnLock:setVisible(false)
		self.m_btnLock:setVisible(true)--]]
	end
	self.m_unbtnLock:addTouchEventListener(btcallback)
	self.m_btnLock:addTouchEventListener(btcallback)
	
	--修改密码
	self._option_modify = self.m_optionBg:getChildByName("btn_option_modify")
	:setTag(OptionLayer.BT_MODIFY)
	:addTouchEventListener(btcallback)
	
	--切换帐号
	self._option_change = self.m_optionBg:getChildByName("btn_option_change")
	:setTag(OptionLayer.BT_EXCHANGE)
	:addTouchEventListener(btcallback)
	
	--退出游戏
	self._option_exit = self.m_optionBg:getChildByName("btn_option_exit")
	:setTag(OptionLayer.BT_EXITPLAZA)
	:addTouchEventListener(btcallback)
	
	-- 版本号
	local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
	
	self._version = self.m_optionBg:getChildByName("text_version")
	self._versiontext = self._version:getChildByName("text_version")
	self._versiontext:setString(BaseConfig.BASE_C_VERSION .. "." .. versionProxy:getResVersion())
	
	
		-- 添加点击监听事件 遮罩
    local mask = csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
		local pos = touch:getLocation()
		local pBg = csbNode:getChildByName("Option_Bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SETTING_LAYER})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function OptionLayer:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function OptionLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	self._csbNodeAni:play("closeAni", false)
end


function OptionLayer:onSelectedEvent(tag,sender,eventType)
	if tag == OptionLayer.CBT_SILENCE then
		GlobalUserItem.setVoiceAble(eventType == 0)
		--背景音乐
		ExternalFun.playPlazzBackgroudAudio()
	elseif tag == OptionLayer.CBT_SOUND then
		GlobalUserItem.setSoundAble(eventType == 0)
	end
end

--按键监听
function OptionLayer:onButtonClickedEvent(tag,sender)
	ExternalFun.playClickEffect()
	if tag ~= OptionLayer.BT_EXCHANGE and tag ~= OptionLayer.BT_EXIT then
		if GlobalUserItem.isAngentAccount() then
			return
		end
	end
	
	if tag == OptionLayer.BT_EXCHANGE then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.CLIENT_SCENE})	
	elseif tag == OptionLayer.BT_EXITPLAZA then
		os.exit(0)
	elseif tag == OptionLayer.BT_EXIT then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SETTING_LAYER})	
	elseif tag == OptionLayer.BT_QUESTION then
		self._scene:onChangeShowMode(yl.SCENE_FAQ)
	elseif tag == OptionLayer.BT_MODIFY then
		if self._scene._gameFrame:isSocketServer() then
			showToast(self,"当前页面无法使用此功能！",1)
			return
		end
		self._scene:onChangeShowMode(yl.SCENE_MODIFY)
	elseif tag == OptionLayer.BT_COMMIT then
		self._scene:onChangeShowMode(yl.SCENE_FEEDBACK)
	elseif tag == OptionLayer.BT_LOCK then
		print("锁定机器")
		self:showLockMachineLayer(self)
	elseif tag == OptionLayer.BT_UNLOCK then
		print("解锁机器")
		self:showLockMachineLayer(self)
	end
end

local TAG_MASK = 101
local BTN_CLOSE = 102
function OptionLayer:showLockMachineLayer( parent )
	if nil == parent then
		return
	end
	--网络回调
	local modifyCallBack = function (result,message)
		self:onModifyCallBack(result,message)
	end
	--网络处理
	self._modifyFrame = ModifyFrame:create(self,modifyCallBack)
	
	-- 加载csb资源
	local csbNode = ExternalFun.loadCSB("Option/LockMachineLayer.csb", parent )
	
	local touchFunC = function (ref, tType)
		if tType == ccui.TouchEventType.ended then
			local tag = ref:getTag()
			if TAG_MASK == tag or BTN_CLOSE == tag then
				csbNode:removeFromParent()
			elseif OptionLayer.BT_LOCK == tag then
				local txt = csbNode.m_editbox:getText()
				if txt == "" then
					showToast(self, "密码不能为空!", 2)
					return
				end
				self._modifyFrame:onBindingMachine(1, txt)
				csbNode:removeFromParent()
			elseif OptionLayer.BT_UNLOCK == tag then
				local txt = csbNode.m_editbox:getText()
				if txt == "" then
					showToast(self, "密码不能为空!", 2)
					return
				end
				self._modifyFrame:onBindingMachine(0, txt)
				csbNode:removeFromParent()
			end
		end
	end
	
	-- 遮罩
	local mask = csbNode:getChildByName("panel_mask")
	mask:setTag(TAG_MASK)
	mask:addTouchEventListener( touchFunC )
	
	local image_bg = csbNode:getChildByName("image_bg")
	image_bg:setSwallowTouches(true)
	
	-- 输入
	local tmp = image_bg:getChildByName("sp_lockmachine_bankpw")
	local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10),"blank.png",UI_TEX_TYPE_PLIST)
	:setPosition(tmp:getPosition())
	:setFontName("base/fonts/round_body.ttf")
	:setPlaceholderFontName("base/fonts/round_body.ttf")
	:setFontSize(30)
	:setPlaceholderFontSize(30)
	:setMaxLength(32)
	:setInputFlag(cc.EDITBOX_INPUT_FLAG_PASSWORD)
	:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
	:setPlaceHolder("请输入密码")
	image_bg:addChild(editbox)
	csbNode.m_editbox = editbox
	
	-- 锁定/解锁
	local btn = image_bg:getChildByName("btn_lock")
	btn:setTag(OptionLayer.BT_LOCK)
	btn:addTouchEventListener( touchFunC )
	local normal = "Option/btn_lockmachine_0.png"
	local disable = "Option/btn_lockmachine_0.png"
	local press = "Option/btn_lockmachine_1.png"
	if 1 == GlobalUserItem.cbLockMachine then
		btn:setTag(OptionLayer.BT_UNLOCK)
		normal = "Option/btn_unlockmachine_0.png"
		disable = "Option/btn_unlockmachine_0.png"
		press = "Option/btn_unlockmachine_1.png"
	end
	btn:loadTextureDisabled(disable)
	btn:loadTextureNormal(normal)
	btn:loadTexturePressed(press)
	
	btn = image_bg:getChildByName("btn_cancel")
	btn:setTag(BTN_CLOSE)
	btn:addTouchEventListener( touchFunC )
	
	-- 关闭
	btn = image_bg:getChildByName("btn_close")
	btn:setTag(BTN_CLOSE)
	btn:addTouchEventListener( touchFunC )
end

function OptionLayer:onModifyCallBack(result, tips)
	if type(tips) == "string" and "" ~= tips then
		showToast(self, tips, 2)
	end
	
	--[[local normal = "Option/btn_lockmachine_0.png"
	local disable = "Option/btn_lockmachine_0.png"
	local press = "Option/btn_lockmachine_1.png"--]]
	if self._modifyFrame.BIND_MACHINE == result then
		if 0 == GlobalUserItem.cbLockMachine then
			GlobalUserItem.cbLockMachine = 1
			self.m_unbtnLock:setTag(OptionLayer.BT_UNLOCK)
			--[[normal = "Option/btn_unlockmachine_0.png"
			disable = "Option/btn_unlockmachine_0.png"
			press = "Option/btn_unlockmachine_1.png"--]]
			self.m_unbtnLock:setVisible(true)
			self.m_btnLock:setVisible(false)
		else
			GlobalUserItem.cbLockMachine = 0
			self.m_btnLock:setTag(OptionLayer.BT_LOCK)
			self.m_unbtnLock:setVisible(false)
			self.m_btnLock:setVisible(true)
		end
	end
	--[[self.m_btnLock:loadTextureDisabled(disable)
	self.m_btnLock:loadTextureNormal(normal)
	self.m_btnLock:loadTexturePressed(press)--]]
end

return OptionLayer