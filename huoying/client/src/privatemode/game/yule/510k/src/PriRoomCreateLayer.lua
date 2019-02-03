--
-- Author: zhong
-- Date: 2016-12-17 14:07:02
--
-- 斗地主私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")

local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

local BTN_HELP = 1
local BTN_CHARGE = 2
local BTN_MYROOM = 3
local BTN_CREATE = 4
local CBT_BEGIN = 300
local CBT_A_2_BIGEST = 400
local CBT_CELL = 500
local BTN_LIMIT = 600

function PriRoomCreateLayer:ctor( scene )
    PriRoomCreateLayer.super.ctor(self, scene)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("room/PrivateRoomCreateLayer.csb", self )
    self.m_csbNode = csbNode

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 帮助按钮
    local btn = csbNode:getChildByName("btn_help")
    btn:setTag(BTN_HELP)
    btn:addTouchEventListener(btncallback)

    -- 充值按钮
    btn = csbNode:getChildByName("btn_cardcharge")
    btn:setTag(BTN_CHARGE)
    btn:addTouchEventListener(btncallback)    

    -- 房卡数
    self.m_txtCardNum = csbNode:getChildByName("txt_cardnum")
    self.m_txtCardNum:setString(GlobalUserItem.lRoomCard .. "")

    -- 我的房间
    btn = csbNode:getChildByName("btn_myroom")
    btn:setTag(BTN_MYROOM)
    btn:addTouchEventListener(btncallback)

    -- 滑动条
    local tabPercent = {3, 27, 50, 73, 98}
    local tabCount = {1, 2, 5, 10, 20}
    local function percentChangedEvent( sender,eventType )
        if eventType == ccui.SliderEventType.slideBallUp then
            local idx = 1            
            local percent = sender:getPercent()
            if 0 <= percent and percent < 13 then
                idx = 1
            elseif 13 <= percent and percent < 38 then
                idx = 2
            elseif 38 <= percent and percent < 63 then
                idx = 3
            elseif 63 <= percent and percent < 88 then
                idx = 4
            elseif 88 <= percent and percent <= 100 then
                idx = 5
            end
            local disper = tabPercent[idx]
            sender:setPercent(disper)
            local count = tabCount[idx]
            self.m_txtSelCount:setString(count .. "局")
            self.m_nSelectScore = count
        end
    end
    self.m_sliderLimit = csbNode:getChildByName("slider_limit")
    self.m_sliderLimit:setVisible(false)
    self.m_sliderLimit:setPercent(3)
    self.m_sliderLimit:addEventListener(percentChangedEvent)
    self.m_nSelectScore = 1

    local function btnLimtcallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonLimitClickedEvent(ref:getTag(),ref)
        end
    end
    self.m_tabBtnLimit = {}
    for i = 1, 5 do
        local btn_limit = csbNode:getChildByName("btn_limit_"..i)
        btn_limit:setVisible(false)
        btn_limit:setTag(BTN_LIMIT + i)
        self.m_tabBtnLimit[i] = btn_limit
        btn_limit:addTouchEventListener(btnLimtcallback)  
    end

   

    

    local cbtlistener = function (sender,eventType)
        self:onSelectedEvent(sender:getTag(),sender)
    end

    local cbCellListener = function (sender,eventType)
        self:onCellSelectedEvent(sender:getTag(),sender)
    end

    local ruleListener = function ( sender,eventType )
        self:onBigestSelectedEvent(sender:getTag(),sender)
    end

     -- 倍数选择
    self.m_tabCellCheckBox = {}
    for k,v in pairs(tabCount) do
        local check = csbNode:getChildByName(string.format("check_%d",k))
        check:setTag(CBT_CELL + k)
        check:setVisible(true)
        check:setSelected(false)
        check:addEventListener(cbCellListener)
        local txt = csbNode:getChildByName("count_" .. k)
        txt:setString(v .. "分")
        self.m_tabCellCheckBox[k] = check
    end
    self.m_tabCellCheckBox[1]:setSelected(true)
    --A大为0 2大为1  
    self.m_nSelectBigest = 0
    for i = 1, 2 do
        local check_rule = csbNode:getChildByName(string.format("check_rule_%d", i))
        check_rule:setTag(CBT_A_2_BIGEST + i)
        check_rule:setVisible(true)
        check_rule:addEventListener(ruleListener)
        if i == 1 then
            check_rule:setSelected(true)
        else
            check_rule:setSelected(false)
        end
    end
    
    
    self.m_tabCheckBox = {}
    -- 局数选项
    dump(PriRoom:getInstance().m_tabFeeConfigList, "----- m_tabFeeConfigList -----", 6)
    for i = 1, 5  do
        
        
        local checkbx = csbNode:getChildByName("check_limit_" .. i)
        print("check_" .. i,nil == checkbx)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTag(CBT_BEGIN + i)
            checkbx:addEventListener(cbtlistener)
            checkbx:setSelected(false)

            self.m_tabCheckBox[CBT_BEGIN + i] = checkbx
            if i > #PriRoom:getInstance().m_tabFeeConfigList then
                checkbx:setVisible(false)
            end
            
        end

        local txtcount = csbNode:getChildByName("txt_limit_" .. i)
        if nil ~= txtcount then
            if i > #PriRoom:getInstance().m_tabFeeConfigList then
                txtcount:setVisible(false)
            else
                local config = PriRoom:getInstance().m_tabFeeConfigList[i]
                txtcount:setString(config.dwDrawCountLimit .. "局")
            end
            
        end
    end
    -- 局数
    self.m_txtSelCount = csbNode:getChildByName("txt_selcount")
    if 0 ~= #PriRoom:getInstance().m_tabFeeConfigList then
        self.m_txtSelCount:setString(PriRoom:getInstance().m_tabFeeConfigList[1].dwDrawCountLimit .. "局")
    else
        self.m_txtSelCount:setVisible(false)
    end
    self.m_txtSelCount:setVisible(false)

    self.pri_sp_countbg_9 = csbNode:getChildByName("pri_sp_countbg_9")
    self.pri_sp_countbg_9:setVisible(false)

    -- 选择的玩法    
    self.m_nSelectIdx = CBT_BEGIN + 1
    self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[self.m_nSelectIdx - CBT_BEGIN]
    self.m_tabCheckBox[self.m_nSelectIdx]:setSelected(true)

    self.m_bLow = false
    -- 创建费用
    self.m_txtFee = csbNode:getChildByName("txt_fee")
    self.m_txtFee:setString("")
    print("init",GlobalUserItem.lRoomCard ,self.m_tabSelectConfig.lFeeScore)
    if GlobalUserItem.lRoomCard < self.m_tabSelectConfig.lFeeScore then
        self.m_bLow = true
    end
    local feeType = "房卡"
    if nil ~= self.m_tabSelectConfig then        
        if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
            feeType = "游戏豆"
            self.m_bLow = false
            if GlobalUserItem.dUserBeans < self.m_tabSelectConfig.lFeeScore then
                self.m_bLow = true
            end
        end
        self.m_txtFee:setString(self.m_tabSelectConfig.lFeeScore .. feeType)
    end
    print("---- PriRoom:getInstance().m_tabRoomOption.cbCardOrBean ----",PriRoom:getInstance().m_tabRoomOption.cbCardOrBean)
    -- 提示
    self.m_spTips = csbNode:getChildByName("priland_sp_card_tips")
    self.m_spTips:setVisible(false)
    self.m_spTips:setVisible(self.m_bLow)
    if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
        self.m_spTips:loadTexture("room/pri510k_sp_card_tips_bean.png")
    end

    -- 创建按钮
    btn = csbNode:getChildByName("btn_createroom")
    btn:setTag(BTN_CREATE)
    btn:addTouchEventListener(btncallback)
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriRoomCreateLayer:onRefreshInfo()
    -- 房卡数更新
    self.m_txtCardNum:setString(GlobalUserItem.lRoomCard .. "")
end

function PriRoomCreateLayer:onLoginPriRoomFinish()
    local meUser = PriRoom:getInstance():getMeUserItem()
    if nil == meUser then
        return false
    end
    -- 发送创建桌子
    if ((meUser.cbUserStatus == yl.US_FREE or meUser.cbUserStatus == yl.US_NULL or meUser.cbUserStatus == yl.US_PLAYING)) then
        if PriRoom:getInstance().m_nLoginAction == PriRoom.L_ACTION.ACT_CREATEROOM then
            -- 创建登陆
            local buffer = CCmd_Data:create(188)
            buffer:setcmdinfo(self._cmd_pri_game.MDM_GR_PERSONAL_TABLE,self._cmd_pri_game.SUB_GR_CREATE_TABLE)
            --510 分数和局数2个字段调换位置
            print("----- self.m_nSelectScore -----",self.m_nSelectScore)
            buffer:pushscore(self.m_nSelectScore)
            buffer:pushdword(self.m_tabSelectConfig.dwDrawCountLimit)
            buffer:pushdword(self.m_tabSelectConfig.dwDrawTimeLimit)
            buffer:pushword(4)
            buffer:pushdword(0)
            buffer:pushstring("", yl.LEN_PASSWORD)
            --自定义规则
            buffer:pushbyte(1)
            buffer:pushbyte(self.m_nSelectBigest)
            --[[
            for i = 1, 100 do
                buffer:pushbyte(0)
            end
            --]]
            PriRoom:getInstance():getNetFrame():sendGameServerMsg(buffer)
            return true
        end        
    end
    return false
end

function PriRoomCreateLayer:getInviteShareMsg( roomDetailInfo )
    local shareTxt = "斗地主约战 房间ID:" .. roomDetailInfo.szRoomID .. " 局数:" .. roomDetailInfo.dwPlayTurnCount
    local friendC = "斗地主房间ID:" .. roomDetailInfo.szRoomID .. " 局数:" .. roomDetailInfo.dwPlayTurnCount
    return {title = "斗地主约战", content = shareTxt .. " 斗地主游戏精彩刺激, 一起来玩吧! ", friendContent = friendC}
end

function PriRoomCreateLayer:onExit()
    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("room/land_room.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("room/land_room.png")
end

------
-- 继承/覆盖
------

function PriRoomCreateLayer:onButtonClickedEvent( tag, sender)
    if BTN_HELP == tag then
        self._scene:popHelpLayer2(200, 1)
        --self._scene:popHelpLayer(yl.HTTP_URL .. "/Mobile/Introduce.aspx?kindid=200&typeid=1")
    elseif BTN_CHARGE == tag then
        local feeType = "房卡"
        if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
            feeType = "游戏豆"
        end
        if feeType == "游戏豆" then
            self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_BEAN)
        else
            self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_PROPERTY)
        end
    elseif BTN_MYROOM == tag then
        self._scene:onChangeShowMode(PriRoom.LAYTAG.LAYER_MYROOMRECORD)
    elseif BTN_CREATE == tag then 
        if self.m_bLow then
            local feeType = "房卡"
            if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
                feeType = "游戏豆"
            end

            local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
            local query = QueryDialog:create("您的" .. feeType .. "数量不足，是否前往商城充值！", function(ok)
                if ok == true then
                    if feeType == "游戏豆" then
                        self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_BEAN)
                    else
                        self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_PROPERTY)
                    end                    
                end
                query = nil
            end):setCanTouchOutside(false)
                :addTo(self._scene)
            return
        end
        if nil == self.m_tabSelectConfig or table.nums(self.m_tabSelectConfig) == 0 then
            showToast(self, "未选择玩法配置!", 2)
            return
        end
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
    end
end

function PriRoomCreateLayer:onButtonLimitClickedEvent( tag, sender)
    -- 滑动条
    local tabPercent = {3, 27, 50, 73, 98}
    local tabCount = {10, 20, 30, 40, 50}
    
    self.m_nSelectScore = 1
    local index = tag - BTN_LIMIT
    local count = tabCount[index]
    local disper = tabPercent[index]
    self.m_sliderLimit:setPercent(disper)
    self.m_txtSelCount:setString(count .. "局")
    self.m_nSelectScore = count
    print("--- self.m_nSelectScore ---"..self.m_nSelectScore)
end

function PriRoomCreateLayer:onBigestSelectedEvent(tag, sender)

    local check_rule_1 = self.m_csbNode:getChildByTag(CBT_A_2_BIGEST + 1)
    local check_rule_2 = self.m_csbNode:getChildByTag(CBT_A_2_BIGEST + 2)

    if tag == CBT_A_2_BIGEST + 1 then
        check_rule_1:setSelected(true)
        check_rule_2:setSelected(false)
        self.m_nSelectBigest = 0
    else
        check_rule_1:setSelected(false)
        check_rule_2:setSelected(true)
        self.m_nSelectBigest = 1
    end
end

function PriRoomCreateLayer:onCellSelectedEvent(tag, sender)
    local index = tag - CBT_CELL
    local tabCount = {1, 2, 5, 10, 20}
    
    self.m_nSelectScore = tabCount[index]
    for k,v in pairs(self.m_tabCellCheckBox) do
        if k ~= index then
            v:setSelected(false)
        else
            v:setSelected(true)
        end
    end
end

function PriRoomCreateLayer:onSelectedEvent(tag, sender)
    if self.m_nSelectIdx == tag then
        sender:setSelected(true)
        return
    end
    self.m_nSelectIdx = tag
    for k,v in pairs(self.m_tabCheckBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
    self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[tag - CBT_BEGIN]
    if nil == self.m_tabSelectConfig then
        return
    end
    print("onSelectedEvent",GlobalUserItem.lRoomCard ,self.m_tabSelectConfig.lFeeScore)
    self.m_bLow = false
    if GlobalUserItem.lRoomCard < self.m_tabSelectConfig.lFeeScore then
        self.m_bLow = true
    end
    local feeType = "房卡"
    if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
        feeType = "游戏豆"
        self.m_bLow = false
        if GlobalUserItem.dUserBeans < self.m_tabSelectConfig.lFeeScore then
            self.m_bLow = true
        end
    end
    self.m_txtFee:setString(self.m_tabSelectConfig.lFeeScore .. feeType)
    self.m_spTips:setVisible(self.m_bLow)
    if self.m_bLow then

        if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
            self.m_spTips:loadTexture("room/pri510k_sp_card_tips_bean.png")
        else
            self.m_spTips:loadTexture("room/pri510k_sp_card_tips.png")
 
        end
    end
end

return PriRoomCreateLayer