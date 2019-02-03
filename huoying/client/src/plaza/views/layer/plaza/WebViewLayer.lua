--
-- Author: zhong
-- Date: 2016-08-02 10:31:30
--
local WebViewLayer = class("WebViewLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

--访问本地html文件
WebViewLayer.LOAD_FILE = 1 
--访问url
WebViewLayer.LOAD_URL = 2

--scheme
local URL_SCHEME = "ryweb"
local JS_SCHEME = "ryweb://"
local TAG_MASK = 101
local BTN_CLOSE = 102

function WebViewLayer:ctor( scene )
    self._scene = scene
    --加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/WebViewLayer.csb", self)
    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    -- 背景
    self.m_spBg = csbNode:getChildByName("image_bg")

    -- 退出按钮
    local btn = self.m_spBg:getChildByName("btn_close")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener(touchFunC)

    -- 今日不再显示
    local swith = self.m_spBg:getChildByName("check_switch")
    swith:addEventListener(function (sender,eventType) 
        GlobalUserItem.setTodayNoAdNotice(swith:isSelected()) 
    end)

    -- 界面
    local tmp = self.m_spBg:getChildByName("content")
    --dump(tmp:getContentSize(), "desciption", 6)
    --平台判定
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
        --介绍页面
        self.m_webView = ccexp.WebView:create()
        self.m_webView:setPosition(tmp:getPosition())
        self.m_webView:setContentSize(cc.size(710, 320))
        
        self.m_webView:setScalesPageToFit(true) 
        local times = os.time()
        local url = yl.HTTP_URL .. "/Mobile/AdsNotice.aspx"

        self.m_webView:loadURL(url)
        self.m_webView:setJavascriptInterfaceScheme(URL_SCHEME)
        ExternalFun.visibleWebView(self.m_webView, false)
        self._scene:showPopWait()

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
        end)
        self.m_webView:setOnJSCallback(function ( sender, url )
            if url == "ryweb://param:close" then
                self._scene:dismissPopWait()
                self:removeFromParent()
            else
                self._scene:queryUserScoreInfo(function(ok)
                    if ok then
                        self.m_webView:reload()
                    end
                end)
            end            
        end)

        self.m_spBg:addChild(self.m_webView)
    end
    tmp:removeFromParent()
end

function WebViewLayer:onButtonClickedEvent(tag, ref)
    if BTN_CLOSE == tag then
        self._scene:dismissPopWait()
        self:removeFromParent()
    end
end

function WebViewLayer:onKeyBack(  )
    if nil ~= self.m_webView and true == self.m_webView:canGoBack() then
        self.m_webView:goBack()
        return true
    end
    return false
end

function WebViewLayer:getSignature(times)
    print("******  WebViewLayer:getSignature ******")
    print("timevalue-"..times)
    local pstr = "3a5ke4a11fzb5c5e1" .. GlobalUserItem.dwUserID .. times
    pstr = string.lower(md5(pstr))
    print("signature-"..pstr)
    print("******  WebViewLayer:getSignature ******")
    return pstr
end

function WebViewLayer:reloadWebView()
    if nil ~= self.m_webView then
        self.m_webView:reload()
    end
end

return WebViewLayer