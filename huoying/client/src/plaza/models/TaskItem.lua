--[[
	手游任务单项
	2016_06_12 Ravioyla
]]

local TaskItem = class("TaskItem")

function TaskItem:ctor(pData)

	--基本信息
	self.wTaskID			=0				--任务标识
	self.wTaskType			=0				--任务类型
	self.wTaskObject		=0				--任务目标
	self.cbPlayerType		=0				--玩家类型
	self.wKindID			=0				--类型标识
	self.dwTimeLimit		=0				--时间限制

	--奖励信息
	self.lStandardAwardGold	=0				--奖励金币
	self.lStandardAwardMedal=0				--奖励奖牌
	self.lMemberAwardGold	=0				--奖励金币
	self.lMemberAwardMedal	=0				--奖励奖牌

	--描述信息
	self.szTaskName			=""				--任务名称
	self.szTaskDescribe		=""				--任务描述

	--进度信息
	self.wTaskProgress		=0				--任务进度
	self.cbTaskStatus		=yl.TASK_STATUS_WAIT				--任务状态

end

function TaskItem:onInit(pData,len)
	if pData == nil then
		print("TaskItem-onInit-null")
		return
	end
    -- tagTaskParameter

    local int64 = Integer64.new()

    self.wTaskID 		= pData:readword()		--任务标识
    self.wTaskType 		= pData:readword()		--任务类型
    self.wTaskObject	= pData:readword()		--任务目标
    self.cbPlayerType	= pData:readbyte()		--玩家类型
    self.wKindID		= pData:readword()		--类型标识
    self.dwTimeLimit	= pData:readdword()		--时间限制

    self.lStandardAwardGold		= pData:readscore(int64):getvalue()		--奖励金币
    self.lStandardAwardMedal	= pData:readscore(int64):getvalue()		--奖励奖牌
    self.lMemberAwardGold		= pData:readscore(int64):getvalue()		--奖励金币
    self.lMemberAwardMedal		= pData:readscore(int64):getvalue()		--奖励奖牌

    self.szTaskName			= pData:readstring(64)			--任务名称
    --print("task len ==> " .. len)
    if len==0 then
    	print("describe length - "..len)
    	self.szTaskDescribe		= pData:readstring()	--任务描述
    else
    	self.szTaskDescribe		= pData:readstring((len-173)/2)	--任务描述
    end
	return self
end

function TaskItem:readScore(dataBuffer)
    if self._int64 == nil then
        self._int64 = Integer64.new():addTo(self)
    end
    dataBuffer:readscore(self._int64)
    return self._int64:getvalue()
end

return TaskItem