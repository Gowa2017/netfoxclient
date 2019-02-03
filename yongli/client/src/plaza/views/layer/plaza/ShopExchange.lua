--[[
	活动界面
]]

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local ShopExchange = class("ShopExchange", ccui.Layout)

-- 进入场景而且过渡动画结束时候触发。
function ShopExchange:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function ShopExchange:onExitTransitionStart()
	--解决切换场景webview残留问题
	if (nil ~= self._webView) then
		self._webView:removeFromParent()
	end
    return self
end

function ShopExchange:onExit()
    return self
end

function ShopExchange:ctor(scene)
	self._scene = scene
	--[[if ("table" ~= type(contentSize)) then
		contentSize = cc.size(200, 200)
	end
	
	self:setContentSize(contentSize)
	self:setAnchorPoint(cc.p(0.5, 0.5))--]]
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
		elseif eventType == "exit" then
			self:onExit()
		end
	end)
	
	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("Shop/ShopExchangeLayer.csb", self)
	self._rootlayer = rootLayer
	
	local webContent = csbNode:getChildByName("Content")
    local webContentSize = webContent:getContentSize()
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	
    if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
        --介绍页面
        self._webView = ccexp.WebView:create()
        self._webView:setPosition(cc.p(webContentSize.width / 2, webContentSize.height / 2))
        self._webView:setContentSize(webContentSize)
        
        self._webView:setScalesPageToFit(true)

		local ostime = os.time()
		local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/Exchange.aspx?userid=" .. GlobalUserItem.dwUserID .. "&signature=" .. GlobalUserItem:getSignature(ostime) .. "&time=" .. ostime
        self._webView:loadURL(url)
        ExternalFun.visibleWebView(self._webView, webContent:isVisible())
       
		--self._webView:setJavascriptInterfaceScheme("http")
       self._webView:setOnDidFailLoading(function ( sender, url )
            self._scene:dismissPopWait()
            print("open " .. url .. " fail")
        end)
		self._webView:setOnShouldStartLoading(function(sender, url)
			self._scene:showPopWait()
			self._webView:runAction(cc.Sequence:create(cc.DelayTime:create(1.5), cc.CallFunc:create(function()
				self._scene:dismissPopWait()
			end)))
			
            print("onWebViewShouldStartLoading, url is ", url)          
            return true
        end)
		--self._scene:showPopWait()
		self._webView:setOnDidFinishLoading(function(sender, url)
            self._scene:dismissPopWait()
            ExternalFun.visibleWebView(self._webView, true)
        end)
        --self:addChild(self._webView)
		webContent:addChild(self._webView)
	else
		webContent:setBackGroundColorType(ccui.LayoutBackGroundColorType.solid)
		webContent:setBackGroundColor(cc.c3b(255, 0, 255))
		--[[local layout = ccui.Layout:create()
		layout:setPosition(cc.p(contentSize.width / 2, contentSize.height / 2))
        layout:setContentSize(contentSize)
		layout:setAnchorPoint(cc.p(0.5, 0.5))
		layout:setBackGroundColorType(ccui.LayoutBackGroundColorType.solid)
		layout:setBackGroundColor(cc.c3b(255, 0, 255))
		self:addChild(layout)--]]
    end
	
	--添加关闭消息监听
	--local listener = cc.EventListenerCustom:create(yl.RY_LOGON_CLOSE,handler(self, self.onLogonClose))
    --cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(listener, self)
	--    self.m_listener = cc.EventListenerCustom:create(yl.RY_USERINFO_NOTIFY,handler(self, self.onUserInfoChange))
    --cc.Director:getInstance():getEventDispatcher():addEventListenerWithSceneGraphPriority(self.m_listener, self)
	--		--通知查询     
    --        local eventListener = cc.EventCustom:new(yl.RY_JFTPAY_NOTIFY)
    --        cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
end

return ShopExchange