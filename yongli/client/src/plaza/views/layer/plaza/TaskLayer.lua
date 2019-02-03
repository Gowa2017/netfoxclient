--[[
	手游任务界面
	2017_08_28
]]

--[[local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local TaskLayer = class("TaskLayer", function(scene)
		local taskLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return taskLayer
end)--]]
local TaskLayer = class("TaskLayer", cc.BaseLayer)

local TaskFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.TaskFrame")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")

TaskLayer.BT_EXIT			= 3
TaskLayer.BT_RETURN			= 4
TaskLayer.BT_CELL			= 15

-- 进入场景而且过渡动画结束时候触发。
function TaskLayer:onEnterTransitionFinish()
	--self._scene:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
	self._taskFrame:onTaskLoad()
	
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function TaskLayer:onExitTransitionStart()
    return self
end

function TaskLayer:onExit()
	if self._taskFrame:isSocketServer() then
		self._taskFrame:onCloseSocket()
	end

	if nil ~= self._taskFrame._gameFrame then
		self._taskFrame._gameFrame._shotFrame = nil
		self._taskFrame._gameFrame = nil
	end
end

function TaskLayer:ctor(gameFrame, curGameKind)	
	local this = self
	self.super.ctor(self)


	--按钮回调
	self._btcallback = function(ref, type)
        if type == ccui.TouchEventType.ended then
         	this:onButtonClickedEvent(ref:getTag(),ref)
        end
    end
	
	--网络回调
    local taskCallBack = function(result,message)
		return this:onTaskCallBack(result,message)
	end

	--网络处理
	self._taskFrame = TaskFrame:create(self,taskCallBack, curGameKind)
    self._taskFrame._gameFrame = gameFrame
    if nil ~= gameFrame then
        gameFrame._shotFrame = self._taskFrame
    end

	self._wTaskID = 0
	self._wCommand = 0

    --MXM任务
	--加载csb资源
	local csbPath = "Task/TaskLayer.csb"
	local rootLayer, csbNode = ExternalFun.loadRootCSB(csbPath, self)
	self._csbNode = csbNode
	
	-- 加载csb动画
	self._csbNodeAni = appdf.loadTimeLine(csbPath)
	self._csbNode:stopAllActions()
	self._csbNode:runAction(self._csbNodeAni)
	
	--返回按钮
	local background = csbNode:getChildByName("background")
	local info_top_bg = background:getChildByName("info_top_bg")
	self._btn_return = info_top_bg:getChildByName("btn_return")
	self._btn_return:setTag(TaskLayer.BT_RETURN)
	self._btn_return:addTouchEventListener(self._btcallback)

	--游戏列表
	self._listView = cc.TableView:create(cc.size(1299, 600))
	self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
	self._listView:setPosition(cc.p(18,30))
	self._listView:setDelegate()
	self._listView:addTo(csbNode:getChildByName("background"))
	self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
	self._listView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)

end

--操作任务
function TaskLayer:onHandleTask(wTaskID,cbTaskStatus)
	--self._scene:showPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)	
	if cbTaskStatus == yl.TASK_STATUS_WAIT then
		self._wCommand = yl.SUB_GP_TASK_TAKE
		self._taskFrame:onTaskTake(wTaskID)
	elseif cbTaskStatus == yl.TASK_STATUS_UNFINISH or cbTaskStatus == yl.TASK_STATUS_FAILED then
		self._wCommand = yl.SUB_GP_TASK_GIVEUP
		self._taskFrame:onTaskGiveup(wTaskID)
	elseif cbTaskStatus == yl.TASK_STATUS_SUCCESS then
		self._wCommand = yl.TASK_STATUS_SUCCESS
		self._taskFrame:onTaskReward(wTaskID)
	end
end

--按键监听
function TaskLayer:onButtonClickedEvent(tag,sender)
	ExternalFun.playClickEffect()
	if tag == TaskLayer.BT_CELL then
		local idx = sender:getParent():getIdx()
		local item = self._taskFrame:getTastList()[idx+1]
        if nil ~= item then
            self._wTaskID = item.wTaskID
            self:onHandleTask(item.wTaskID,item.cbTaskStatus)
        end	
	elseif tag == TaskLayer.BT_RETURN then
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.TASKLAYER})
	end
end

--操作结果
function TaskLayer:onTaskCallBack(result,message)
	print("======== TaskLayer:onTaskCallBack ========")
    local bRes = false
	--self._scene:dismissPopWait()
	AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
	if  message ~= nil and message ~= "" then
		showToast(self,message,2)
	end

    if result == 1 then
        self._listView:reloadData()
	elseif result == 2 then
        print(" taskid " .. self._wTaskID .. " command " .. self._wCommand)
        self._taskFrame:updateTask(self._wTaskID, self._wCommand)
		self._listView:reloadData()

        --self._txtGold:setString(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"))
       -- self._txtIngot:setString(string.formatNumberThousands(GlobalUserItem.lUserIngot,true,"/"))

        --bRes = true
	end
    return bRes
end

---------------------------------------------------------------------

--子视图大小
function TaskLayer.cellSizeForTable(view, idx)
  	return 1294,600/5.0
end

--子视图数目
function TaskLayer:numberOfCellsInTableView(view)
    return #self._taskFrame:getTastList()
end
	
--获取子视图
function TaskLayer:tableCellAtIndex(view, idx)	
	local cell = view:dequeueCell()
	local item = self._taskFrame:getTastList()[idx+1]

	if not cell then		
		cell = cc.TableViewCell:new()
    else
        cell:removeAllChildren()
	end
    if nil == item then
        return cell
    end
	
    self:createTaskItem(cell, item, view)
	cell:getChildByTag(3):setTexture("Task/icon_task_"..item.wTaskType..".png")

	if item.dwTimeLimit ~= 0 then
		local hour = math.floor(item.dwTimeLimit/3600)
		local minute = math.floor((item.dwTimeLimit-hour*3600)/60)
		local second = item.dwTimeLimit-hour*3600-minute*60
        local str = ""

        if 0 ~= hour then
            str = hour .. " 小时"
        end
        if 0 ~= minute then
            str = (str ~= "") and (str .. ":") or str
            str = str .. minute .. " 分钟"
        end
        if 0 ~= second then
            str = (str ~= "") and (str .. ":") or str
            str = str .. second .. " 秒"
        end
		--cell:getChildByTag(8):setString(""..hour.."："..minute.."："..second.."")
        cell:getChildByTag(8):setString(str)
	else
		cell:getChildByTag(8):setString("无时限")
	end
    --dump(item, "item", 7)

    --按钮处理
    local btn = cell:getChildByTag(TaskLayer.BT_CELL)
	if nil ~= btn then
        btn:setEnabled(true)
        btn:setVisible(true)
        if item.cbTaskStatus == yl.TASK_STATUS_UNFINISH then
            btn:loadTextureNormal("Task/bt_task_giveup_0.png")
            btn:loadTexturePressed("Task/bt_task_giveup_1.png")
            btn:loadTextureDisabled("Task/bt_task_giveup_0.png")
        elseif item.cbTaskStatus == yl.TASK_STATUS_WAIT then
            btn:loadTextureNormal("Task/bt_task_take_0.png")
            btn:loadTexturePressed("Task/bt_task_take_1.png")
            btn:loadTextureDisabled("Task/bt_task_take_0.png")
			item.wTaskProgress = 0 --MXM任务放弃后把任务进度置为空
        elseif item.cbTaskStatus == yl.TASK_STATUS_FAILED then
            btn:loadTextureNormal("Task/bt_task_fail_0.png")
            btn:loadTexturePressed("Task/bt_task_fail_1.png")
            btn:loadTextureDisabled("Task/bt_task_fail_0.png")
            btn:setEnabled(false)        
        elseif item.cbTaskStatus == yl.TASK_STATUS_SUCCESS then            
            btn:loadTextureNormal("Task/bt_task_reward_0.png")
            btn:loadTexturePressed("Task/bt_task_reward_1.png")
            btn:loadTextureDisabled("Task/bt_task_reward_0.png")
        else -- 已完成
            btn:loadTextureNormal("Task/bt_task_finished_0.png")
            btn:loadTexturePressed("Task/bt_task_finished_1.png")
            btn:loadTextureDisabled("Task/bt_task_finished_0.png")
			btn:setEnabled(false)
        end
    end    

	--进度条处理
	if  item.wTaskObject > 0 then
		local scalex = (item.wTaskProgress*1.0)/(item.wTaskObject*1.0)
		if scalex > 1 then
			scalex = 1
		end
		cell:getChildByTag(6):setTextureRect(cc.rect(0,0,218*scalex,20))
	else
		cell:getChildByTag(6):setTextureRect(cc.rect(0,0,1,20))
	end
	cell:getChildByTag(7):setString(""..item.wTaskProgress.."/"..item.wTaskObject)

	return cell
end

function TaskLayer:createTaskItem(cell, item, view)
     
    local cy = 53
    local cellwidth = 1294
	
	--local frame_task_item = self._csbNode:getChildByName("frame_task_item")
    display.newSprite("Task/frame_task_2.png")
        :addTo(cell)
        :move(cellwidth/2, cy)
        :setTag(1)

    display.newSprite("Task/frame_task_5.png")
        :move(90,cy)
        :setTag(2)
        :addTo(cell)
    --任务图标
    display.newSprite("Task/icon_task_0.png")
        :move(90,cy)
        :setTag(3)
        :addTo(cell)

    --任务名称
    display.newSprite("Task/text_task_task.png")
        :move(195,80)
        :setTag(4)
        :addTo(cell)
    local cpName = ClipText:createClipText(cc.size(280,30), item.szTaskName, "base/fonts/round_body.ttf", 25)
    cpName:setAnchorPoint(cc.p(0, 0.5))
    cpName:setPosition(230, 80)
    cpName:setTag(5)
    cell:addChild(cpName)

    --任务进度
    display.newSprite("Task/text_task_progress.png")
        :addTo(cell)
        :move(210-15,35)
    display.newSprite("Task/frame_task_progress_0.png")
        :addTo(cell)
        :move(360-15,33)
    --进度条
    display.newSprite("Task/frame_task_progress_1.png")
        :setTextureRect(cc.rect(0,0,1,20))
        :setAnchorPoint(cc.p(0,0.5))
        :move(250-15,33)
        :setTag(6)
        :addTo(cell)
    --进度文字
    cc.Label:createWithTTF(""..item.wTaskProgress.."/"..item.wTaskObject, "base/fonts/round_body.ttf", 14)
            :setAnchorPoint(cc.p(0.5,0.5))
            :move(360-15,33)
            :setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
            :setHorizontalAlignment(cc.TEXT_ALIGNMENT_CENTER)
            :setWidth(100)
            :setHeight(15)
            :setTag(7)
            :setLineBreakWithoutSpace(false)
            :setTextColor(cc.c4b(255,255,255,255))
            :addTo(cell)

    --时间限制
    display.newSprite("Task/text_task_time.png")
        :move(540,50)
        :addTo(cell)
    cc.Label:createWithTTF("无时限", "base/fonts/round_body.ttf", 24)
            :setAnchorPoint(cc.p(0,0.5))
            :move(578,50)
            :setVerticalAlignment(cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
            :setHorizontalAlignment(cc.TEXT_ALIGNMENT_LEFT)
            :setWidth(300)
            :setHeight(30)
            :setTag(8)
            :setLineBreakWithoutSpace(false)
            :setTextColor(cc.c4b(255,255,255,255))
            :addTo(cell)

    --任务奖励
    display.newSprite("Task/text_task_reward.png")
        :move(730,50)
        :addTo(cell)

    display.newSprite("Task/icon_task_gold_1.png")
        :move(792,50)
        :addTo(cell)
	
    cc.LabelAtlas:_create(string.formatNumberFhousands(item.lStandardAwardGold), "Task/num_task_1.png", 18, 25, string.byte("."))
        :move(815,50)
        :setAnchorPoint(cc.p(0,0.5))
        :addTo(cell)
	

    --操作按钮
    ccui.Button:create("Task/bt_task_take_0.png","Task/bt_task_take_1.png","Task/bt_task_take_0.png")
        :move(1150,cy)
        :setTag(TaskLayer.BT_CELL)
        :addTo(cell)
        :addTouchEventListener(self._btcallback)
end

function TaskLayer:onWillViewEnter()
	self._csbNodeAni:setAnimationEndCallFunc("openAni", function ()
		self:enterViewFinished()
		
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PLAY_ONE_EFFECT, {}, "sound/task.mp3")
	end)
	
	self._csbNodeAni:play("openAni", false)
end

function TaskLayer:onWillViewExit()
	self._csbNodeAni:setAnimationEndCallFunc("closeAni", function ()
		self:exitViewFinished()
	end)
	
	self._csbNodeAni:play("closeAni", false)
end

---------------------------------------------------------------------
return TaskLayer