--
-- Author: zhong
-- Date: 2017-03-07 14:07:02
--
-- 牛牛私人房创建界面
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")

local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

local BTN_HELP = 1
local BTN_CHARGE = 2
local BTN_MYROOM = 3
local BTN_CREATE = 4
local CBT_BEGIN = 300
local CBT_MODEBEGIN = 400
local CBT_PLAYERBEGIN = 500

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

    -- 坐庄模式
    local modelistener = function (sender,eventType)
        self:onModeSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabModeCheckBox = {}
    self.m_nSelectMode = CBT_MODEBEGIN + 11
    -- 模式1
    local check = csbNode:getChildByName("modecheck_1")
    check:setTag(CBT_MODEBEGIN + 11)
    check:addEventListener(modelistener)
    check:setSelected(true)
    self.m_tabModeCheckBox[CBT_MODEBEGIN + 11] = check
    -- 模式2
    check = csbNode:getChildByName("modecheck_2")
    check:setTag(CBT_MODEBEGIN + 12)
    check:addEventListener(modelistener)
    check:setSelected(false)
    self.m_tabModeCheckBox[CBT_MODEBEGIN + 12] = check
    -- 模式3
    check = csbNode:getChildByName("modecheck_3")
    check:setTag(CBT_MODEBEGIN + 13)
    check:addEventListener(modelistener)
    check:setSelected(false)
    self.m_tabModeCheckBox[CBT_MODEBEGIN + 13] = check

    -- 游戏人数
    local playerlistener = function (sender,eventType)
        self:onPlayerSelectEvent(sender:getTag(),sender)
    end
    self.m_tabPlayerCheckBox = {}
    self.m_nSelectPlayer = CBT_PLAYERBEGIN + 2

    -- 2人
    local pCheck = csbNode:getChildByName("player_check_1")
    pCheck:setTag(CBT_PLAYERBEGIN + 2)
    pCheck:addEventListener(playerlistener)
    pCheck:setSelected(true)
    self.m_tabPlayerCheckBox[CBT_PLAYERBEGIN + 2] = pCheck
    -- 3人
    pCheck = csbNode:getChildByName("player_check_2")
    pCheck:setTag(CBT_PLAYERBEGIN + 3)
    pCheck:addEventListener(playerlistener)
    pCheck:setSelected(false)
    self.m_tabPlayerCheckBox[CBT_PLAYERBEGIN + 3] = pCheck
    -- 4人
    pCheck = csbNode:getChildByName("player_check_3")
    pCheck:setTag(CBT_PLAYERBEGIN + 4)
    pCheck:addEventListener(playerlistener)
    pCheck:setSelected(false)
    self.m_tabPlayerCheckBox[CBT_PLAYERBEGIN + 4] = pCheck
    -- 2-4人
    pCheck = csbNode:getChildByName("player_check_4")
    pCheck:setTag(CBT_PLAYERBEGIN + 0)
    pCheck:addEventListener(playerlistener)
    pCheck:setSelected(false)
    self.m_tabPlayerCheckBox[CBT_PLAYERBEGIN + 0] = pCheck

    local cbtlistener = function (sender,eventType)
        self:onSelectedEvent(sender:getTag(),sender)
    end
    self.m_tabCheckBox = {}
    -- 游戏局数
    for i = 1, #PriRoom:getInstance().m_tabFeeConfigList do
        local config = PriRoom:getInstance().m_tabFeeConfigList[i]
        local checkbx = csbNode:getChildByName("check_" .. i)
        if nil ~= checkbx then
            checkbx:setVisible(true)
            checkbx:setTag(CBT_BEGIN + i)
            checkbx:addEventListener(cbtlistener)
            checkbx:setSelected(false)
            self.m_tabCheckBox[CBT_BEGIN + i] = checkbx
        end

        local txtcount = csbNode:getChildByName("count_" .. i)
        if nil ~= txtcount then
            txtcount:setString(config.dwDrawCountLimit .. "局")
        end
    end
    -- 选择的玩法    
    self.m_nSelectIdx = CBT_BEGIN + 1
    self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[self.m_nSelectIdx - CBT_BEGIN]
    self.m_tabCheckBox[self.m_nSelectIdx]:setSelected(true)

    self.m_bLow = false
    -- 创建费用
    self.m_txtFee = csbNode:getChildByName("txt_fee")
    self.m_txtFee:setString("")
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

    -- 提示
    self.m_spTips = csbNode:getChildByName("priland_sp_card_tips")
    self.m_spTips:setVisible(self.m_bLow)
    if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
        local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("27_pri_sp_card_tips_bean.png")
        if nil ~= frame then
            self.m_spTips:setSpriteFrame(frame)
        end
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
            buffer:pushscore(1)
            buffer:pushdword(self.m_tabSelectConfig.dwDrawCountLimit)
            buffer:pushdword(self.m_tabSelectConfig.dwDrawTimeLimit)
            buffer:pushword(0)
            buffer:pushdword(0)
            buffer:pushstring("", yl.LEN_PASSWORD)

            --单个游戏规则(额外规则)
            buffer:pushbyte(1)
            buffer:pushbyte(self.m_nSelectPlayer - CBT_PLAYERBEGIN)
            buffer:pushbyte(self.m_nSelectMode - CBT_MODEBEGIN)
            for i = 1, 97 do
                buffer:pushbyte(0)
            end
            PriRoom:getInstance():getNetFrame():sendGameServerMsg(buffer)
            return true
        end        
    end
    return false
end

function PriRoomCreateLayer:getInviteShareMsg( roomDetailInfo )
    local shareTxt = "牛牛约战 房间ID:" .. roomDetailInfo.szRoomID .. " 局数:" .. roomDetailInfo.dwPlayTurnCount
    local friendC = "牛牛房间ID:" .. roomDetailInfo.szRoomID .. " 局数:" .. roomDetailInfo.dwPlayTurnCount
    return {title = "牛牛约战", content = shareTxt .. " 牛牛游戏精彩刺激, 一起来玩吧! ", friendContent = friendC}
end

function PriRoomCreateLayer:onExit()
    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("room/27_pri_room.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("room/27_pri_room.png")
end

------
-- 继承/覆盖
------

function PriRoomCreateLayer:onButtonClickedEvent( tag, sender )
    if BTN_HELP == tag then
        self._scene:popHelpLayer2(27, 1)
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
        if nil == self.m_nSelectMode then
            showToast(self, "未选择玩法配置!", 2)
            return
        end
        if nil == self.m_tabSelectConfig or table.nums(self.m_tabSelectConfig) == 0 then
            showToast(self, "未选择游戏局数!", 2)
            return
        end 
        if nil == self.m_nSelectPlayer then
            showToast(self, "未选择游戏人数!", 2)
            return
        end       
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
    end
end

function PriRoomCreateLayer:onModeSelectedEvent( tag, sender )
    if self.m_nSelectMode == tag then
        sender:setSelected(true)
        return
    end
    self.m_nSelectMode = tag
    for k,v in pairs(self.m_tabModeCheckBox) do
        if k ~= tag then
            v:setSelected(false)
        end
    end
end

function PriRoomCreateLayer:onPlayerSelectEvent( tag, sender )
    if self.m_nSelectPlayer == tag then
        sender:setSelected(true)
        return
    end
    self.m_nSelectPlayer = tag
    for k,v in pairs(self.m_tabPlayerCheckBox) do
        if k ~= tag then
            v:setSelected(false)
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
        local frame = nil
        if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("27_pri_sp_card_tips_bean.png")   
        else
            frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("27_pri_sp_card_tips.png")   
        end
        if nil ~= frame then
            self.m_spTips:setSpriteFrame(frame)
        end
    end
end

return PriRoomCreateLayer