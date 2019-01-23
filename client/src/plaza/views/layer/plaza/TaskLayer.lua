--[[
	手游任务界面
	2016_06_12 Ravioyla
]]

local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local TaskLayer = class("TaskLayer", function(scene)
		local taskLayer = display.newLayer(cc.c4b(0, 0, 0, 125))
    return taskLayer
end)

local TaskFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.TaskFrame")
local ClipText = appdf.req(appdf.EXTERNAL_SRC .. "ClipText")

TaskLayer.BT_EXIT			= 3
TaskLayer.BT_CELL			= 15

-- 进入场景而且过渡动画结束时候触发。
function TaskLayer:onEnterTransitionFinish()
	self._scene:showPopWait()
	self._taskFrame:onTaskLoad()
	
    return self
end

-- 退出场景而且开始过渡动画时候触发。
function TaskLayer:onExitTransitionStart()
    return self
end

function TaskLayer:ctor(scene, gameFrame)	
	local this = self

	self._scene = scene
	
	self:registerScriptHandler(function(eventType)
		if eventType == "enterTransitionFinish" then	-- 进入场景而且过渡动画结束时候触发。
			self:onEnterTransitionFinish()
		elseif eventType == "exitTransitionStart" then	-- 退出场景而且开始过渡动画时候触发。
			self:onExitTransitionStart()
        elseif eventType == "exit" then
            if self._taskFrame:isSocketServer() then
                self._taskFrame:onCloseSocket()
            end

            if nil ~= self._taskFrame._gameFrame then
                self._taskFrame._gameFrame._shotFrame = nil
                self._taskFrame._gameFrame = nil
            end
		end
	end)

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
	self._taskFrame = TaskFrame:create(self,taskCallBack)
    self._taskFrame._gameFrame = gameFrame
    if nil ~= gameFrame then
        gameFrame._shotFrame = self._taskFrame
    end

	self._wTaskID = 0
	self._wCommand = 0

	display.newSprite("Task/frame_task_top.png")
		:move(yl.WIDTH/2,yl.HEIGHT - 51)
		:addTo(self)
	--金币和元宝
	display.newSprite("Task/frame_task_3.png")
		:move(842,699)
		:addTo(self)
	display.newSprite("Task/icon_task_gold_0.png")
		:move(727,699)
		:addTo(self)
	display.newSprite("Task/frame_task_4.png")
		:move(1158,699)
		:addTo(self)
	display.newSprite("Task/icon_task_ingot_0.png")
		:move(1039,699)
		:addTo(self)

	self._txtGold = cc.LabelAtlas:_create(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"), "Task/num_task_0.png", 16, 22, string.byte("/")) 
    		:move(765,699)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(self)
    self._txtIngot = cc.LabelAtlas:_create(string.formatNumberThousands(GlobalUserItem.lUserIngot,true,"/"), "Task/num_task_0.png", 16, 22, string.byte("/")) 
    		:move(1087,699)
    		:setAnchorPoint(cc.p(0,0.5))
    		:addTo(self)

	ccui.Button:create("bt_return_0.png","bt_return_1.png")
		:move(75,yl.HEIGHT-51)
		:addTo(self)
		:addTouchEventListener(function(ref, type)
       		 	if type == ccui.TouchEventType.ended then
					this._scene:onKeyBack()
				end
			end)

    local frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_0.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,320)
        self:addChild(sp)
    end
    frame = cc.SpriteFrameCache:getInstance():getSpriteFrame("sp_public_frame_2.png")
    if nil ~= frame then
        local sp = cc.Sprite:createWithSpriteFrame(frame)
        sp:setPosition(yl.WIDTH/2,324)
        self:addChild(sp)
    end

	--游戏列表
	self._listView = cc.TableView:create(cc.size(1145, 505))
	self._listView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)    
	self._listView:setPosition(cc.p(95,70))
	self._listView:setDelegate()
	self._listView:addTo(self)
	self._listView:setVerticalFillOrder(cc.TABLEVIEW_FILL_TOPDOWN)
	self._listView:registerScriptHandler(self.cellSizeForTable, cc.TABLECELL_SIZE_FOR_INDEX)
	self._listView:registerScriptHandler(handler(self, self.tableCellAtIndex), cc.TABLECELL_SIZE_AT_INDEX)
	self._listView:registerScriptHandler(handler(self, self.numberOfCellsInTableView), cc.NUMBER_OF_CELLS_IN_TABLEVIEW)

end

--操作任务
function TaskLayer:onHandleTask(wTaskID,cbTaskStatus)
	self._scene:showPopWait()
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
	if tag == TaskLayer.BT_CELL then
		local idx = sender:getParent():getIdx()
		local item = self._taskFrame:getTastList()[idx+1]
        if nil ~= item then
            self._wTaskID = item.wTaskID
            self:onHandleTask(item.wTaskID,item.cbTaskStatus)
        end		
	end
end

--操作结果
function TaskLayer:onTaskCallBack(result,message)
	print("======== TaskLayer:onTaskCallBack ========")
    local bRes = false
	self._scene:dismissPopWait()
	if  message ~= nil and message ~= "" then
		showToast(self,message,2)
	end

    if result == 1 then
        self._listView:reloadData()
	elseif result == 2 then
        print(" taskid " .. self._wTaskID .. " command " .. self._wCommand)
        self._taskFrame:updateTask(self._wTaskID, self._wCommand)
		self._listView:reloadData()

        self._txtGold:setString(string.formatNumberThousands(GlobalUserItem.lUserScore,true,"/"))
        self._txtIngot:setString(string.formatNumberThousands(GlobalUserItem.lUserIngot,true,"/"))

        --bRes = true
	end
    return bRes
end

---------------------------------------------------------------------

--子视图大小
function TaskLayer.cellSizeForTable(view, idx)
  	return 1145,505/4.0
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
        elseif item.cbTaskStatus == yl.TASK_STATUS_FAILED then
            btn:loadTextureNormal("Task/bt_task_fail_0.png")
            btn:loadTexturePressed("Task/bt_task_fail_1.png")
            btn:loadTextureDisabled("Task/bt_task_fail_0.png")
            btn:setEnabled(false)        
        elseif item.cbTaskStatus == yl.TASK_STATUS_SUCCESS then            
            btn:loadTextureNormal("Task/bt_task_reward_0.png")
            btn:loadTexturePressed("Task/bt_task_reward_1.png")
            btn:loadTextureDisabled("Task/bt_task_reward_0.png")
        else
            btn:setVisible(false)
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
    local cellwidth = 1145
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
    local cpName = ClipText:createClipText(cc.size(210,30), item.szTaskName, "fonts/round_body.ttf", 25)
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
    cc.Label:createWithTTF(""..item.wTaskProgress.."/"..item.wTaskObject, "fonts/round_body.ttf", 14)
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
        :move(550,80)
        :addTo(cell)
    cc.Label:createWithTTF("无时限", "fonts/round_body.ttf", 24)
            :setAnchorPoint(cc.p(0,0.5))
            :move(593,80)
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
        :move(550,35)
        :addTo(cell)

    display.newSprite("Task/icon_task_gold_1.png")
        :move(612,35)
        :addTo(cell)
    cc.LabelAtlas:_create(string.formatNumberThousands(item.lStandardAwardGold,true,"/"), "Task/num_task_1.png", 16, 20, string.byte("/")) 
        :move(635,35)
        :setAnchorPoint(cc.p(0,0.5))
        :addTo(cell)

    display.newSprite("Task/icon_task_ingot_1.png")
        :move(760,35)
        :addTo(cell)
    cc.LabelAtlas:_create(string.formatNumberThousands(item.lStandardAwardMedal,true,"/"), "Task/num_task_1.png", 16, 20, string.byte("/")) 
        :move(788,35)
        :setAnchorPoint(cc.p(0,0.5))
        :addTo(cell)

    --操作按钮
    ccui.Button:create("Task/bt_task_take_0.png","Task/bt_task_take_1.png","Task/bt_task_take_0.png")
        :move(1004,cy)
        :setTag(TaskLayer.BT_CELL)
        :addTo(cell)
        :addTouchEventListener(self._btcallback)
end
---------------------------------------------------------------------
return TaskLayer