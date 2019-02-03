--[[
	活动界面
]]

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local ActivityLayer = class("ActivityLayer", cc.BaseLayer)

-- 进入场景而且过渡动画结束时候触发。
function ActivityLayer:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function ActivityLayer:onExitTransitionStart()
    return self
end

function ActivityLayer:ctor()
	self.super.ctor(self)

	--加载csb资源
	local csbPath = "activity/ActivityLayer.csb"
	local rootLayer, csbNode = ExternalFun.loadRootCSB(csbPath, self)
	self._csbNode = csbNode
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)	
end

--初始化主界面
function ActivityLayer:initUI()
	local activityBg = self._csbNode:getChildByName("activity_bg_1")
	--local btnClose = activityBg:getChildByName("btnClose")
	--btnClose:addClickEventListener(handler(self, self.onButtonClickedEvent))	
	
	local top_bg = activityBg:getChildByName("info_top_bg")
	--返回按钮
	self._return = top_bg:getChildByName("Button_return")
    self._return:addClickEventListener(handler(self, self.onButtonClickedEvent))

	
	local webContent = activityBg:getChildByName("Content")
    local webContentSize = webContent:getContentSize()
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
        --介绍页面
--        self._webView = ccexp.WebView:create()
--		self._webView:setContentSize(webContentSize)
--        self._webView:setPosition(cc.p(webContentSize.width / 2, webContentSize.height / 2))
--        self._webView:setScalesPageToFit(true) 
--        local times = os.time()
        local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/activity.aspx"
--        self._webView:loadURL("http://www.baidu.com")
--        self._webView:setJavascriptInterfaceScheme("http")
--        self._webView:setVisible(self._webView, webContent:isVisible())
--        self._scene:showPopWait()

--        self._webView:setOnDidFailLoading(function ( sender, url )
--            self._scene:dismissPopWait()
--            print("open " .. url .. " fail")
--        end)
--        self._webView:setOnShouldStartLoading(function(sender, url)
--            print("onWebViewShouldStartLoading, url is ", url)          
--            return true
--        end)
--        self._webView:setOnDidFinishLoading(function(sender, url)
--            self._scene:dismissPopWait()
--            ExternalFun.visibleWebView(self._webView, true)
--        end)
--        self._webView:setOnJSCallback(function ( sender, url )
--            if url == "http://param:close" then
--                self._scene:dismissPopWait()
--                self:removeFromParent()
--            else
--                self._scene:queryUserScoreInfo(function(ok)
--                    if ok then
--                        self._webView:reload()
--                    end
--                end)
--            end            
--        end)



        --介绍页面
        self._webView = ccexp.WebView:create()
        self._webView:setPosition(cc.p(webContentSize.width / 2, webContentSize.height / 2))
        self._webView:setContentSize(webContentSize)
        
        self._webView:setScalesPageToFit(true)        
        self._webView:loadURL(url)
		
        self:showPopWait()

        self._webView:setOnDidFailLoading(function ( sender, url )
			self:dismissPopWait()
            print("open " .. url .. " fail")
        end)
        self._webView:setOnShouldStartLoading(function(sender, url)
            print("onWebViewShouldStartLoading, url is ", url)          
            return true
        end)
        self._webView:setOnDidFinishLoading(function(sender, url)
			self:dismissPopWait()
        end)

        webContent:addChild(self._webView)
	else
		webContent:setBackGroundColorType(ccui.LayoutBackGroundColorType.solid)
		webContent:setBackGroundColor(cc.c3b(255, 0, 255))
    end	
end

function ActivityLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function ActivityLayer:onWillViewExit()
	--由于WebView 在隐藏，放大 关闭的时候 会有残影bug，因此在播放移除动画的时候必须先释放webview
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
		self._webView:setVisible(false)
	end
	
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

--按键监听
function ActivityLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	if ("Button_return" == sender:getName()) then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.ACTIVITY_LAYER})
	end
end

function ActivityLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function ActivityLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end

return ActivityLayer