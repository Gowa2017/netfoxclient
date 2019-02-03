local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local VIPLayer = class("VIPLayer", cc.BaseLayer)
function VIPLayer:ctor()
	self.super.ctor(self)
	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("VIP/VIP.csb", self)
	self._csbNodeAni = ExternalFun.loadTimeLine( "VIP/VIP.csb" )
	csbNode:runAction(self._csbNodeAni)
	self._csbNode = csbNode
	self.m_webView = nil
end

function VIPLayer:initUI()
	--按钮事件
	local btn_return = self._csbNode:getChildByName("VIP_bg"):getChildByName("btn_close")
    btn_return:setPressedActionEnabled(true)
	btn_return:addClickEventListener(handler(self,self.onButtonClickedEvent))

    local btn_chongzhi = self._csbNode:getChildByName("VIP_bg"):getChildByName("bt_chongzhi")
    btn_chongzhi:setPressedActionEnabled(true)
	btn_chongzhi:addClickEventListener(handler(self,self.onButtonClickedEvent))


   

      local img_level_0 = self._csbNode:getChildByName("VIP_bg"):getChildByName("Image_2")
      local img_level_size = img_level_0:getContentSize()
	GlobalUserItem.cbMemberOrder=2
      local img_level = cc.Sprite:createWithSpriteFrameName("vip_maozi_label_"..GlobalUserItem.cbMemberOrder..".png")
        img_level:setPosition(img_level_size.width/2, img_level_size.height/2)
        img_level_0:addChild(img_level)
    
    local nex=GlobalUserItem.cbMemberOrder+1
    local img_level_1 = self._csbNode:getChildByName("VIP_bg"):getChildByName("Image_2_0")
      local img_level1 = cc.Sprite:createWithSpriteFrameName("vip_maozi_label_"..nex..".png")
        img_level1:setPosition(img_level_size.width/2, img_level_size.height/2)
        img_level_1:addChild(img_level1)


        local img_touxiang = self._csbNode:getChildByName("VIP_bg"):getChildByName("im_touxiang")
      local img_touxiang_size = img_level_0:getContentSize()

	if GlobalUserItem.cbMemberOrder ~= 0 then
      local img_touxiang_r = cc.Sprite:createWithSpriteFrameName("vip_head_label_"..GlobalUserItem.cbMemberOrder..".png")
        img_touxiang_r:setPosition(img_level_size.width/1.75, img_level_size.height*1.52)
        img_touxiang:addChild(img_touxiang_r)
    end


    --local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	--local gonggao_cont = self._csbNode:getChildByName("gonggao_bg"):getChildByName("gonggao_cont")
	--local contentSize = gonggao_cont:getContentSize()
--    if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
--        --介绍页面
--        self.m_webView = ccexp.WebView:create()
--        self.m_webView:setPosition(cc.p(contentSize.width / 2, contentSize.height / 2))
--        self.m_webView:setContentSize(contentSize)

--        self.m_webView:setScalesPageToFit(true) 
--        --local times = os.time()
--        local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/AdsNotice.aspx"

--        self.m_webView:loadURL(url)
--        --self.m_webView:setJavascriptInterfaceScheme(URL_SCHEME)

--        self:showPopWait()

--        self.m_webView:setOnDidFailLoading(function ( sender, url )
--            self:dismissPopWait()
--            print("open " .. url .. " fail")
--        end)
--        self.m_webView:setOnShouldStartLoading(function(sender, url)
--            print("onWebViewShouldStartLoading, url is ", url)          
--            return true
--        end)
--        self.m_webView:setOnDidFinishLoading(function(sender, url)
--            self:dismissPopWait()
--            ExternalFun.visibleWebView(self.m_webView, true)
--        end)
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

       -- gonggao_cont:addChild(self.m_webView)
	--end
	
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
		local pBg = self._csbNode:getChildByName("VIP_bg")
		pos = pBg:convertToNodeSpace(pos)
		local rec = cc.rect(0, 0, pBg:getContentSize().width, pBg:getContentSize().height)
		if false == cc.rectContainsPoint(rec, pos) then
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.GONGGAO_LAYER})
		end
		return true
	end,cc.Handler.EVENT_TOUCH_ENDED )
	self:getEventDispatcher():addEventListenerWithSceneGraphPriority(Layoutlistener, mask)
end

function VIPLayer:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
end

--关闭等待
function VIPLayer:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
end


function VIPLayer:onWillViewEnter()
	self:initUI()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
	end)

	self._csbNodeAni:play("openAni", false)
end

function VIPLayer:onWillViewExit()
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
		self.m_webView:setVisible(false)
	end
	
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

function VIPLayer:onButtonClickedEvent(sender)
	if (sender:getName() == "btn_close") then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.VIP_LAYER})
		--AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.GONGGAO_LAYER)
	end
    if (sender:getName() == "bt_chongzhi") then
        AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.SHOP_LAYER)
	end
end

return VIPLayer