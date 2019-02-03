local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
--头像选择界面
local SelectSystemHeadLayer = class("SelectSystemHeadLayer", cc.BaseLayer)

function SelectSystemHeadLayer:ctor()
	self.super.ctor(self)

    -- 加载csb资源
	local csbPath = "Userinfo/SelectSystemHeadLayer.csb"
    local rootLayer, csbNode = appdf.loadRootCSB(csbPath,self)
	self._csbNode = csbNode
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	local setbg = csbNode:getChildByName("setbg")
	
	
	self._btnClose = setbg:getChildByName("btn_close")
	self._btnClose:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	self._btnOk = setbg:getChildByName("btn_ok")
	self._btnOk:addClickEventListener(handler(self, self.onButtonClickedEvent))
	
	
	local yaan = setbg:getChildByName("clip"):getChildByName("yaan")
	
	self.m_radioBox = cc.RadioBox:create()
	self.m_radioBox:setShowRadioCount(3)
	local yaancontentSize = yaan:getContentSize()
	--yaancontentSize.width = yaancontentSize.width + 300
	self.m_radioBox:setAnchorPoint(cc.p(0.5, 0.5))
	self.m_radioBox:setContentSize(cc.size(yaancontentSize.width + 300, yaancontentSize.height))
	self.m_radioBox:setPosition(cc.p(yaancontentSize.width / 2, yaancontentSize.height / 2))
	self.m_radioBox:addFocusEventListener(handler(self, self.onFocusEvent))
	self.m_radioBox:setSelectedScale(1.1, 1.1)
	yaan:addChild(self.m_radioBox)
	
	self._faceindex = 0
	
	--性别选择

	
	for i = 1, 4 do
		local unSelectImg = ""
		local selectImg = ""
		if (GlobalUserItem.cbGender == yl.GENDER_MANKIND) then
			unSelectImg = string.format("man_%d_1.png", i - 1)
			selectImg = string.format("man_%d_0.png", i - 1)
		else
			unSelectImg = string.format("women_%d_1.png", i - 1)
			selectImg = string.format("women_%d_0.png", i - 1)
		end
		
		pRadioButton = ccui.RadioButton:create(unSelectImg,unSelectImg,selectImg,unSelectImg,unSelectImg, ccui.TextureResType.plistType)
		self.m_radioBox:addRadioButton(pRadioButton, false)
	end
	
	--self.m_radioBox:refresh()

	
		-- 添加点击监听事件 遮罩
    local mask = csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
		local pos = touch:getLocation();
		local pBg = csbNode:getChildByName("setbg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SELECT_SYSTEM_HEAD_LAYER})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function SelectSystemHeadLayer:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
		
		--弹开后播放选中头像的动画
		if (GlobalUserItem.wFaceID > 0) then
			self.m_radioBox:setSelectedButton(GlobalUserItem.wFaceID - 1)
		end
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function SelectSystemHeadLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

function SelectSystemHeadLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	
	if (sender:getName() == "btn_close") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SELECT_SYSTEM_HEAD_LAYER})
	elseif (sender:getName() == "btn_ok") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.POPWAIT_LAYER)
		self:emit("V2M_ChangeSystemFaceOK", self._faceindex,function (code, msg)
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
			self:showToast(msg)
			if (code == 0) then
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.UPDATE_USER_FACE)
				AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SELECT_SYSTEM_HEAD_LAYER})
			end
		end)
	end
end

function SelectSystemHeadLayer:showToast(msg)
	local curScene = cc.Director:getInstance():getRunningScene()
	showToast(curScene, msg, 1)
end


function SelectSystemHeadLayer:onFocusEvent(sender, index, eventType)
	if (ccui.RadioButtonEventType.selected == eventType) then
		ExternalFun.playClickEffect()
		self._faceindex = index
	end
end

return SelectSystemHeadLayer