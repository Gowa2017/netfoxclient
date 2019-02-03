--
-- Author: zhong
-- Date: 2016-09-01 16:11:55
--
local TrumpetSendLayer = class("TrumpetSendLayer", cc.Layer)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local QueryDialog   = appdf.req("app.views.layer.other.QueryDialog")
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

local targetPlatform = cc.Application:getInstance():getTargetPlatform()

local BT_CLOSE = 101
local BT_CANCEL = 102
local BT_SURE = 103

function TrumpetSendLayer:ctor(scene)
    self._scene = scene
    --注册触摸事件
    ExternalFun.registerTouchEvent(self, true)
    --加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("plaza/TrumpetSendLayer.csb", self)

    self.m_spBg = csbNode:getChildByName("sp_bg")
    if targetPlatform == cc.PLATFORM_OS_ANDROID then
        self.m_spBg:setScale(0.000001)
    else
        self.m_spBg:setScale(1.0)
    end
    local spbg = self.m_spBg

    --喇叭数量
    self.m_altasTrumpetNum = spbg:getChildByName("atlas_left")

    --编辑区
    local tmp = spbg:getChildByName("edit_bg")
    if device.platform == "ios" then
        local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, 60),"blank.png",UI_TEX_TYPE_PLIST)
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入要发送的消息")
            :setMaxLength(64)
        spbg:addChild(editbox)
        editbox:setContentSize(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10))
        editbox:setPlaceholderFont("fonts/round_body.ttf", 30)
        editbox:setFont("fonts/round_body.ttf", 30)   
        self.m_editTrumpet = editbox
    else
        local editbox = ccui.EditBox:create(cc.size(tmp:getContentSize().width - 10, tmp:getContentSize().height - 10),"blank.png",UI_TEX_TYPE_PLIST)
            :setPosition(tmp:getPosition())
            :setPlaceHolder("请输入要发送的消息")
            :setPlaceholderFont("fonts/round_body.ttf", 30)
            :setFont("fonts/round_body.ttf", 30)  
            :setMaxLength(64)
        spbg:addChild(editbox)         
        self.m_editTrumpet = editbox
    end

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    local btn = spbg:getChildByName("btn_close")
    btn:setTag(BT_CLOSE)
    btn:addTouchEventListener(btncallback)

    btn = spbg:getChildByName("btn_cancel")
    btn:setTag(BT_CANCEL)
    btn:addTouchEventListener(btncallback)

    btn = spbg:getChildByName("btn_sure")
    btn:setTag(BT_SURE)
    btn:addTouchEventListener(btncallback)

    --加载动画
    local call = cc.CallFunc:create(function()
        self:setVisible(true)
        self.m_altasTrumpetNum:setString("" .. GlobalUserItem.nLargeTrumpetCount)
    end)
    local scale = cc.ScaleTo:create(0.2, 1.0)
    self.m_actShowAct = cc.Sequence:create(call, scale)
    ExternalFun.SAFE_RETAIN(self.m_actShowAct)

    local scale1 = cc.ScaleTo:create(0.2, 0.0001)
    local call1 = cc.CallFunc:create(function( )
        self:setVisible(false)
        self.m_editTrumpet:setText("")
    end)
    self.m_actHideAct = cc.Sequence:create(scale1, call1)
    ExternalFun.SAFE_RETAIN(self.m_actHideAct)

    self:setVisible(false)
end

function TrumpetSendLayer:showLayer( var )
    local ani = nil
    if var then
        ani = self.m_actShowAct
    else 
        ani = self.m_actHideAct
    end

    if nil ~= ani then
        self.m_spBg:stopAllActions()
        if targetPlatform == cc.PLATFORM_OS_ANDROID then
            self.m_spBg:runAction(ani)
        else
            self:setVisible(var)
            if var then
                self.m_altasTrumpetNum:setString("" .. GlobalUserItem.nLargeTrumpetCount)
            end
        end
    end
end

function TrumpetSendLayer:onButtonClickedEvent( tag, sender )
    if BT_CLOSE == tag then
        self:showLayer(false)
    elseif BT_SURE == tag then
        if GlobalUserItem.nLargeTrumpetCount < 1 then
            self:showLayer(false)
            local query = QueryDialog:create("您的喇叭数量不足，是否前往商城购买！", function(ok)
                if ok == true then
                    self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_PROPERTY)
                end
                query = nil
            end):setCanTouchOutside(false)
                :addTo(self._scene)
            return
        end

        local content = self.m_editTrumpet:getText()
        if string.len(content)<1 then
            showToast(self, "喇叭内容不能为空", 3)
        else
            --判断emoji
            if ExternalFun.isContainEmoji(content) then
                showToast(self, "喇叭内容包含非法字符,请重试", 2)
                return
            end

            --敏感词过滤  
            if true == ExternalFun.isContainBadWords(content) then
                showToast(self, "喇叭内容包含敏感词汇!", 3)
                return
            end 

            local msgTab = {}
            msgTab.dwSenderID = GlobalUserItem.dwUserID
            msgTab.szNickName = GlobalUserItem.szNickName
            msgTab.dwFontColor = 16777215
            msgTab.cbFontSize = 0
            msgTab.cbFontAttri = 0
            msgTab.szFontName = "微软雅黑"
            msgTab.szMessageContent = content

            --喇叭使用后数量控制在使用结果处理
            FriendMgr:getInstance():sendTrupmet(msgTab)

            self:showLayer(false)
            --self.m_editTrumpet:setText("")
        end 
    elseif BT_CANCEL == tag then
        self:showLayer(false)
    end
end

function TrumpetSendLayer:onExit()
    ExternalFun.SAFE_RELEASE(self.m_actShowAct)
    self.m_actShowAct = nil
    ExternalFun.SAFE_RELEASE(self.m_actHideAct)
    self.m_actHideAct = nil
end

function TrumpetSendLayer:onTouchBegan(touch, event)
    return self:isVisible()
end

function TrumpetSendLayer:onTouchEnded(touch, event)
    local pos = touch:getLocation() 
    local m_spBg = self.m_spBg
    pos = m_spBg:convertToNodeSpace(pos)
    local rec = cc.rect(0, 0, m_spBg:getContentSize().width, m_spBg:getContentSize().height)
    if false == cc.rectContainsPoint(rec, pos) then
        --self:showLayer(false)
    end
end

function TrumpetSendLayer:showPopWait()
    self._scene:showPopWait()
end

function TrumpetSendLayer:dismissPopWait()
    self._scene:dismissPopWait()
end

return TrumpetSendLayer