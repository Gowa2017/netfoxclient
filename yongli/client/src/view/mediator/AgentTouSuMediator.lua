local BaseMediator = import(".BaseMediator")
local ExternalFun = appdf.req(appdf.EXTERNAL_SRC .. "ExternalFun")
local AgentTouSuMediator = class("AgentTouSuMediator", BaseMediator)
local logincmd = appdf.req(appdf.HEADER_SRC .. "CMD_LogonServer")
local MultiPlatform = appdf.req(appdf.EXTERNAL_SRC .. "MultiPlatform")

function AgentTouSuMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --该mediator关心的消息
    return	{
			}
end

function AgentTouSuMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)
    local msgName = notification:getName()
    local msgData = notification:getBody()
    local msgType = notification:getType()
	
    --deal something
end

function AgentTouSuMediator:onRegister()
    self.super.onRegister(self)
	
	--注册UI监听事件
	self.viewComponent:attach("V2M_TouShuAgent", handler(self, self.onSendTouShuReq))
end

--发送投诉代理请求
function AgentTouSuMediator:onSendTouShuReq(e, dwUserid, szUserWx, szAgentWx, szReason, szImageName)
	--if cc.FileUtils:getInstance():isFileExist(szImageName) then
		--发送上传投诉截图
		--local truncat_imageName = appdf.stripextension(appdf.strippath(szImageName))
		
		--填充10位随机码
		local seed = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}
		local tb = {}
		for i = 1, 10 do
			table.insert(tb, seed[math.random(1, 10)])
		end
		local rename_imageName = ""--table.concat(tb)

		local ClientData = ExternalFun.create_netdata(logincmd.CMD_MB_TouSuDaili)
		ClientData:setcmdinfo(yl.MDM_MB_LOGON,yl.SUB_MB_TOUSU_AGENT)
		
		ClientData:pushdword(dwUserid)
		ClientData:pushstring(szUserWx, yl.LEN_AGENT_ID)
		ClientData:pushstring(szAgentWx,yl.LEN_AGENT_ID)
		ClientData:pushstring(szReason, yl.LEN_DESC)
		ClientData:pushstring(rename_imageName, yl.LEN_IMAGE_NAME)
		
		AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"正在提交信息，请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)
		appdf.onSendData(yl.LOGONSERVER, yl.LOGONPORT, ClientData, function (Datatable, Responce)
			AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
			if (Responce.code == 0) then
				if Datatable.sub == logincmd.SUB_GP_OPERATE_SUCCESS then
					self:onSubOperateSuccess(Datatable.data)
--[[					local url = BaseConfig.WEB_HTTP_URL .. "/WS/Account.ashx?action=UploadTousuImg&filename=" .. rename_imageName
					local uploader = CurlAsset:createUploader(url,szImageName)
					if nil == uploader then
						self.viewComponent:emit("M2V_ShowToast", "图片上传异常，请稍后再试！")
						return
					end
					
					local nres = uploader:addToFileForm("file", szImageName, "image/png")
					
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.PUSH_VIEW, {ctor = {"正在上传图片，请稍候！"}, canrepeat = false}, VIEW_LIST.POPWAIT_LAYER)
					
					uploader:uploadFile(function (sender, ncode, msg)
						AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.POPWAIT_LAYER})
						if (ncode ~= 0) then
							self.viewComponent:emit("M2V_ShowToast", "图片上传异常，请稍后再试！")
						end
					end)--]]
					AppFacade:getInstance():sendNotification(GAME_COMMAMD.POP_VIEW, {Name = VIEW_LIST.SHOP_TOUSU_DAILI})
				elseif Datatable.sub == logincmd.SUB_GP_OPERATE_FAILURE then
					self:onSubOperateFailure(Datatable.data)
				end
			else
				self.viewComponent:emit("M2V_ShowToast", "网络连接失败！")
			end
		end)
	--else
	--	self.viewComponent:emit("M2V_ShowToast", "图像打开失败！")
	--end
end

function AgentTouSuMediator:onSubOperateSuccess(pData)
	local lResultCode = pData:readint()
	local szDescribe = pData:readstring()
	self.viewComponent:emit("M2V_ShowToast", szDescribe)
end

function AgentTouSuMediator:onSubOperateFailure(pData)
	local lResultCode = pData:readint()
	local szDescribe = pData:readstring()
	self.viewComponent:emit("M2V_ShowToast", szDescribe)
end

return AgentTouSuMediator