--
-- Author: Tang
-- Date: 2017-01-08 16:27:52
--
-- 红中麻将私人房创建界面
--local PriRoom = appdf.req("client.src.privatemode.plaza.src.models.PriRoom")
require("client.src.plaza.models.yl")
local CreateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.CreateLayerModel")

local PriRoomCreateLayer = class("PriRoomCreateLayer", CreateLayerModel)
-- local PriRoomCreateLayer = class("PriRoomCreateLayer", function(scene)
--     local layer = display.newLayer()
--     return layer
-- end)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local Shop = appdf.req(appdf.CLIENT_SRC.."plaza.views.layer.plaza.ShopLayer")

local RES_PATH = "client/src/privatemode/game/yule/sparrowhz/res/"

local BT_CREATE = 1
local BT_HELP = 2
local BT_PAY = 3

local CBX_MACOUNT_1 = 4
local CBX_MACOUNT_2 = 5
local CBX_MACOUNT_3 = 6
local CBX_MACOUNT_4 = 7
local CBX_MACOUNT_5 = 8
local CBX_MACOUNT_6 = 9

local CBX_INNINGS_1 = 10
local CBX_INNINGS_2 = 11
local CBX_INNINGS_3 = 12
local CBX_INNINGS_4 = 13
local CBX_INNINGS_5 = 14
local CBX_INNINGS_6 = 15

local CBX_USERNUM_2 = 16
local CBX_USERNUM_3 = 17
local CBX_USERNUM_4 = 18

local BT_MYROOM = 19

function PriRoomCreateLayer:onInitData()
    self.cbMaCount = 0
    self.cbInningsCount = 0
    self.cbUserNum = 0
end

function PriRoomCreateLayer:onResetData()
    self.cbMaCount = 0
    self.cbInningsCount = 0
    self.cbUserNum = 0
end

function PriRoomCreateLayer:ctor( scene )
    self:onInitData()
    PriRoomCreateLayer.super.ctor(self, scene)

    -- 加载csb资源
    cc.FileUtils:getInstance():addSearchPath(device.writablePath..RES_PATH)
    local rootLayer
    rootLayer, self.m_csbNode = ExternalFun.loadRootCSB(RES_PATH.."privateRoom/RoomCardLayer.csb", self)

    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 创建按钮
    self.m_csbNode:getChildByName("bt_createRoom")
        :setTag(BT_CREATE)
        :addTouchEventListener(btncallback)
    -- 帮助按钮
    self.m_csbNode:getChildByName("bt_help")
        :setTag(BT_HELP)
        :addTouchEventListener(btncallback)
    -- 充值按钮
    self.m_csbNode:getChildByName("bt_pay")
        :setTag(BT_PAY)
        :addTouchEventListener(btncallback)
    --我的房间
    self.m_csbNode:getChildByName("bt_myRoom")
        :setTag(BT_MYROOM)
        :addTouchEventListener(btncallback)   

    --复选按钮
    local cbtlistener = function (sender,eventType)
        self:onSelectedEvent(sender:getTag(), sender)
    end
    --扎码数量
    self.cbMaCount = 2
    self.m_csbNode:getChildByName("cbx_maCount_1")
        :setTag(CBX_MACOUNT_1)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_maCount_2")
        :setTag(CBX_MACOUNT_2)
        :setSelected(true)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_maCount_3")
        :setTag(CBX_MACOUNT_3)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_maCount_4")
        :setTag(CBX_MACOUNT_4)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_maCount_5")
        :setTag(CBX_MACOUNT_5)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_maCount_6")
        :setTag(CBX_MACOUNT_6)
        :addEventListener(cbtlistener)
    --房间限制
    local roomConfigList = PriRoom:getInstance().m_tabFeeConfigList
    self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[1]
    for i = 1, 6 do
        local textInnings = self.m_csbNode:getChildByName("Text_innings_"..i)
        local cbxInnings = self.m_csbNode:getChildByName("cbx_innings_"..i)
        if i <= #roomConfigList then
            self.cbInningsCount = self.m_tabSelectConfig.dwDrawCountLimit
            textInnings:setString(roomConfigList[i].dwDrawCountLimit)
            textInnings:setVisible(true)
            cbxInnings:setVisible(true)
        else
            textInnings:setVisible(false)
            cbxInnings:setVisible(false)
        end
    end
    self.m_csbNode:getChildByName("cbx_innings_1")
        :setTag(CBX_INNINGS_1)
        :setSelected(true)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_innings_2")
        :setTag(CBX_INNINGS_2)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_innings_3")
        :setTag(CBX_INNINGS_3)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_innings_4")
        :setTag(CBX_INNINGS_4)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_innings_5")
        :setTag(CBX_INNINGS_5)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_innings_6")
        :setTag(CBX_INNINGS_6)
        :addEventListener(cbtlistener)
    --房间人数
    self.cbUserNum = 2
    self.m_csbNode:getChildByName("cbx_userNum_2")
        :setTag(CBX_USERNUM_2)
        :setSelected(true)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_userNum_3")
        :setTag(CBX_USERNUM_3)
        :addEventListener(cbtlistener)
    self.m_csbNode:getChildByName("cbx_userNum_4")
        :setTag(CBX_USERNUM_4)
        :addEventListener(cbtlistener)
    --自己房卡数目
    self.textCardNum = self.m_csbNode:getChildByName("Text_myRoomCardNum"):setString(GlobalUserItem.lRoomCard .. "")

    --创建房卡花费花费
    self.m_bLow = false
    self.textCreateCost = self.m_csbNode:getChildByName("Text_costNum")
    self.textCreateCost:setString("")
    local feeType = "房卡"
    local strLockPrompt = "sp_lackRoomCard.png"
    local lMyTreasure = GlobalUserItem.lRoomCard
    if nil ~= self.m_tabSelectConfig then
        local dwCost = self.m_tabSelectConfig.lFeeScore
        if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
            feeType = "游戏豆"
            strLockPrompt = "sp_lackBean.png"
            lMyTreasure = GlobalUserItem.dUserBeans
        end
        if lMyTreasure < dwCost or lMyTreasure == 0 then
            self.m_bLow = true
        end
        self.textCreateCost:setString(dwCost..feeType)
    end

    --房卡或游戏豆不足提示
    self.sp_roomCardLack = self.m_csbNode:getChildByName("sp_roomCardLack")
    self.sp_roomCardLack:setVisible(self.m_bLow)
    self.sp_roomCardLack:setSpriteFrame(strLockPrompt)
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriRoomCreateLayer:onRefreshInfo()
    -- 房卡数更新
    self.textCardNum:setString(GlobalUserItem.lRoomCard .. "")
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
            buffer:pushbyte(self.cbUserNum)         --人数必须在第2个位置
            buffer:pushbyte(self.cbMaCount)
            print("人数", self.cbUserNum)
            print("码数", self.cbMaCount)
            for i = 1, 100 - 3 do
                buffer:pushbyte(0)
            end
            PriRoom:getInstance():getNetFrame():sendGameServerMsg(buffer)
            return true
        end        
    end
    return false
end

function PriRoomCreateLayer:getInviteShareMsg( roomDetailInfo )
    local shareTxt = "红中麻将约战 房间ID:" .. roomDetailInfo.szRoomID .. " 局数:" .. roomDetailInfo.dwPlayTurnCount
    local friendC = "红中麻将房间ID:" .. roomDetailInfo.szRoomID .. " 局数:" .. roomDetailInfo.dwPlayTurnCount
    return {title = "红中麻将约战", content = shareTxt .. " 红中麻将游戏精彩刺激, 一起来玩吧! ", friendContent = friendC}
end

------
-- 继承/覆盖
------

function PriRoomCreateLayer:onButtonClickedEvent(tag, sender)
    if BT_HELP == tag then
        print("帮助")
        self._scene:popHelpLayer(yl.HTTP_URL .. "/Mobile/Introduce.aspx?kindid=389&typeid=1")
    elseif BT_CREATE == tag then
        print("创建房间_tom")
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
        if nil == self.m_tabSelectConfig or
            table.nums(self.m_tabSelectConfig) == 0 or
            self.cbMaCount == 0 or
            self.cbUserNum == 0 then
            showToast(self, "未选择玩法配置!", 2)
            return
        end
        PriRoom:getInstance():showPopWait()
        PriRoom:getInstance():getNetFrame():onCreateRoom()
    elseif BT_PAY == tag then
        print("充值")
        local feeType = "房卡"
        if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
            feeType = "游戏豆"
        end
        if feeType == "游戏豆" then
            self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_BEAN)
        else
            self._scene:onChangeShowMode(yl.SCENE_SHOP, Shop.CBT_PROPERTY)
        end
    elseif BT_MYROOM == tag then
        print("我的房间")
        self._scene:onChangeShowMode(PriRoom.LAYTAG.LAYER_MYROOMRECORD)
    end
end

function PriRoomCreateLayer:onSelectedEvent(tag, sender)
    print("进", tag)
    if CBX_MACOUNT_1 <= tag and tag <= CBX_MACOUNT_6 then
        for i = CBX_MACOUNT_1, CBX_MACOUNT_6 do
            local checkBox = self.m_csbNode:getChildByTag(i)
            if i == tag then
                self.cbMaCount = checkBox:isSelected() and i - CBX_MACOUNT_1 + 1 or 0
            else
                checkBox:setSelected(false)
            end
        end
    elseif CBX_INNINGS_1 <= tag and tag <= CBX_INNINGS_6 then
        for i = CBX_INNINGS_1, CBX_INNINGS_6 do
            local checkBox = self.m_csbNode:getChildByTag(i)
            if i == tag then
                local feeType = "房卡"
                local lMyTreasure = GlobalUserItem.lRoomCard
                if PriRoom:getInstance().m_tabRoomOption.cbCardOrBean == 0 then
                    feeType = "游戏豆"
                    lMyTreasure = GlobalUserItem.dUserBeans
                end
                if checkBox:isSelected() then
                    local index = i - CBX_INNINGS_1 + 1
                    self.cbInningsCount = index
                    self.m_tabSelectConfig = PriRoom:getInstance().m_tabFeeConfigList[index]
                    self.textCreateCost:setString(self.m_tabSelectConfig.lFeeScore..feeType)
                    if lMyTreasure < self.m_tabSelectConfig.lFeeScore or lMyTreasure == 0 then --房卡或游戏豆不足
                        self.sp_roomCardLack:setVisible(true)
                        self.m_bLow = true
                    else
                        self.sp_roomCardLack:setVisible(false)
                        self.m_bLow = false
                    end
                else
                    self.cbInningsCount = 0
                    self.m_tabSelectConfig = nil
                    self.textCreateCost:setString("0"..feeType)
                    self.sp_roomCardLack:setVisible(lMyTreasure == 0)
                end
            else
                checkBox:setSelected(false)
            end
        end
    elseif CBX_USERNUM_2 <= tag and tag <= CBX_USERNUM_4 then
        for i = CBX_USERNUM_2, CBX_USERNUM_4 do
            local checkBox = self.m_csbNode:getChildByTag(i)
            if i == tag then
                self.cbUserNum = checkBox:isSelected() and i - CBX_USERNUM_2 + 2 or 0
            else
                checkBox:setSelected(false)
            end
        end
    else
        assert(false)
    end
    print(self.cbMaCount, self.cbInningsCount, self.cbUserNum)
end

return PriRoomCreateLayer