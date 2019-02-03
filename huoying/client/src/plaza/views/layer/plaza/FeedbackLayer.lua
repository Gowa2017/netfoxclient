--
-- Author: zhong
-- Date: 2016-08-10 17:49:13
--
local ExternalFun = require(appdf.EXTERNAL_SRC .. "ExternalFun")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

--返回按钮
local BT_EXIT 		= 101
--选择图片
local BT_PICIMG 	= 102
--发送按钮
local BT_SEND 		= 103
--我的反馈
local BT_MYFEEDBACk = 104

--我的反馈列表
local FeedbackListLayer = class("FeedbackListLayer", cc.Layer)
function FeedbackListLayer:ctor(scene)
	self._scene = scene
	
	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("feedback/FeedbackListLayer.csb", self)
	self.m_csbNode = csbNode

	local function btncallback(ref, type)
        if type == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    --返回按钮
    local btn = csbNode:getChildByName("btn_back")
    btn:setTag(BT_EXIT)
    btn:addTouchEventListener(btncallback)
end

function FeedbackListLayer:onButtonClickedEvent( tag, sender )
	if BT_EXIT == tag then
		self._scene:onKeyBack()		
	end
end

--反馈编辑界面
local FeedbackLayer = class("FeedbackLayer", cc.Layer)
function FeedbackLayer.createFeedbackList( scene )
	local list = FeedbackListLayer.new(scene)
	return list
end

function FeedbackLayer:ctor( scene )
	self._scene = scene

	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("feedback/FeedbackSendLayer.csb", self)
	self.m_csbNode = csbNode

	local function btncallback(ref, type)
        if type == ccui.TouchEventType.ended then
         	self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    --返回按钮
    local btn = csbNode:getChildByName("btn_back")
    btn:setTag(BT_EXIT)
    btn:addTouchEventListener(btncallback)
    --[[
    --图片选择
    btn = csbNode:getChildByName("btn_pickimg")
    btn:setTag(BT_PICIMG)
    btn:addTouchEventListener(btncallback)

    --我的反馈
    btn = csbNode:getChildByName("btn_myfeed")
    btn:setTag(BT_MYFEEDBACk)
    btn:addTouchEventListener(btncallback)

	--反馈编辑框
	local tmp = csbNode:getChildByName("sp_edit_bg")
	local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10),"blank.png",UI_TEX_TYPE_PLIST)
		:setPosition(tmp:getPosition())
		:setFontName("fonts/round_body.ttf")
		:setPlaceholderFontName("fonts/round_body.ttf")
		:setFontSize(30)
		:setPlaceholderFontSize(30)
		:setPlaceHolder("欢迎您对我们的游戏提出宝贵意见,您的意见会让我们做的更好!")
	csbNode:addChild(editbox)
	self.m_editFeedback = editbox]]

	local tmp = csbNode:getChildByName("sp_public_frame")
	--平台判定
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
		--反馈页面
		self.m_webView = ccexp.WebView:create()
	    self.m_webView:setPosition(cc.p(667, 322))
	    self.m_webView:setContentSize(cc.size(1260, 580))
	    
	    self.m_webView:setScalesPageToFit(true)
	    local url = yl.HTTP_URL .. "/SyncLogin.aspx?userid=" .. GlobalUserItem.dwUserID .. "&time=".. os.time() .. "&signature="..GlobalUserItem:getSignature(os.time()).."&url=/Mobile/Feedback.aspx"
	    self.m_webView:loadURL(url)
        ExternalFun.visibleWebView(self.m_webView, false)
	    self._scene:showPopWait()

	    self.m_webView:setOnJSCallback(function ( sender, url )
	    	    	
	    end)

	    self.m_webView:setOnDidFailLoading(function ( sender, url )
	    	self._scene:dismissPopWait()
	    	print("open " .. url .. " fail")
	    end)
	    self.m_webView:setOnShouldStartLoading(function(sender, url)
	        print("onWebViewShouldStartLoading, url is ", url)	        
	        return true
	    end)
	    self.m_webView:setOnDidFinishLoading(function(sender, url)
	    	self._scene:dismissPopWait()
            ExternalFun.visibleWebView(self.m_webView, true)
	        print("onWebViewDidFinishLoading, url is ", url)
	    end)
	    self:addChild(self.m_webView)
	end
    --tmp:removeFromParent()
end

function FeedbackLayer:onButtonClickedEvent( tag, sender )
	if BT_EXIT == tag then
		--[[if nil ~= self.m_webView then
			if true == self.m_webView:canGoBack() then
				self.m_webView:goBack()
				return
			end
		end]]
		self._scene:onKeyBack()
	elseif BT_PICIMG == tag then
		MultiPlatform:getInstance():triggerPickImg(function ( param )
			if type(param) == "string" then
				print("lua path ==> " .. param)
			end
		end, false)
	elseif BT_SEND == tag then
		
	elseif BT_MYFEEDBACk == tag then
		self._scene:onChangeShowMode(yl.SCENE_FEEDBACKLIST)
	end
end

return FeedbackLayer