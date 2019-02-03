--[[
	赚金界面
]]

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

local EarnLayer = class("EarnLayer", cc.Layer)

-- 进入场景而且过渡动画结束时候触发。
function EarnLayer:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function EarnLayer:onExitTransitionStart()
    return self
end

EarnLayer.RTO_1 = 1 --推广统计
EarnLayer.RTO_2 = 2 --推广管理
EarnLayer.RTO_3 = 3 --赚金说明
EarnLayer.RTO_4 = 4 --无

function EarnLayer:ctor(scene)
	
	self._scene = scene
	
	--节点事件
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
		end
	end)
	
	--加载csb资源
	local rootLayer, csbNode = ExternalFun.loadRootCSB("Earn/EarnLayer.csb", self)
	self._csbNode = csbNode
	

	local function btnEvent( sender, eventType )
		if eventType == ccui.TouchEventType.ended then
			self:onButtonClickedEvent(sender)
		end
	end	
	
	
	local btnClose = csbNode:getChildByName("btnClose")
	btnClose:addTouchEventListener(btnEvent)
	
	--左侧单选按钮
    self._radioButtonGroup = ccui.RadioButtonGroup:create()
    self._radioButtonGroup:setAllowedNoSelection(false)
    self:addChild(self._radioButtonGroup)
	
	
	

	--RadioBox回调
	local rtolistener = function (sender,eventType)
		self:onSelectedEvent(sender,eventType)
	end

	local default_button_image_show_count = 3
	local default_button_image_total_count = 4
	for i=1,default_button_image_show_count,1 do
		local radioBtn = nil
		--只有三张图，所以对3求余
		
		local strBackGround = string.format("Earn/earn_btn_%d_1.png", i % default_button_image_total_count)
		local strBackGroundSeleted = string.format("Earn/earn_btn_%d_2.png", i % default_button_image_total_count)
		local strCross = string.format("Earn/earn_btn_%d_2.png", i % default_button_image_total_count)
		
		radioBtn = ccui.RadioButton:create(strBackGround,strBackGroundSeleted,strCross,"","")
		radioBtn:setTag(i)
		
		local Container_1 = self._csbNode:getChildByName(string.format("rtn_container_%d", i))
		--取出csb的Button容器，没有直接跳过	
		assert(type(Container_1) == "userdata", "expected userdata, but not now!")
		if (type(Container_1) == "userdata") then
			self:addRadioButton(radioBtn, Container_1, self._radioButtonGroup, rtolistener)
		end
	end
	local ostime = os.time()
	local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/MarketCount.aspx?userid=" .. GlobalUserItem.dwUserID .. "&signature=" .. GlobalUserItem:getSignature(ostime) .. "&time=" ..ostime
	self:loadWeb(url)
end

function EarnLayer:loadWeb(url, webContent)
	local webContent = self._csbNode:getChildByName("Content")
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()

	if (nil ~= self._webView) then
		self._webView:removeFromParent()
	end
	
    if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
        --介绍页面
        self._webView = ccexp.WebView:create()
		self._webView:setName("EarnMoneyWebView")
		self._webView:setContentSize(webContent:getContentSize())
        self._webView:setPosition(cc.p(webContent:getContentSize().width / 2, webContent:getContentSize().height / 2))
        self._webView:setScalesPageToFit(true) 

        self._webView:loadURL(url)
        --self._webView:setJavascriptInterfaceScheme("http")
        ExternalFun.visibleWebView(self._webView, webContent:isVisible())

        self._webView:setOnDidFailLoading(function ( sender, url )
            self._scene:dismissPopWait()
            print("open " .. url .. " fail")
        end)
        self._webView:setOnShouldStartLoading(function(sender, url)
			self._scene:showPopWait()
            print("onWebViewShouldStartLoading, url is ", url)          
            return true
        end)
        self._webView:setOnDidFinishLoading(function(sender, url)
            self._scene:dismissPopWait()
            ExternalFun.visibleWebView(self._webView, true)
        end)
--[[        self._webView:setOnJSCallback(function ( sender, url )
            if url == "http://param:close" then
                self._scene:dismissPopWait()
                self:removeFromParent()
            else
                self._scene:queryUserScoreInfo(function(ok)
                    if ok then
                        self._webView:reload()
                    end
                end)
            end            
        end)
--]]
        webContent:addChild(self._webView)
	else
		webContent:setBackGroundColorType(ccui.LayoutBackGroundColorType.solid)
		webContent:setBackGroundColor(cc.c3b(255, 0, 255))
    end
end

function EarnLayer:addRadioButton(radiobtn, parent, group, callback)
    --check parm
    assert(type(radiobtn) == "userdata", "expected userdata data, but not now!")
    if (type(radiobtn) ~= "userdata") then
        return
    end

    assert(type(parent) == "userdata", "expected userdata data, but not now!")
    if (type(parent) ~= "userdata") then
        return
    end

    assert(type(group) == "userdata", "expected userdata data, but not now!")
    if (type(group) ~= "userdata") then
        return
    end

    assert(type(callback) == "function", "expected function, but not now!")
    if (type(callback) ~= "function") then
        return
    end

    local ContainerSize_1 = parent:getContentSize()
	radiobtn:move(ContainerSize_1.width / 2,ContainerSize_1.height / 2)
		:addTo(parent)
        :setVisible(true)
        :setEnabled(true)
		:addEventListener(callback)
    group:addRadioButton(radiobtn)
end

function EarnLayer:onSelectedEvent(sender,eventType)
    if (ccui.RadioButtonEventType.selected == eventType) then
		local tag = sender:getTag()
        
        local targetPlatform = cc.Application:getInstance():getTargetPlatform()
		if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
            if (tag == EarnLayer.RTO_1) then
				local ostime = os.time()
				local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/MarketCount.aspx?userid=" .. GlobalUserItem.dwUserID .. "&signature=" .. GlobalUserItem:getSignature(ostime) .. "&time=" ..ostime	
				self:loadWeb(url)
			elseif (tag == EarnLayer.RTO_2) then
				local ostime = os.time()
				local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/MarketGain.aspx?userid=" .. GlobalUserItem.dwUserID .. "&signature=" .. GlobalUserItem:getSignature(ostime) .. "&time=" ..ostime	
				self:loadWeb(url)
			elseif (tag == EarnLayer.RTO_3) then
				local ostime = os.time()
				local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/MarketDetails.aspx"	
				self:loadWeb(url)
			elseif (tag == EarnLayer.RTO_4) then
				local ostime = os.time()
				local url = "www.baidu.com"	
				self:loadWeb(url)
			end
		else
			if (tag == EarnLayer.RTO_1) then
				local ostime = os.time()
				local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/MarketCount.aspx?userid=" .. GlobalUserItem.dwUserID .. "&signature=" .. GlobalUserItem:getSignature(ostime) .. "&time=" ..ostime
				local i = 0
			elseif (tag == EarnLayer.RTO_2) then
				local ostime = os.time()
				local url = BaseConfig.WEB_HTTP_URL .. "/Mobile/MarketGain.aspx?userid=" .. GlobalUserItem.dwUserID .. "&signature=" .. GlobalUserItem:getSignature(ostime) .. "&time=" ..ostime	
				local i = 0
			elseif (tag == EarnLayer.RTO_3) then
				local ostime = os.time()
				local url = "www.baidu.com"	
				local i = 0
			elseif (tag == EarnLayer.RTO_4) then
				local ostime = os.time()
				local url = "www.baidu.com"	
				local i = 0
			end
		end
	end
end

--按键监听
function EarnLayer:onButtonClickedEvent(sender)
	ExternalFun.playClickEffect()
	if ("btnClose" == sender:getName()) then
		self._scene:onKeyBack()
	end
end

return EarnLayer