--
-- Author: zhong
-- Date: 2017-01-05 10:22:19
--
-- 玩法介绍
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local IntroduceLayer = class("IntroduceLayer", cc.Layer)

local TAG_MASK = 101
local BTN_CLOSE = 102
function IntroduceLayer:ctor( scene, url )
    self._scene = scene
    url = url or BaseConfig.WEB_HTTP_URL
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/IntroduceLayer.csb", self )

    local touchFunC = function(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(), ref)            
        end
    end

    -- 遮罩
    local mask = csbNode:getChildByName("panel_mask")
    mask:setTag(TAG_MASK)
    mask:addTouchEventListener( touchFunC )

    local image_bg = csbNode:getChildByName("image_bg")
    image_bg:setTouchEnabled(true)
    image_bg:setSwallowTouches(true)

    -- 退出按钮
    local btn = image_bg:getChildByName("btn_close")
    btn:setTag(BTN_CLOSE)
    btn:addTouchEventListener(touchFunC)

    -- 界面
    local tmp = image_bg:getChildByName("content")
    --平台判定
    local targetPlatform = cc.Application:getInstance():getTargetPlatform()
    if (cc.PLATFORM_OS_IPHONE == targetPlatform) or (cc.PLATFORM_OS_IPAD == targetPlatform) or (cc.PLATFORM_OS_ANDROID == targetPlatform) then
        --介绍页面
        self.m_webView = ccexp.WebView:create()
        self.m_webView:setPosition(tmp:getPosition())
        self.m_webView:setContentSize(tmp:getContentSize())
        
        self.m_webView:setScalesPageToFit(true)        
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
        end)
        image_bg:addChild(self.m_webView)
    end
    tmp:removeFromParent()
end

function IntroduceLayer:onButtonClickedEvent(tag, ref)
	ExternalFun.playClickEffect()
    if TAG_MASK == tag or BTN_CLOSE == tag then
        self._scene:dismissPopWait()
        self:removeFromParent()
    end
end

return IntroduceLayer