local BaseMediator = import(".BaseMediator")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local RoomListMediator = class("RoomListMediator", BaseMediator)
local ClientUpdate = appdf.req(appdf.CLIENT_SRC.."app.controllers.ClientUpdate")

local versionProxy = AppFacade:getInstance():retrieveProxy("VersionProxy")
local appConfigProxy = AppFacade:getInstance():retrieveProxy("AppConfigProxy")	

function RoomListMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --该mediator关心的消息
    return	{
				GAME_COMMAMD.UPDATE_USER_SCORE,
				GAME_COMMAMD.HALL_GONGGAO_RESULT,			--订阅大厅公告内容（固定公告）
			}
end

function RoomListMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local msgName = notification:getName()
    local msgData = notification:getBody()
    local msgType = notification:getType()

	if (msgName == GAME_COMMAMD.UPDATE_USER_SCORE) then
		self.viewComponent:emit("M2V_Update_UserScore", GlobalUserItem.lUserScore)
	elseif (msgName == GAME_COMMAMD.HALL_GONGGAO_RESULT) then
		self.viewComponent:emit("M2V_Update_GongGao_Message", msgData, msgType)
	end
end

function RoomListMediator:onRegister()
    self.super.onRegister(self)
end

return RoomListMediator