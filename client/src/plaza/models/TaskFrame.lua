--[[
	手游任务接口
	2016_06_12 Ravioyla
]]

local BaseFrame = appdf.req(appdf.CLIENT_SRC.."plaza.models.BaseFrame")
local TaskFrame = class("TaskFrame",BaseFrame)
local TaskItem  = appdf.req(appdf.CLIENT_SRC.."plaza.models.TaskItem")
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local game_cmd = appdf.req(appdf.HEADER_SRC .. "CMD_GameServer")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local NotifyMgr = appdf.req(appdf.EXTERNAL_SRC .. "NotifyMgr")

function TaskFrame:ctor(view,callbcak)
	TaskFrame.super.ctor(self,view,callbcak)

	self.m_tabTaskInfo = {}
	self.m_tabTaskInfo.tasklist = {}
end

function TaskFrame:getTaskInfo()
	return self.m_tabTaskInfo
end

function TaskFrame:getTastList()
	return self.m_tabTaskInfo.tasklist
end

function TaskFrame:updateTask(wTaskID, command)
	local newStatus = yl.TASK_STATUS_UNFINISH
	if command == yl.SUB_GP_TASK_TAKE then
		newStatus = yl.TASK_STATUS_UNFINISH
	elseif command == yl.SUB_GP_TASK_GIVEUP then
		newStatus = yl.TASK_STATUS_WAIT
	elseif command == yl.TASK_STATUS_SUCCESS then
		newStatus = yl.TASK_STATUS_REWARD
	end

    for k,v in pairs(self.m_tabTaskInfo.tasklist) do
        if v.wTaskID == self._wTaskID then
            v.cbTaskStatus = newStatus
            v.wTaskLevel = self:getLevel(newStatus)
            break
        end
    end
	
	--任务排序
	self:sortTaskList()
end

function TaskFrame:sortTaskList()
	--任务排序
	table.sort(self.m_tabTaskInfo.tasklist, function(a,b)
		if a.wTaskLevel ~= b.wTaskLevel then
			return a.wTaskLevel > b.wTaskLevel
		else
			return a.wTaskID < b.wTaskID
		end		
	end)
end

--连接结果
function TaskFrame:onConnectCompeleted()
	print("============Task onConnectCompeleted============")
	print("TaskFrame:onConnectCompeleted oprateCode="..self._oprateCode)
	print("============Task onConnectCompeleted============")

	if self._oprateCode == 0 then	--刷新
		self:sendTaskLoad()
	elseif self._oprateCode == 1 then	--放弃
		self:sendTaskGiveup()
	elseif self._oprateCode == 2 then   --领取
		self:sendTaskTake()
	elseif self._oprateCode == 3 then	--奖励
		self:sendTaskReward()
	else
		self:onCloseSocket()
		self._callBack(-1,"未知操作模式！")
	end
end

--网络信息(短连接)
function TaskFrame:onSocketEvent(main,sub,pData)
	print("============Task onSocketEvent============")
	print("socket event:"..main.."#"..sub)
	print("============Task onSocketEvent============")
	local bConnect = false
	if main == yl.MDM_GP_USER_SERVICE then 		--用户服务
		if sub == yl.SUB_GP_TASK_LIST then 					--全部任务
			self:onSubTaskList(pData, GlobalUserItem.bFilterTask)
		elseif sub == yl.SUB_GP_TASK_INFO then 				--任务信息
			self:onSubTaskInfo(pData)
		elseif sub == yl.SUB_GP_TASK_RESULT then 			--任务结果
			bConnect = self:onSubTaskResult(pData)
		elseif sub == yl.SUB_GP_TASK_GIVEUP_RESULT then 	--放弃结果
			
		else
			local message = string.format("未知命令码：%d-%d",main,sub)
			self._callBack(-1,message)
		end
	end
	if sub ~= yl.SUB_GP_TASK_LIST and not bConnect then
		self:onCloseSocket()
	end
end

--网络消息(长连接)
function TaskFrame:onGameSocketEvent(main,sub,pData)
	if main == game_cmd.MDM_GR_TASK then
		if sub == game_cmd.SUB_GR_TASK_INFO then 				--任务信息
			self:onSubTaskInfo(pData)
		elseif sub == game_cmd.SUB_GR_TASK_FINISH then 			--任务完成

		elseif sub == game_cmd.SUB_GR_TASK_LIST then 			--任务列表
			self:onSubTaskList(pData, true)
		elseif sub == game_cmd.SUB_GR_TASK_RESULT then 			--任务结果
			self:onSubTaskResult(pData)
		elseif sub == game_cmd.SUB_GR_TASK_GIVEUP_RESULT then 	--放弃结果

		end
	end
end

function TaskFrame:onSubTaskList(pData, filterKind)
	filterKind = filterKind or false 
	--计算任务数目
	local itemcount = pData:readword()
	print("itemcount - "..itemcount)
	self.m_tabTaskInfo = {}
	self.m_tabTaskInfo.wTaskCount = itemcount
	
	--记录任务信息
	local tasklist = {}
	--读取任务信息
	for i = 1,itemcount do
		local len = pData:readword()
		local item = nil
		if i < itemcount then
			item = TaskItem:create():onInit(pData,len)
		else
			item = TaskItem:create():onInit(pData,0)
		end

		if not item then 
			break
		end
		
		item.wTaskLevel = self:getLevel(item.cbTaskStatus)
		if filterKind then
			if item.wKindID == GlobalUserItem.nCurGameKind then
				table.insert(tasklist, item)
			end
		else
			table.insert(tasklist, item)
		end		
	end
	self.m_tabTaskInfo.tasklist = tasklist
	--任务排序
	self:sortTaskList()
	
	if nil ~= self._callBack then
		self._callBack(-1)
	end	
end

function TaskFrame:onSubTaskInfo(pData)
	local len = pData:getlen()
	--任务数目
	local itemcount = pData:readword()

	local bNotify = false
	--进度赋值
	for i=1,itemcount do
		local wTaskID = pData:readword()
		local wTaskProgress = pData:readword()
		local cbTaskStatus = pData:readbyte()

		for k,v in pairs(self.m_tabTaskInfo.tasklist) do
			if v.wTaskID == wTaskID then
				v.wTaskProgress = wTaskProgress
				v.cbTaskStatus = cbTaskStatus
				v.wTaskLevel = self:getLevel(cbTaskStatus)
			end
		end

		if cbTaskStatus == yl.TASK_STATUS_SUCCESS then
			bNotify = true
		end
	end

	--任务排序
	self:sortTaskList()

	if nil ~= self._callBack then
		self._callBack(1)
	end	

	if bNotify then
		--通知处理 (长短连接统一用一个消息)
		NotifyMgr:getInstance():excute(yl.MDM_GP_USER_SERVICE, yl.SUB_GP_TASK_INFO, nil)
	end
end

function TaskFrame:onSubTaskResult(pData)
	--logincmd.CMD_GP_TaskResult
	local bSuccessed = pData:readbool()
	local wCommandID = pData:readword()
	local score = GlobalUserItem:readScore(pData)
	local ingot = GlobalUserItem:readScore(pData)
	--领取奖励更新
	if wCommandID == logincmd.SUB_GP_TASK_REWARD and true == bSuccessed then
		GlobalUserItem.lUserScore = score
		GlobalUserItem.lUserIngot = ingot

		--通知更新        
		local eventListener = cc.EventCustom:new(yl.RY_USERINFO_NOTIFY)
	    eventListener.obj = yl.RY_MSG_USERWEALTH
	    cc.Director:getInstance():getEventDispatcher():dispatchEvent(eventListener)
	end	
	local szTip = pData:readstring()

	local bRes = false
	if nil ~= self._callBack then
		if bSuccessed == true then
			bRes = self._callBack(2,szTip)
		elseif bSuccessed == false then
			self._callBack(3,szTip)
		end
	end	
	return bRes
end

function TaskFrame:onSubTaskGiveupResult(pData)
	
end

--加载任务
function TaskFrame:sendTaskLoad()
	local TaskLoad = CCmd_Data:create(70)
	TaskLoad:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_TASK_LOAD)
	local password = md5(GlobalUserItem.szPassword)
	TaskLoad:pushdword(GlobalUserItem.dwUserID)
	TaskLoad:pushstring(password,33)

	--发送失败
	if not self:sendSocketData(TaskLoad) and nil ~= self._callBack then
		self._callBack(-1,"发送开通失败！")
	end
end

--放弃任务
function TaskFrame:sendTaskGiveup()
	local TaskGiveup = CCmd_Data:create(138)
	TaskGiveup:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_TASK_GIVEUP)
	local password = md5(GlobalUserItem.szPassword)
	TaskGiveup:pushword(self._wTaskID)
	TaskGiveup:pushdword(GlobalUserItem.dwUserID)
	TaskGiveup:pushstring(password,33)
	TaskGiveup:pushstring(GlobalUserItem.szMachine,33)

	--发送失败
	if not self:sendSocketData(TaskGiveup) and nil ~= self._callBack then
		self._callBack(-1,"发送放弃失败！")
	end
end

--领取任务
function TaskFrame:sendTaskTake()
	local TaskTake = CCmd_Data:create(138)
	TaskTake:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_TASK_TAKE)
	local password = md5(GlobalUserItem.szPassword)
	TaskTake:pushword(self._wTaskID)
	TaskTake:pushdword(GlobalUserItem.dwUserID)
	TaskTake:pushstring(password,33)
	TaskTake:pushstring(GlobalUserItem.szMachine,33)

	--发送失败
	if not self:sendSocketData(TaskTake) and nil ~= self._callBack then
		self._callBack(-1,"发送领取失败！")
	end
end

--领取奖励
function TaskFrame:sendTaskReward()
	local TaskReward = CCmd_Data:create(138)
	TaskReward:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_TASK_REWARD)
	local password = md5(GlobalUserItem.szPassword)
	TaskReward:pushword(self._wTaskID)
	TaskReward:pushdword(GlobalUserItem.dwUserID)
	TaskReward:pushstring(password,33)
	TaskReward:pushstring(GlobalUserItem.szMachine,33)

	--发送失败
	if not self:sendSocketData(TaskReward) and nil ~= self._callBack then
		self._callBack(-1,"发送领取奖励失败！")
	end
end

--加载任务
function TaskFrame:onTaskLoad()
	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_LoadTaskInfo)
		buffer:setcmdinfo(game_cmd.MDM_GR_TASK,game_cmd.SUB_GR_TASK_LOAD_INFO)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushstring(md5(GlobalUserItem.szPassword), yl.LEN_PASSWORD)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送加载任务失败！")
		end
	else
		--操作记录
		self._oprateCode = 0
		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--放弃任务
function TaskFrame:onTaskGiveup(wTaskID)
	self._wTaskID = wTaskID
	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_TakeGiveUp)
		buffer:setcmdinfo(game_cmd.MDM_GR_TASK,game_cmd.SUB_GR_TASK_GIVEUP)
		buffer:pushword(wTaskID)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushstring(md5(GlobalUserItem.szPassword), yl.LEN_PASSWORD)
		buffer:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送放弃任务失败！")
		end
	else
		--操作记录
		self._oprateCode = 1		

		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--领取任务
function TaskFrame:onTaskTake(wTaskID)
	self._wTaskID = wTaskID
	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_TakeTask)
		buffer:setcmdinfo(game_cmd.MDM_GR_TASK,game_cmd.SUB_GR_TASK_TAKE)
		buffer:pushword(wTaskID)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushstring(md5(GlobalUserItem.szPassword), yl.LEN_PASSWORD)
		buffer:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送领取任务失败！")
		end
	else
		--操作记录
		self._oprateCode = 2

		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--领取奖励
function TaskFrame:onTaskReward(wTaskID)
	self._wTaskID = wTaskID
	if nil ~= self._gameFrame and self._gameFrame:isSocketServer() then
		local buffer = ExternalFun.create_netdata(game_cmd.CMD_GR_C_TaskReward)
		buffer:setcmdinfo(game_cmd.MDM_GR_TASK,game_cmd.SUB_GR_TASK_REWARD)
		buffer:pushword(wTaskID)
		buffer:pushdword(GlobalUserItem.dwUserID)
		buffer:pushstring(md5(GlobalUserItem.szPassword), yl.LEN_PASSWORD)
		buffer:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)
		if not self._gameFrame:sendSocketData(buffer) then
			self._callBack(-1,"发送领取奖励失败！")
		end
	else
		--操作记录
		self._oprateCode = 3

		if not self:onCreateSocket(yl.LOGONSERVER,yl.LOGONPORT) and nil ~= self._callBack then
			self._callBack(-1,"建立连接失败！")
		end
	end	
end

--获取优先级
function TaskFrame:getLevel(wStatus)
	--优先级赋值
	local wLevel = 1
	if wStatus == yl.TASK_STATUS_UNFINISH then
		wLevel = 3
	elseif wStatus == yl.TASK_STATUS_REWARD then
		wLevel = 0
	elseif wStatus == yl.TASK_STATUS_FAILED then
		wLevel = 2
	elseif wStatus == yl.TASK_STATUS_SUCCESS then
		wLevel = 1
	elseif wStatus == yl.TASK_STATUS_WAIT then
		wLevel = 4
	end

	return wLevel
end

return TaskFrame