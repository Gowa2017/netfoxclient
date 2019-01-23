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
    self.m_atlasRoomID = image_bg:getChildByName("num_roomID")
    self.m_atlasRoomID:setString("000000")

    -- 局数
    self.m_atlasCount = image_bg:getChildByName("num_count")
    self.m_atlasCount:setString("0 / 0")

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
                --GlobalUserItem.bAutoConnect = true
            end
            local shareTxt = "斗地主约战 房间ID:" .. self.m_atlasRoomID:getString() .. " 局数:" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
            local friendC = "斗地主房间ID:" .. self.m_atlasRoomID:getString() .. " 局数:" .. PriRoom:getInstance().m_tabPriData.dwDrawCountLimit
            local url = GlobalUserItem.szWXSpreaderURL or yl.HTTP_URL
            if bMyFriend then
                PriRoom:getInstance():getTagLayer(PriRoom.LAYTAG.LAYER_FRIENDLIST, function( frienddata )
                    local serverid = tonumber(PriRoom:getInstance().m_tabPriData.szServerID) or 0                    
                    PriRoom:getInstance():priInviteFriend(frienddata, GlobalUserItem.nCurGameKind, serverid, yl.INVALID_TABLE, friendC)
                end)
            elseif nil ~= target then
                MultiPlatform:getInstance():shareToTarget(target, sharecall, "斗地主约战", shareTxt .. " 斗地主游戏精彩刺激, 一起来玩吧! ", url, "")
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
    self.m_atlasCount:setString(strcount)

    self:onRefreshInviteBtn()
end

function PriGameLayer:onRefreshInviteBtn()
    if self._gameLayer.m_cbGameStatus ~= 0 then --空闲场景
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
        local cellbg = image_bg:getChildByName("im_score_cell_" .. i)
        if nil ~= cellbg then
            cellbg:setVisible(true)
            local cellsize = cellbg:getContentSize()            
            local useritem = self._gameLayer:getUserInfoByChairID(i - 1)
            dump(useritem, "useritem", 6)
            if nil ~= useritem then
                -- 头像
                local head = PopupInfoHead:createNormal(useritem, 50)
                head:setPosition(35, cellsize.height * 0.5)
                head:enableInfoPop(true)
                cellbg:addChild(head)

                -- 昵称
                local nick = ClipText:createClipText(cc.size(150, 30), useritem.szNickName, nil, 30)
                nick:setAnchorPoint(cc.p(0, 0.5))
                nick:setPosition(70, cellsize.height * 0.5)
                cellbg:addChild(nick)

                local score = scoreList[useritem.wChairID + 1] or 0
                -- 成绩
                local strscore = score .. ""
                if score > 0 then
                    strscore = "+" .. score
                end
                local cpscore = ClipText:createClipText(cc.size(150, 30), strscore, nil, 30)
                cpscore:setAnchorPoint(cc.p(0, 0.5))
                cpscore:setPosition(340, cellsize.height * 0.5)
                cellbg:addChild(cpscore)
                local sp_res = cellbg:getChildByName("sp_gameres")
                if score > 0 then
                    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("txt_win.png")
                elseif score < 0 then
                    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("txt_failed.png")
                else
                    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("blank.png")
                end
                if nil ~= frame then
                    sp_res:setVisible(true)
                    sp_res:setSpriteFrame(frame)
                else
                    sp_res:setVisible(false)
                end
            else
                cellbg:setVisible(false)
            end
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