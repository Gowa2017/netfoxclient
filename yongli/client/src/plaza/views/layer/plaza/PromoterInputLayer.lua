local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")
-- 代理填写界面
local TAG_MASK = 101
local BT_SURE = 102
local BT_S_WECHAT = 103 	--分享到微信
local BT_S_CYCLE = 104 		--分享到朋友圈
local BT_S_CONTACT = 105 	--分享到短信
local BT_COPY = 106 		--复制粘贴板
local BT_QR_ENTER = 107 	--点击显示二维码

local QrCodeLayer = import(".QrCodeLayer")

local PromoterInputLayer = class("PromoterInputLayer", cc.BaseLayer)
function PromoterInputLayer:ctor( viewparent )
	self.super.ctor(self)
	self.m_parent = viewparent
	local noSpreader = GlobalUserItem.szSpreaderAccount == ""

	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("Userinfo/PromoterInputLayer.csb", self)
	self.m_spBg = csbNode:getChildByName("qr_sp_bg")
	self.m_spBg:setTouchEnabled(true)
    self.m_spBg:setSwallowTouches(true)
	
	
	self._csbNodeAni = ExternalFun.loadTimeLine( "Userinfo/PromoterInputLayer.csb" )
	self._csbNodeAni:setTimeSpeed(2)
	csbNode:runAction(self._csbNodeAni)
	

	local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end
	
	--注册事件
	ExternalFun.registerNodeEvent(self)

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    local editHanlder = function(event,editbox)
        self:onEditEvent(event,editbox)
    end
    -- 编辑框
    local tmp = self.m_spBg:getChildByName("sp_uinfoedit_bg")
    local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, 60),"blank.png",UI_TEX_TYPE_PLIST)
        :setPosition(tmp:getPosition())
        :setFontName("base/fonts/round_body.ttf")
        :setPlaceholderFontName("base/fonts/round_body.ttf")
        :setPlaceHolder("请输入推广员ID")
        :setMaxLength(32)
		:setVisible(false)
        :setPlaceholderFontColor(cc.c3b(51, 45, 106))
        :setFontColor(cc.c3b(51, 45, 106))
        :setInputMode(cc.EDITBOX_INPUT_MODE_NUMERIC)
    self.m_spBg:addChild(editbox)
    editbox:setContentSize(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10))
    editbox:setPlaceholderFont("base/fonts/round_body.ttf", 30)
    editbox:setFont("base/fonts/round_body.ttf", 30)
    editbox:registerScriptEditBoxHandler(editHanlder)
    self.m_bEditChat = false
    self.m_editID = editbox
    editbox:setEnabled(noSpreader)

--    -- 邀请奖励
--    local atlas = self.m_spBg:getChildByName("atlas_sendcount")
--    local count = GlobalUserItem.nInviteSend or 0
--    atlas:setString(count .. "")

    -- 确定按钮
    local btn = self.m_spBg:getChildByName("btn_sure")
    btn:setTag(BT_SURE)
    btn:addTouchEventListener( touchFunC )
    btn:setEnabled(noSpreader)
    if not noSpreader then
    	editbox:setPlaceHolder(GlobalUserItem.szSpreaderAccount)
    	btn:setOpacity(125)
    end

    -- 关闭按钮
    btn = self.m_spBg:getChildByName("btn_close")
    btn:setTag(TAG_MASK)
    btn:addTouchEventListener( touchFunC )

    -- 微信分享按钮
    btn = self.m_spBg:getChildByName("btn_wechat")
    btn:setTag(BT_S_WECHAT)
    btn:addTouchEventListener( touchFunC )

    -- 朋友圈
    btn = self.m_spBg:getChildByName("btn_cycle")
    btn:setTag(BT_S_CYCLE)
    btn:addTouchEventListener( touchFunC )

    -- 通讯录
    btn = self.m_spBg:getChildByName("btn_contact")
    btn:setTag(BT_S_CONTACT)
    btn:addTouchEventListener( touchFunC )

    -- 复制
    btn = self.m_spBg:getChildByName("btn_copy")
    btn:setTag(BT_COPY)
    btn:addTouchEventListener( touchFunC )

    -- 二维码
    btn = self.m_spBg:getChildByName("btn_qrcode")
    btn:setTag(BT_QR_ENTER)
    btn:addTouchEventListener( touchFunC )

    self.m_qrContent = GlobalUserItem.getShareUrl()
	local qr = QrNode:createQrNode(self.m_qrContent, 184, 5, 1)
    self.m_spBg:addChild(qr)
    qr:setPosition(btn:getPosition())
	qr:setVisible(btn:isVisible())
	self.m_qrNode = qr
end

function PromoterInputLayer:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("inAction", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("inAction", false)
end

function PromoterInputLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("outAction", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("outAction", false)
end

function PromoterInputLayer:onExitTransitionStart()
	--self._csbNodeAni:play("outAction", false)
end

function PromoterInputLayer:onButtonClickedEvent( tag, sender )
	ExternalFun.playClickEffect()
	if BT_SURE == tag then
		local txt = string.gsub(self.m_editID:getText(), "[.]", "")
		if txt == "" then
			showToast(self, "推广员ID不能为空!", 1)
			return
		end
		txt = tonumber(txt)
		if nil == txt then
			showToast(self, "请输入合法的推广员ID!", 1)
			return
		end
		self.m_parent:showPopWait()
		self.m_parent._modifyFrame:onBindSpreader(txt)
		self:runAction(cc.Sequence:create(cc.DelayTime:create(0.5), cc.CallFunc:create(function()
			self:removeFromParent()
		end)))
	elseif BT_S_WECHAT == tag then
        local function sharecall( isok )
            if type(isok) == "string" and isok == "true" then
                showToast(self, "分享完成", 1)
            end
        end
        local url = GlobalUserItem.getShareUrl()
        MultiPlatform:getInstance():shareToTarget(yl.ThirdParty.WECHAT, sharecall, GlobalUserItem.szWXShareTitle, GlobalUserItem.szWXShareContent, url)
	elseif BT_S_CYCLE == tag then
		local function sharecall( isok )
            if type(isok) == "string" and isok == "true" then
                showToast(self, "分享完成", 1)
            end
        end
        local url = GlobalUserItem.getShareUrl()
        MultiPlatform:getInstance():shareToTarget(yl.ThirdParty.WECHAT_CIRCLE, sharecall, GlobalUserItem.szWXShareTitle, GlobalUserItem.szWXShareContent, url)
	elseif BT_S_CONTACT == tag then
		local function sharecall( isok )
            if type(isok) == "string" and isok == "true" then
                showToast(self, "分享完成", 1)
            end
        end
        local url = GlobalUserItem.getShareUrl()
        MultiPlatform:getInstance():shareToTarget(yl.ThirdParty.SMS, sharecall, yl.SocialShare.title, url)
	elseif BT_COPY == tag then
		local url = GlobalUserItem.getShareUrl()
		local res, msg = MultiPlatform:getInstance():copyToClipboard(url)
		if true == res then
			showToast(self, "复制到剪贴板成功!", 1)
		else
			if type(msg) == "string" then
				showToast(self, msg, 1, cc.c3b(250,0,0))
			end
		end
	elseif BT_QR_ENTER == tag then
		local qrlayer = QrCodeLayer:create()
		self.m_parent:addChild(qrlayer)
		self:removeFromParent()
	elseif TAG_MASK == tag then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHARE_LAYER})
		if true == self.m_bEditChat then
	        self.m_bEditChat = false
	    end
	end
end

function PromoterInputLayer:closeView()
	if false == self.m_bEditChat then
		self._csbNodeAni:setLastFrameCallFunc(function ()
			self:removeFromParent()
		end)
		self._csbNodeAni:play("outAction", false)
		
	end		
	if true == self.m_bEditChat then
		self.m_bEditChat = false
	end
end

function PromoterInputLayer:onEditEvent(event,editbox)
    local src = editbox:getText()
    if event == "began" then
        self.m_bEditChat = string.len(src) ~= 0
    elseif event == "changed" then
        self.m_bEditChat = string.len(src) ~= 0
    end
end

return PromoterInputLayer