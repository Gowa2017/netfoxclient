--[[
	银行界面
	2017_08_25 MXM
]]

local AgentShengQingLayer = class("AgentShengQingLayer", cc.BaseLayer)

local PopWait = appdf.req(appdf.CLIENT_SRC.."app.views.layer.other.PopWait")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

--开通银行
function AgentShengQingLayer:ctor(backinfo)
	self.super.ctor(self)
    -- 加载csb资源
	local csbPath = "Shop/AgentRecharge/AgentRechargeShengQing.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	self._backInfo = backinfo or {}
end

function AgentShengQingLayer:initUI()
	local imgBg = self._csbNode:getChildByName("bg")
	
     --返回按钮
    local btn = imgBg:getChildByName("btn_close")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
    --确定按钮
    btn = imgBg:getChildByName("btn_agent_sq")
    btn:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	local text_field_bg_1 = imgBg:getChildByName("text_field_bg_1")
	local text_field_1 = text_field_bg_1:getChildByName("text_field_1")	
	text_field_size = text_field_1:getContentSize()
	self.m_edit1 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入账号(不能包含中文)不能少于8位...")
		:setText(self._backInfo[1] or "")
		:addTo(text_field_1)
		
		
	local text_field_bg_2 = imgBg:getChildByName("text_field_bg_2")
	local text_field_2 = text_field_bg_2:getChildByName("text_field_2")	
	text_field_size = text_field_2:getContentSize()
	self.m_edit2 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入商户名称...")
		:setText(self._backInfo[2] or "")
		:addTo(text_field_2)
		
	local text_field_bg_3 = imgBg:getChildByName("text_field_bg_3")
	local text_field_3 = text_field_bg_3:getChildByName("text_field_3")	
	text_field_size = text_field_3:getContentSize()
	self.m_edit3 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入账号,方便玩家支付...")
		:setText(self._backInfo[3] or "")
		:addTo(text_field_3)
		
	local text_field_bg_4 = imgBg:getChildByName("text_field_bg_4")
	local text_field_4 = text_field_bg_4:getChildByName("text_field_4")	
	text_field_size = text_field_4:getContentSize()
	self.m_edit4 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入微信账号,方便玩家支付...")
		:setText(self._backInfo[4] or "")
		:addTo(text_field_4)

	local text_field_bg_5 = imgBg:getChildByName("text_field_bg_5")
	local text_field_5 = text_field_bg_5:getChildByName("text_field_5")	
	text_field_size = text_field_5:getContentSize()
	self.m_edit5 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(26)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请输入QQ账号,方便玩家支付...")
		:setText(self._backInfo[5] or "")
		:addTo(text_field_5)	


	local text_field_bg_6 = imgBg:getChildByName("text_field_bg_6")
	local text_field_6 = text_field_bg_6:getChildByName("text_field_6")	
	text_field_size = text_field_6:getContentSize()
	self.m_edit6 = ccui.EditBox:create(text_field_size, ccui.Scale9Sprite:create("public/text_field_space.png"))
		:move(text_field_size.width / 2,text_field_size.height / 2)
		:setAnchorPoint(cc.p(0.5,0.5))
		:setFontName("base/fonts/round_body.ttf")
		:setPlaceholderFontName("base/fonts/round_body.ttf")
		:setFontSize(24)
		:setPlaceholderFontSize(24)
		:setMaxLength(150)
		:setInputMode(cc.EDITBOX_INPUT_MODE_SINGLELINE)
		:setPlaceHolder("请简单描述您的情况...")
		:setText(self._backInfo[6] or "")
		:addTo(text_field_6)			
		
	self:attach("M2V_ShowToast", function (e, msg)
		showToast(self, msg, 2)
	end)	
		
	-- 添加点击监听事件 遮罩
    local mask = self._csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
--[[		local pos = touch:getLocation();
		local pBg = self._csbNode:getChildByName("bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_SHENGQING_DAILI})
		end--]]
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function AgentShengQingLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)

	self._csbNodeAni:play("openAni", false)
end

function AgentShengQingLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)

	self._csbNodeAni:play("closeAni", false)
end

--按键监听
function AgentShengQingLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()

	local senderName = sender:getName()
	
	if senderName == "btn_close" then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_SHENGQING_DAILI})
	elseif senderName == "btn_agent_sq" then
		--传递参数
		local text1 = self.m_edit1:getText()	--代理微信号
		local text2 = self.m_edit2:getText()	--代理名称
		local text3 = self.m_edit3:getText()	--支付宝
		local text4 = self.m_edit4:getText()	--微信号
		local text5 = self.m_edit5:getText()	--QQ号
		local text6 = self.m_edit6:getText()	--说明
		
		local parminfo = {text1, text2, text3, text4, text5, text6}
		
		if (self:checkShengQingParm(parminfo)) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_SHENGQING_DAILI})
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {parminfo}}, VIEW_LIST.SHOP_SHENGQING_DAILI_COMFIRM)
		end
	end
end

--检查参数
function AgentShengQingLayer:checkShengQingParm(parminfo)
	assert(type(parminfo) == "table")
	--代理微信号
	local text1 = parminfo[1] or ""
	len = ExternalFun.stringLen(text1)
	if len < 8 or len > 20 then
		showToast(self,"微信号长度不符，请重新输入！",2,cc.c4b(250,255,255,255));
		return false
	end	
	
	-- 含有中文
	local lenInByte = string.len(text1)
	local chinese_count = 0
	local index = 0
	while index < lenInByte do
		index = index + 1
		
		local curByte = string.byte(text1, index)
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
		 
		local char = string.sub(text1, index, index+byteCount-1)
		index = index + byteCount -1
	end
	
	if (chinese_count ~= 0) then
		showToast(self,"账号不能包含中文，请重新输入！",2,cc.c4b(250,255,255,255));
		return false
	end	
	
	--代理名称
	local text2 = parminfo[2] or ""
	len = ExternalFun.stringLen(text2)
	if len <= 0 then
		showToast(self,"代理名称不能为空，请重新输入！",2,cc.c4b(250,255,255,255));
        return false
    end
	--支付宝
	local text3 = parminfo[3] or ""
	len = ExternalFun.stringLen(text3)
	if len <= 0 then
		showToast(self,"支付宝账号不能为空，请重新输入！",2,cc.c4b(250,255,255,255));
        return false
    end	
	--微信号
	local text4 = parminfo[4] or ""
	len = ExternalFun.stringLen(text4)
	if len < 8 or len > 20 then
		showToast(self,"微信号长度不符，请重新输入！",2,cc.c4b(250,255,255,255));
        return false
    end		
	--QQ号
	local text5 = parminfo[5] or ""
	len = ExternalFun.stringLen(text5)
	if len <= 0 then
		showToast(self,"QQ号不能为空，请重新输入！",2,cc.c4b(250,255,255,255));
        return false
    end
	
	return true
end

--显示等待
function AgentShengQingLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function AgentShengQingLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

return AgentShengQingLayer