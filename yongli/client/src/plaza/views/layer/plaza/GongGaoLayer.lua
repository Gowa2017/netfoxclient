local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local GongGaoLayer = class("GongGaoLayer", cc.BaseLayer)
function GongGaoLayer:ctor()
	self.super.ctor(self)
	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("GongGao/GongGao.csb", self)
	self._csbNodeAni = ExternalFun.loadTimeLine( "GongGao/GongGao.csb" )
	csbNode:runAction(self._csbNodeAni)
	self._csbNode = csbNode
	self.m_webView = nil
end

function GongGaoLayer:initUI()
	--按钮事件
	local btn_return = self._csbNode:getChildByName("gonggao_bg"):getChildByName("btn_close")
	btn_return:addClickEventListener(handler(self,self.onButtonClickedEvent))
	
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	local gonggao_cont = self._csbNode:getChildByName("gonggao_bg"):getChildByName("gonggao_cont")
	local contentSize = gonggao_cont:getContentSize()
    if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
        --介绍页面
        self.m_webView = ccexp.WebView:create()
        self.m_webView:setPosition(cc.p(contentSize.width / 2, contentSize.height / 2))
        self.m_webView:setContentSize(contentSize)
        
        self.m_webView:setScalesPageToFit(true) 
        --local times = os.time()
        local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/AdsNotice.aspx"

        self.m_webView:loadURL(url)
        --self.m_webView:setJavascriptInterfaceScheme(URL_SCHEME)
		
        self:showPopWait()

        self.m_webView:setOnDidFailLoading(function ( sender, url )
            self:dismissPopWait()
            print("open " .. url .. " fail")
        end)
        self.m_webView:setOnShouldStartLoading(function(sender, url)
            print("onWebViewShouldStartLoading, url is ", url)          
            return true
        end)
        self.m_webView:setOnDidFinishLoading(function(sender, url)
            self:dismissPopWait()
            ExternalFun.visibleWebView(self.m_webView, true)
        end)
--[[        self.m_webView:setOnJSCallback(function ( sender, url )
            if url == "ryweb://param:close" then
                self:dismissPopWait()
                self:removeFromParent()
            else
				
                self._scene:queryUserScoreInfo(function(ok)
                    if ok then
                        self.m_webView:reload()
                    end
                end)
            end            
        end)--]]

        gonggao_cont:addChild(self.m_webView)
	end
	
	-- 添加点击监听事件 遮罩
    local mask = self._csbNode:getChildByName("mask")
	local Layoutlistener = cc.EventListenerTouchOneByOne:create()
	Layoutlistener:setSwallowTouches(true)
	Layoutlistener:registerScriptHandler(function(touch, event)
		return self:isVisible()
	end,cc.Handler.EVENT_TOUCH_BEGAN)
	--listener:registerScriptHandler(onTouchMoved,cc.Handler.EVENT_TOUCH_MOVED )
	Layoutlistener:registerScriptHandler(function(touch, event)
		local pos = touch:getLocation();
		local pBg = self._csbNode:getChildByName("gonggao_bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.GONGGAO_LAYER})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function GongGaoLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function GongGaoLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end


function GongGaoLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)

	self._csbNodeAni:play("openAni", false)
end

function GongGaoLayer:onWillViewExit()
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
		self.m_webView:setVisible(false)
	end
	
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

function GongGaoLayer:onButtonClickedEvent(sender)
	if (sender:getName() == "btn_close") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.GONGGAO_LAYER})
		--AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.GONGGAO_LAYER)
	end
end

return GongGaoLayer