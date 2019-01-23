local GameModel = class("GameModel", function(frameEngine,scene)
        local gameLayer =  display.newLayer()
    return gameLayer
end)

--[[
    此类负责处理游戏服务器与客户端交互
    游戏数据保存放于此 于 onInitData 中初始化
    网络消息放于此
    计时器处理放于此
]]

-- local cmd = appdf.req(appdf.GAME_SRC.."gamemodel.gamename.src.models.CMD_Game")
-- local GameLogic = appdf.req(appdf.GAME_SRC.."gamemodel.gamename.src.models.GameLogic")
-- local GameViewLayer = appdf.req(appdf.GAME_SRC.."gamemodel.gamename.src.views.layer.GameViewLayer")

local QueryDialog = appdf.req("app.views.layer.other.QueryDialog")
local ExternalFun =  appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

-- 初始化界面
function GameModel:ctor(frameEngine,scene)
    ExternalFun.registerNodeEvent(self)
    self._scene = scene
	self._gameFrame = frameEngine

    --设置搜索路径
    self._gameKind = self:getGameKind()
    self._searchPath = ""
    self:setSearchPath()

    self._gameView = self:CreateView()
    self:OnInitGameEngine()
    self.m_bOnGame = false
    self.m_cbGameStatus = -1
    GlobalUserItem.bAutoConnect = true
end

function GameModel:setSearchPath()
    if nil == self._gameKind then
        return
    end

    local entergame = self._scene:getEnterGameInfo()
    if nil ~= entergame then
        local modulestr = string.gsub(entergame._KindName, "%.", "/")
        self._searchPath = device.writablePath.."game/" .. modulestr .. "/res/"
        cc.FileUtils:getInstance():addSearchPath(self._searchPath)
    end
end

function GameModel:reSetSearchPath()
    --重置搜索路径
    local oldPaths = cc.FileUtils:getInstance():getSearchPaths()
    local newPaths = {}
    for k,v in pairs(oldPaths) do
        if tostring(v) ~= tostring(self._searchPath) then
            table.insert(newPaths, v)
        end
    end
    cc.FileUtils:getInstance():setSearchPaths(newPaths)
end

-- 房卡信息层zorder
function GameModel:priGameLayerZorder()
    if nil ~= self._gameView and nil ~= self._gameView.priGameLayerZorder then
        return self._gameView:priGameLayerZorder()
    end
    return yl.MAX_INT
end

function GameModel:onExit()
    GlobalUserItem.bAutoConnect = true
    self:reSetSearchPath()
end

function GameModel:onEnterTransitionFinish()
end

--显示等待
function GameModel:showPopWait()
    if self._scene and self._scene.showPopWait then
        self._scene:showPopWait()
    end
end

--关闭等待
function GameModel:dismissPopWait()
    if self._scene and self._scene.dismissPopWait then
        self._scene:dismissPopWait()
    end
end

--初始化游戏数据
function GameModel:OnInitGameEngine()

    self._ClockFun = nil
    self._ClockID = yl.INVALID_ITEM
    self._ClockTime = 0
    self._ClockChair = yl.INVALID_CHAIR
    self._ClockViewChair = yl.INVALID_CHAIR

end

--重置框架
function GameModel:OnResetGameEngine()
    self:KillGameClock()
    self.m_bOnGame = false
end

--退出询问
function GameModel:onQueryExitGame()
    if PriRoom and true == GlobalUserItem.bPrivateRoom then
        PriRoom:getInstance():queryQuitGame(self.m_cbGameStatus)
    else
        if self._queryDialog then
           return
        end

        self._queryDialog = QueryDialog:create("您要退出游戏么？", function(ok)
            if ok == true then
                --退出防作弊
                self._gameFrame:setEnterAntiCheatRoom(false)
                
                self:onExitTable()
            end
            self._queryDialog = nil
        end):setCanTouchOutside(false)
            :addTo(self)
    end
end

function GameModel:standUpAndQuit()
    
end

-- 退出桌子
function GameModel:onExitTable()
    self:stopAllActions()
    self:KillGameClock()


    local MeItem = self:GetMeUserItem()
    if MeItem and MeItem.cbUserStatus > yl.US_FREE then
        local wait = self._gameFrame:StandUp(1)
        if wait then
            self:showPopWait()
            return
        end
    end
    self:dismissPopWait()
    self._scene:onKeyBack()
end

function GameModel:onExitRoom()
    self._gameFrame:onCloseSocket()
    self:stopAllActions()
    self:KillGameClock()
    self:dismissPopWait()
    self._scene:onChangeShowMode(yl.SCENE_ROOMLIST)
end

-- 返回键处理
function  GameModel:onKeyBack()
    self:onQueryExitGame()
    return true
end

-- 获取自己椅子
function GameModel:GetMeChairID()
    return self._gameFrame:GetChairID()
end

-- 获取自己桌子
function GameModel:GetMeTableID()
   return self._gameFrame:GetTableID()
end

-- 获取自己
function GameModel:GetMeUserItem()
    return self._gameFrame:GetMeUserItem()
end

-- 椅子号转视图位置,注意椅子号从0~nChairCount-1,返回的视图位置从1~nChairCount
function GameModel:SwitchViewChairID(chair)
    local viewid = yl.INVALID_CHAIR
    local nChairCount = self._gameFrame:GetChairCount()
    local nChairID = self:GetMeChairID()
    if chair ~= yl.INVALID_CHAIR and chair < nChairCount then
        viewid = math.mod(chair + math.floor(nChairCount * 3/2) - nChairID, nChairCount) + 1
    end
    return viewid
end

-- 是否合法视图id
function GameModel:IsValidViewID( viewId )
    local nChairCount = self._gameFrame:GetChairCount()
    return (viewId > 0) and (viewId <= nChairCount)
end

-- 设置计时器
function GameModel:SetGameClock(chair,id,time)
    if not self._ClockFun then
        local this = self
        self._ClockFun = cc.Director:getInstance():getScheduler():scheduleScriptFunc(function()
                this:OnClockUpdata()
            end, 1, false)
    end
    self._ClockChair = chair
    self._ClockID = id
    self._ClockTime = time
    self._ClockViewChair = self:SwitchViewChairID(chair)
    self:OnUpdataClockView()
end

function GameModel:GetClockViewID()
    return self._ClockViewChair
end

-- 关闭计时器
function GameModel:KillGameClock(notView)
    print("KillGameClock")
    self._ClockID = yl.INVALID_ITEM
    self._ClockTime = 0
    self._ClockChair = yl.INVALID_CHAIR
    self._ClockViewChair = yl.INVALID_CHAIR
    if self._ClockFun then
        --注销时钟
        cc.Director:getInstance():getScheduler():unscheduleScriptEntry(self._ClockFun) 
        self._ClockFun = nil
    end
    if not notView then
        self:OnUpdataClockView()
    end
end

--计时器更新
function GameModel:OnClockUpdata()
    if  self._ClockID ~= yl.INVALID_ITEM then
        self._ClockTime = self._ClockTime - 1
        local result = self:OnEventGameClockInfo(self._ClockChair,self._ClockTime,self._ClockID)
        if result == true   or self._ClockTime < 1 then
            self:KillGameClock()
        end
    end
    self:OnUpdataClockView()
end

--更新计时器显示
function GameModel:OnUpdataClockView()
    if self._gameView and self._gameView.OnUpdataClockView then
        self._gameView:OnUpdataClockView(self._ClockViewChair,self._ClockTime)
    end
end

--用户状态
function GameModel:onEventUserStatus(useritem,newstatus,oldstatus)
    if not self._gameView or not self._gameView.OnUpdateUser then
        return
    end
    local MyTable = self:GetMeTableID()
    local MyChair = self:GetMeChairID()

    if not MyTable or MyTable == yl.INVLAID_TABLE then
        return
    end

    --旧的清除
    if oldstatus.wTableID == MyTable then
        local viewid = self:SwitchViewChairID(oldstatus.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, nil, useritem.cbUserStatus == yl.US_FREE)
            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, true)
            end
        end
    end

    --更新新状态
    if newstatus.wTableID == MyTable then
        local viewid = self:SwitchViewChairID(newstatus.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)
            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, false)
            end
        end
    end

end

--用户积分
function GameModel:onEventUserScore(useritem)
    if not self._gameView or not self._gameView.OnUpdateUser then
        return
    end
    local MyTable = self:GetMeTableID()
    
    if not MyTable or MyTable == yl.INVLAID_TABLE then
        return
    end 

    if  MyTable == useritem.wTableID then
        local viewid = self:SwitchViewChairID(useritem.wChairID)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)
        end
    end 
end

--用户进入
function GameModel:onEventUserEnter(tableid,chairid,useritem)

    if not self._gameView or not self._gameView.OnUpdateUser then
        return
    end
    local MyTable = self:GetMeTableID()
        
    if not MyTable or MyTable == yl.INVLAID_TABLE then
        return
    end

    if MyTable == tableid then
        local viewid = self:SwitchViewChairID(chairid)
        if viewid and viewid ~= yl.INVALID_CHAIR then
            self._gameView:OnUpdateUser(viewid, useritem)

            if PriRoom then
                PriRoom:getInstance():onEventUserState(viewid, useritem, false)
            end
        end
    end
end

--发送准备
function GameModel:SendUserReady(dataBuffer)
    self._gameFrame:SendUserReady(dataBuffer)
end

--发送数据
function GameModel:SendData(sub,dataBuffer)
    if self._gameFrame then
        dataBuffer:setcmdinfo(yl.MDM_GF_GAME, sub)
        return self._gameFrame:sendSocketData(dataBuffer)   
    end

    return false
end

--是否观看
function GameModel:IsLookonMode()
    
end

--播放音效
function GameModel:PlaySound(path)
    if GlobalUserItem.bSoundAble == true then
        AudioEngine.playEffect(path)
    end
end

--获取gamekind
function GameModel:getGameKind()
    return nil
end

-- 创建场景
function GameModel:CreateView()
    -- body
end

-- 场景消息
function GameModel:onEventGameScene(cbGameStatus,dataBuffer)

end

-- 游戏消息
function GameModel:onEventGameMessage(sub,dataBuffer)
    -- body
end

-- 计时器响应
function GameModel:OnEventGameClockInfo(chair,time,clockid)
    -- body
end

-- 私人房解散响应
-- useritem 用户数据
-- cmd_table CMD_GR_RequestReply(回复数据包)
-- 返回是否自定义处理
function GameModel:onCancellApply(useritem, cmd_table)
    print("base onCancellApply")
    return false
end

-- 私人房解散结果
-- 返回是否自定义处理
function GameModel:onCancelResult( cmd_table )
    print("base onCancelResult")
    return false
end

-- 桌子坐下人数
function GameModel:onGetSitUserNum()
    print("base get sit user number")
    return 0
end

-- 根据chairid获取玩家信息
function GameModel:getUserInfoByChairID( chairid )
    
end

return GameModel