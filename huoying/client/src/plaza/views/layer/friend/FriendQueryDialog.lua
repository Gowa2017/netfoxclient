--
-- Author: zhong
-- Date: 2017-02-10 15:16:26
--
local FriendQueryDialog = class("FriendQueryDialog", function(msg,callback)
        local FriendQueryDialog = display.newLayer()
    return FriendQueryDialog
end)

--默认字体大小
FriendQueryDialog.DEF_TEXT_SIZE   = 32

--UI标识
FriendQueryDialog.DG_QUERY_EXIT   =  2 
FriendQueryDialog.BT_CANCEL       =  0   
FriendQueryDialog.BT_CONFIRM      =  1

-- 对话框类型
FriendQueryDialog.QUERY_SURE          = 1
FriendQueryDialog.QUERY_SURE_CANCEL   = 2

-- 进入场景而且过渡动画结束时候触发。
function FriendQueryDialog:onEnterTransitionFinish()
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function FriendQueryDialog:onExitTransitionStart()
    self:unregisterScriptTouchHandler()
    return self
end

--窗外触碰
function FriendQueryDialog:setCanTouchOutside(canTouchOutside)
    self._canTouchOutside = canTouchOutside
    return self
end

--msg 显示信息
--callback 交互回调
--txtsize 字体大小
function FriendQueryDialog:ctor(msgTab, callback, txtsize, queryType)
    queryType = queryType or FriendQueryDialog.QUERY_SURE_CANCEL
    self._callback = callback
    self._canTouchOutside = true

    local this = self 
    self:setContentSize(appdf.WIDTH,appdf.HEIGHT)
    self:move(0,appdf.HEIGHT)

    --回调函数
    self:registerScriptHandler(function(eventType)
        if eventType == "enterTransitionFinish" then    -- 进入场景而且过渡动画结束时候触发。
            this:onEnterTransitionFinish()
        elseif eventType == "exitTransitionStart" then  -- 退出场景而且开始过渡动画时候触发。
            this:onExitTransitionStart()
        end
    end)

    --按键监听
    local  btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
            this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    --区域外取消显示
    local  onQueryExitTouch = function(eventType, x, y)
        if not self._canTouchOutside then
            return true
        end

        if self._dismiss == true then
            return true
        end

        if eventType == "began" then
            local rect = this:getChildByTag(FriendQueryDialog.DG_QUERY_EXIT):getBoundingBox()
            if cc.rectContainsPoint(rect,cc.p(x,y)) == false then
                self:dismiss()
            end
        end
        return true
    end
    self:setTouchEnabled(true)
    self:registerScriptTouchHandler(onQueryExitTouch)

    display.newSprite("query_bg.png")
        :setTag(FriendQueryDialog.DG_QUERY_EXIT)
        :move(appdf.WIDTH/2,appdf.HEIGHT/2)
        :addTo(self)

    if FriendQueryDialog.QUERY_SURE == queryType then
        ccui.Button:create("bt_query_confirm_0.png","bt_query_confirm_1.png")
            :move(appdf.WIDTH/2 , 200 )
            :setTag(FriendQueryDialog.BT_CONFIRM)
            :addTo(self)
            :addTouchEventListener(btcallback)
    else
        ccui.Button:create("bt_query_confirm_0.png","bt_query_confirm_1.png")
            :move(appdf.WIDTH/2+169 , 200 )
            :setTag(FriendQueryDialog.BT_CONFIRM)
            :addTo(self)
            :addTouchEventListener(btcallback)

        ccui.Button:create("bt_query_cancel_0.png","bt_query_cancel_1.png")
            :move(appdf.WIDTH/2-169 ,200 )
            :setTag(FriendQueryDialog.BT_CANCEL)
            :addTo(self)
            :addTouchEventListener(btcallback)
    end

    cc.Label:createWithTTF("提示", "fonts/round_body.ttf", 36)
        :setTextColor(cc.c4b(255,221,65,255))
        :setAnchorPoint(cc.p(0.5,0.5))
        :setDimensions(600, 120)
        :setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
        :setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
        :move(appdf.WIDTH/2 ,545 )
        :addTo(self)

    local mainMsg = msgTab.main or ""
    local mainText = cc.Label:createWithTTF(mainMsg, "fonts/round_body.ttf", not txtsize and FriendQueryDialog.DEF_TEXT_SIZE or txtsize)
        :setTextColor(cc.c4b(255,255,255,255))
        :setAnchorPoint(cc.p(0.5,0.5))
        :setDimensions(600, 0)
        :setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
        :setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
        :move(appdf.WIDTH/2 ,395)
        :addTo(self)
    local subMsg = msgTab.sub or ""
    if "" ~= subMsg then
        local con = mainText:getContentSize()
        local yPos = 365 - con.height * 0.5
        cc.Label:createWithTTF(subMsg, "fonts/round_body.ttf", not txtsize and FriendQueryDialog.DEF_TEXT_SIZE or txtsize)
            :setTextColor(cc.c4b(255,255,255,255))
            :setAnchorPoint(cc.p(0.5,0.5))
            :setDimensions(600, 0)
            :setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
            :setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
            :move(appdf.WIDTH/2 ,yPos)
            :addTo(self)
    end
    self._dismiss  = false
    self:runAction(cc.MoveTo:create(0.3,cc.p(0,0)))
end

--按键点击
function FriendQueryDialog:onButtonClickedEvent(tag,ref)
    if self._dismiss == true then
        return
    end
    --取消显示
    self:dismiss()
    --通知回调
    if self._callback then
        self._callback(tag == FriendQueryDialog.BT_CONFIRM)
    end
end

--取消消失
function FriendQueryDialog:dismiss()
    self._dismiss = true
    local this = self
    self:stopAllActions()
    self:runAction(
        cc.Sequence:create(
            cc.MoveTo:create(0.3,cc.p(0,appdf.HEIGHT)),
            cc.CallFunc:create(function()
                    this:removeSelf()
                end)
            ))  
end

return FriendQueryDialog
