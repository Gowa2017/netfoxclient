local BaseMediator = import(".BaseMediator")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local SelectSystemHeadMediator = class("SelectSystemHeadMediator", BaseMediator)
local loginCMD = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")

function SelectSystemHeadMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --该mediator关心的消息
    return	{
			}
end

function SelectSystemHeadMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local msgName = notification:getName()
    local msgData = notification:getBody()
    local msgType = notification:getType()
	
    --deal something
end

function SelectSystemHeadMediator:onRegister()
    self.super.onRegister(self)
	
	--注册UI监听事件
	self.viewComponent:attach("V2M_ChangeSystemFaceOK", handler(self, self.onModifySystemFace))

	--self:sendNotification(GAME_COMMAMD.POP_VIEW)
end

--发送修改头像
function SelectSystemHeadMediator:onModifySystemFace(e, faceindex, callback)
	local sysmodify = ExternalFun.create_netdata(loginCMD.CMD_GP_SystemFaceInfo)
	sysmodify:setcmdinfo(yl.MDM_GP_USER_SERVICE,yl.SUB_GP_SYSTEM_FACE_INFO)
	sysmodify:pushword(faceindex + 1)
	sysmodify:pushdword(GlobalUserItem.dwUserID)
	sysmodify:pushstring(md5(GlobalUserItem.szPassword),yl.LEN_PASSWORD)
	sysmodify:pushstring(GlobalUserItem.szMachine,yl.LEN_MACHINE_ID)

	appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, sysmodify, function (Datatable, Responce)
		if (Responce.code == 0) then
			if (Datatable.sub == yl.SUB_GP_USER_FACE_INFO) then
				local wFaceId = Datatable.data:readword()
				local dwCustomId = Datatable.data:readdword()
				--更新数据
				GlobalUserItem.wFaceID = wFaceId
				GlobalUserItem.dwCustomID = dwCustomId
				callback(0, "头像修改成功")
			else
				callback(-1, "修改头像失败")
			end
		else
			callback(-1, "修改头像失败")
		end
	end)
end

--绑定手机
function SelectSystemHeadMediator:onBindPhone(e)

end

return SelectSystemHeadMediator