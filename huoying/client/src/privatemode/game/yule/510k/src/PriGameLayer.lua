--
-- Author: zhong
-- Date: 2016-12-29 11:13:57
--
-- 私人房游戏顶层
local PrivateLayerModel = appdf.req(PriRoom.MODULE.PLAZAMODULE .."models.PrivateLayerModel")
local PriGameLayer = class("PriGameLayer", PrivateLayerModel)
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local PopupInfoHead = appdf.req(appdf.EXTERNAL_SRC .. "PopupInfoHead")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

local BTN_DISMISS = 101
local BTN_INVITE = 102
local BTN_SHARE = 103
local BTN_QUIT = 104
local BTN_ZANLI = 105
function PriGameLayer:ctor( gameLayer )
    PriGameLayer.super.ctor(self, gameLayer)
    -- 加载csb资源
    local rootLayer, csbNode = ExternalFun.loadRootCSB("game/PrivateGameLayer.csb", self )
    self.m_rootLayer = rootLayer

    --
    local image_bg = csbNode:getChildByName("Image_bg")

    -- 房间ID
    self.m_atlasRoomID = image_bg:getChildByName("room_id")
    self.m_atlasRoomID:setString("000000")

    -- 局数
    self.m_curRoundNum = image_bg:getChildByName("curRoundNum")
    self.m_curRoundNum:setString("000")

    self.m_img_cur_round = image_bg:getChildByName("img_cur_round")
    local img_round_Size = self.m_img_cur_round:getContentSize()
    local img_round_posX = self.m_curRoundNum:getPositionX() + self.m_curRoundNum:getContentSize().width  + img_round_Size.width / 2 + 5 
    self.m_img_cur_round:setPositionX(img_round_posX)

    self.img_sprit = image_bg:getChildByName("img_sprit")
    self.img_sprit:setPositionX(self.m_img_cur_round:getPositionX() + img_round_Size.width / 2 + 6)

    -- 局数
    self.m_totalRoundNum = image_bg:getChildByName("totalRoundNum")
    self.m_totalRoundNum:setString("000")
    self.m_totalRoundNum:setPositionX(self.img_sprit:getPositionX() + self.img_sprit:getContentSize().width / 2 + self.m_totalRoundNum:getContentSize().width / 2 + 6  )

    self.m_img_total_round = image_bg:getChildByName("img_total_round")
    img_round_Size = self.m_img_total_round:getContentSize()
    img_round_posX = self.m_totalRoundNum:getPositionX() + self.m_totalRoundNum:getContentSize().width  + img_round_Size.width / 2 + 5 
    self.m_img_total_round:setPositionX(img_round_posX)


    self.m_img_multiple = image_bg:getChildByName("img_multiple")
    self.m_cellNum = image_bg:getChildByName("cellNum")
    self.m_cellNum:setString(string.format("%d", self._gameLayer.m_lCellScore))
    local img_multiple_Size = self.m_img_multiple:getContentSize()
    local img_multiple_posX = self.m_cellNum:getPositionX() + self.m_cellNum:getContentSize().width  + img_multiple_Size.width / 2 + 5 
    self.m_img_multiple:setPositionX(img_multiple_posX)
    
    self.img_rule = image_bg:getChildByName("img_rule")
    if self._gameLayer.m_b2Biggest then
        self.img_rule:loadTexture("game/large_2.png")
    else
        self.img_rule:loadTexture("game/large_A.png")
    end
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
    -- 解散按钮
    local btn = image_bg:getChildByName("bt_dismiss")
    btn:setTag(BTN_DISMISS)
    btn:addTouchEventListener(btncallback)

    -- 暂离按钮
    btn = image_bg:getChildByName("bt_zanli")
    btn:setTag(BTN_ZANLI)
    btn:addTouchEventListener(btncallback)

    -- 邀请按钮
    self.m_btnInvite = csbNode:getChildByName("bt_invite")
    self.m_btnInvite:setTag(BTN_INVITE)
    self.m_btnInvite:addTouchEventListener(btncallback)
end

function PriGameLayer:onButtonClickedEvent( tag, sender )
    if BTN_DISMISS == tag then              -- 请求解散游戏
        PriRoom:getInstance():queryDismissRoom()
    elseif BTN_INVITE == tag then
        --GlobalUserItem.bAutoConnect = false
        PriRoom:getInstance():getPlazaScene():popTargetShare(function(target, bMyFriend)
            bMyFriend = bMyFriend or false
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    showToast(self, "分享成功", 2)
                end
                GlobalUserItem.bAutoConnect = true
            end
            local shareTxt = "510k约战 房间ID:" .. self.m_atlasRoomID:getString() .. " 局数:" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
            local friendC = "510k房间ID:" .. self.m_atlasRoomID:getString() .. " 局数:" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
            if bMyFriend then
                PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_FRIENDLIST, function( frienddata )
                    local serverid = tonumber(PriRoom:getInstance().m_tabPriData.szServerID) or 0                    
                    PriRoom:getInstance():priInviteFriend(frienddata, GlobalUserItem.nCurGameKind, serverid, yl.INVALID_TABLE, friendC)
                end)
            elseif nil ~= target then
                GlobalUserItem.bAutoConnect = false
                MultiPlatform:getInstance():shareToTarget(target, sharecall, "510k约战", shareTxt .. " 510k游戏精彩刺激, 一起来玩吧! ", url, "")
            end
        end)
    elseif BTN_SHARE == tag then
        GlobalUserItem.bAutoConnect = false
        PriRoom:getInstance():getPlazaScene():popTargetShare(function(target, bMyFriend)
            bMyFriend = bMyFriend or false
            local function sharecall( isok )
                if type(isok) == "string" and isok == "true" then
                    showToast(self, "分享成功", 2)
                end
                GlobalUserItem.bAutoConnect = true
            end
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
            -- 截图分享
            local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()
            local area = cc.rect(0, 0, framesize.width, framesize.height)
            local imagename = "grade_share.jpg"
            if bMyFriend then
                imagename = "grade_share_" .. os.time() .. ".jpg"
            end
            ExternalFun.popupTouchFilter(0, false)
            captureScreenWithArea(area, imagename, function(ok, savepath)
                ExternalFun.dismissTouchFilter()
                if ok then
                    if bMyFriend then
                        PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_FRIENDLIST, function( frienddata )
                            PriRoom:getInstance():imageShareToFriend(frienddata, savepath, "分享我的约战房战绩")
                        end)
                    elseif nil ~= target then
                        GlobalUserItem.bAutoConnect = false
                        MultiPlatform:getInstance():shareToTarget(target, sharecall, "我的约战房战绩", "分享我的约战房战绩", url, savepath, "true")
                    end            
                end
            end)
        end)
    elseif BTN_QUIT == tag then
        GlobalUserItem.bWaitQuit = false
        self._gameLayer:onExitRoom()
    elseif BTN_ZANLI == tag then
        PriRoom:getInstance():getNetFrame()._gameFrame:setEnterAntiCheatRoom(false)                
        PriRoom:getInstance():getNetFrame()._gameFrame:onCloseSocket()
        self._gameLayer:onExitRoom()
    end
end

------
-- 继承/覆盖
------
-- 刷新界面
function PriGameLayer:onRefreshInfo()
    -- 房间ID
    self.m_atlasRoomID:setString(PriRoom:getInstance().m_tabPriData.szServerID or "000000")

    -- 局数
    local strcount = PriRoom:getInstance().m_tabPriData.dwPlayCount .. " / " .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
    strcount = PriRoom:getInstance().m_tabPriData.dwDrawCountLimit - PriRoom:getInstance().m_tabPriData.dwPlayCount
    --self.m_curRoundNum:setString(strcount)

    -- 局数
    self.m_curRoundNum:setString(""..PriRoom:getInstance().m_tabPriData.dwPlayCount)

    local img_round_Size = self.m_img_cur_round:getContentSize()
    local img_round_posX = self.m_curRoundNum:getPositionX() + self.m_curRoundNum:getContentSize().width  + img_round_Size.width / 2 + 5 
    self.m_img_cur_round:setPositionX(img_round_posX)

    self.img_sprit:setPositionX(self.m_img_cur_round:getPositionX() + img_round_Size.width / 2 + 6)

    -- 局数
    self.m_totalRoundNum:setString(""..PriRoom:getInstance().m_tabPriData.dwDrawCountLimit)
    self.m_totalRoundNum:setPositionX(self.img_sprit:getPositionX() + self.img_sprit:getContentSize().width / 2 + self.m_totalRoundNum:getContentSize().width / 2 - 4  )

    img_round_Size = self.m_img_total_round:getContentSize()
    img_round_posX = self.m_totalRoundNum:getPositionX() + self.m_totalRoundNum:getContentSize().width  + img_round_Size.width / 2 + 5 
    self.m_img_total_round:setPositionX(img_round_posX)

    self.m_cellNum:setString(string.format("%d", self._gameLayer.m_lCellScore))
    local img_multiple_Size = self.m_img_multiple:getContentSize()
    local img_multiple_posX = self.m_cellNum:getPositionX() + self.m_cellNum:getContentSize().width  + img_multiple_Size.width / 2 + 5 
    self.m_img_multiple:setPositionX(img_multiple_posX)
    print(···)
    if self._gameLayer.m_b2Biggest then
        self.img_rule:loadTexture("game/large_2.png")
    else
        self.img_rule:loadTexture("game/large_A.png")
    end
    self.m_cellNum:setString(string.format("%d", self._gameLayer.m_lCellScore))

    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    if self._gameLayer.m_cbGameStatus ~= 0 or PriRoom:getInstance().m_tabPriData.dwPlayCount > 0 then --空闲场景
        self.m_btnInvite:setVisible(false)
        return
    end
    -- 邀请按钮
    if nil ~= self._gameLayer.onGetSitUserNum then
        local chairCount = PriRoom:getInstance():getChairCount()
        if self._gameLayer:onGetSitUserNum() == chairCount then
            self.m_btnInvite:setVisible(false)
            return
        end
    end
    self.m_btnInvite:setVisible(true)
end

-- 私人房游戏结束
function PriGameLayer:onPriGameEnd( cmd_table )
    self:removeChildByName("private_end_layer")
    if true then
        --return
    end
    local csbNode = ExternalFun.loadCSB("game/PrivateGameEndLayer.csb", self.m_rootLayer)
    csbNode:setVisible(false)
    csbNode:setName("private_end_layer")
    local function btncallback(ref, tType)
        if tType == ccui.TouchEventType.ended then
            self:onButtonClickedEvent(ref:getTag(),ref)
        end
    end

    local image_bg = csbNode:getChildByName("Image_bg")

    local chairCount = PriRoom:getInstance():getChairCount()
    -- 玩家成绩
    local scoreList = cmd_table.lScore[1]
    for i = 1, chairCount do
        local useritem = self._gameLayer:getUserInfoByChairID(i - 1)
        local viewId = self._gameLayer:SwitchViewChairID(i - 1)
        dump(useritem, "useritem", 6)
        
         -- 昵称
        local nickName = image_bg:getChildByName("nickName_"..viewId)
        nickName:setVisible(false)
        local clipText = ClipText:createClipText(cc.size(280, 30), "xx", nil, 30)
        clipText:setAnchorPoint(cc.p(0.5, 0.5))
        clipText:setPosition(cc.p(nickName:getPositionX(), nickName:getPositionY()))
        image_bg:addChild(clipText)
        if nil ~= useritem then
            clipText:setString(useritem.szNickName)
        else
            clipText:setString(self._gameLayer._gameView.m_tabUserItemCopy[viewId].szNickName)
        end  
        local score = scoreList[i] or 0
            -- 成绩
        local strscore = score .. ""
        if score > 0 then
                strscore = "+" .. score
        end
            
        local totalScore = image_bg:getChildByName("totalScore_"..viewId)
        totalScore:setString(strscore)

        local img_result = image_bg:getChildByName("img_result_"..viewId)
        if score > 0 then
            frame = img_result:loadTexture("game/win.png") 
        elseif score < 0 then
            frame = img_result:loadTexture("game/lose.png")
        else
            frame = img_result:loadTexture("game/win.png")
        end  
    end

    -- 分享按钮
    local btn = image_bg:getChildByName("btn_share")
    btn:setTag(BTN_SHARE)
    btn:addTouchEventListener(btncallback)

    -- 退出按钮
    btn = image_bg:getChildByName("btn_quit")
    btn:setTag(BTN_QUIT)
    btn:addTouchEventListener(btncallback)

    csbNode:runAction(cc.Sequence:create(cc.DelayTime:create(3.0),
        cc.CallFunc:create(function()
            csbNode:setVisible(true)
        end)))
end

function PriGameLayer:onExit()
    cc.SpriteFrameCache:getInstance():removeSpriteFramesFromFile("game/land_game.plist")
    cc.Director:getInstance():getTextureCache():removeTextureForKey("game/land_game.png")
end

return PriGameLayer