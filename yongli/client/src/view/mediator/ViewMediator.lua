--[[
    游戏启动后，会注册一个 ViewMediator 作为视图的控制中枢，
    实际上它是一个工厂，
    根据视图的名字创建 Context 实例并把相应的 Mediator 与 ViewComponent 关联起来
--]]
local ViewMediator = class("ViewMediator", puremvc.Mediator)

local function guid()
	math.randomseed(os.time())
	local seed = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}
	local tb = {}
	
	--填充8位随机码
	for i = 1, 16 do
		table.insert(tb, seed[math.random(1, 16)])
	end	
	
	--填充8位时间戳
	local timestamp = string.format("%X", os.time())
	for i = 1, 8 do
		table.insert(tb, string.sub(timestamp, i, i))
	end

	--填充8位随机码
	for i = 1, 8 do
		table.insert(tb, seed[math.random(1, 16)])
	end
	
	local sid=table.concat(tb)
	return sid
end

function ViewMediator:ctor()
    self.super.ctor(self)
    self._initialized = false
end

function ViewMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --该mediator关心的消息
    return	{
				GAME_COMMAMD.PUSH_VIEW,
				GAME_COMMAMD.POP_VIEW,
				GAME_COMMAMD.PLAY_EFFECT,
				GAME_COMMAMD.PLAY_ONE_EFFECT,
			}
end

function ViewMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)

    local msgName = notification:getName()
    local msgData = notification:getBody()
	local msgType = notification:getType()

	--local contextProxy = AppFacade:getInstance():retrieveProxy("ContextProxy")
    --处理收到的消息
	printf("msgName:%s Line:%d Command:%s", msgName, debug.getinfo(1).currentline, msgName)
    if (msgName == GAME_COMMAMD.PUSH_VIEW) then

		--local context = Context.new() or contextProxy:findContextByName(msgType)
		
		-- load context nData
--[[		if (type(msgData) == "table") then
			for k, v in pairs(msgData) do
				context.data[k] = v
			end
		end--]]
		
		assert(msgType ~= nil, "PushView Name expected not nil")
		local mediatorClass = nil
		local viewClass = nil
		
		--创建上下文环境
		if (msgType == VIEW_LIST.WELLCOME_SCENE) then
			mediatorClass = nil
			viewClass = require("client.src.view.component.WelcomeScene")
		elseif (msgType == VIEW_LIST.LOGON_SCENE) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.LogonScene")
		elseif (msgType == VIEW_LIST.CLIENT_SCENE) then
			mediatorClass = require("client.src.view.mediator.ClientSceneMediator")
			viewClass = require("client.src.plaza.views.ClientScene")
		elseif (msgType == VIEW_LIST.ROOM_LIST_LAYER) then
			mediatorClass = require("client.src.view.mediator.RoomListMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.RoomListLayer")
		elseif (msgType == VIEW_LIST.GONGGAO_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.GongGaoLayer")
        elseif (msgType == VIEW_LIST.VIP_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.VIPLayer")
		elseif (msgType == VIEW_LIST.GONGGAO_IOS_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.GongGaoLayerIOS")
		elseif (msgType == VIEW_LIST.PERSON_LAYER) then
			mediatorClass = require("client.src.view.mediator.UserInfoMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.UserInfoLayer")
		elseif (msgType == VIEW_LIST.POPWAIT_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.app.views.layer.other.PopWait")
		elseif (msgType == VIEW_LIST.QUERY_DIALOG_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.app.views.layer.other.QueryDialog")
		elseif (msgType == VIEW_LIST.SELECT_SYSTEM_HEAD_LAYER) then
			mediatorClass = require("client.src.view.mediator.SelectSystemHeadMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.SelectSystemHeadLayer")
		elseif (msgType == VIEW_LIST.SHARE_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.PromoterInputLayer")
		elseif (msgType == VIEW_LIST.GAME_LAYER) then
			mediatorClass = nil
			assert(msgData.viewClassPath ~= nil, "viewClassPath is nil please check game path")
			viewClass = require(msgData.viewClassPath)
		elseif (msgType == VIEW_LIST.ACTIVITY_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.ActivityLayer")
		elseif (msgType == VIEW_LIST.ACTIVITY_IOS_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.ActivityLayerIOS")
		elseif (msgType == VIEW_LIST.MODIFY_ACCOUNT_PASS_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.ModifyAccountPasswdLayer")
		elseif (msgType == VIEW_LIST.MODIFY_BANK_PASS_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.ModifyBankPasswdLayer")
		elseif (msgType == VIEW_LIST.TASKLAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.TaskLayer")
		elseif (msgType == VIEW_LIST.BANK_LAYER) then
			mediatorClass = require("client.src.view.mediator.BankMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.BankLayer")
		elseif (msgType == VIEW_LIST.BANK_OPEN_LAYER) then
			mediatorClass = require("client.src.view.mediator.BankOpenMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.BankOpenLayer")
		elseif (msgType == VIEW_LIST.BANK_MODIFY_LAYER) then
			mediatorClass = require("client.src.view.mediator.BankModifyMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.BankModifyLayer")
		elseif (msgType == VIEW_LIST.GAME_WAIT_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.app.views.layer.other.PopGameWait")
		elseif (msgType == VIEW_LIST.SETTING_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.other.OptionLayer")
		elseif (msgType == VIEW_LIST.SHOP_LAYER) then
			mediatorClass = require("client.src.view.mediator.ShopMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.ShopLayer")
		elseif (msgType == VIEW_LIST.SHOP_APPSTORE_LAYER) then
			mediatorClass = require("client.src.view.mediator.ShopMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.ShopAppstoreLayer")
		elseif (msgType == VIEW_LIST.EARN_LAYER) then
			mediatorClass = require("client.src.view.mediator.ShopMediator")--require("client.src.view.mediator.EarnMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.EarnLayer")
        elseif (msgType == VIEW_LIST.EARN_MONEY) then
			mediatorClass = require("client.src.view.mediator.ShopMediator")--require("client.src.view.mediator.EarnMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.EarnMoney")
		elseif (msgType == VIEW_LIST.VISITOR_BIND_LAYER) then
			mediatorClass = nil --require("client.src.view.mediator.EarnMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.VisitorBindLayer")
		elseif (msgType == VIEW_LIST.AGENT_RECHARGE_LAYER) then
			mediatorClass = nil --require("client.src.view.mediator.EarnMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.AgentRechargeLayer")
		elseif (msgType == VIEW_LIST.REGISTER_AGREATMENT) then
			mediatorClass = nil --require("client.src.view.mediator.EarnMediator")
			viewClass = require("client.src.plaza.views.layer.logon.Agreatment")
		elseif (msgType == VIEW_LIST.ACCOUNT_REGISTER_LAYER) then
			mediatorClass = nil --require("client.src.view.mediator.EarnMediator")
			viewClass = require("client.src.plaza.views.layer.logon.AccountRegisterView")
		elseif (msgType == VIEW_LIST.SHOW_POP_TIPS) then
			mediatorClass = nil
			viewClass = require("client.src.app.views.layer.other.PopTips")
		elseif (msgType == VIEW_LIST.SHOP_SHENGQING_DAILI) then
			mediatorClass = nil --require("client.src.view.mediator.EarnMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.AgentShengQingLayer")
		elseif (msgType == VIEW_LIST.SHOP_SHENGQING_DAILI_COMFIRM) then
			mediatorClass = nil --require("client.src.view.mediator.EarnMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.AgentShengQingConfirmLayer")
		elseif (msgType == VIEW_LIST.SHOP_TOUSU_DAILI) then
			mediatorClass = require("client.src.view.mediator.AgentTouSuMediator")
			viewClass = require("client.src.plaza.views.layer.plaza.AgentTouSuLayer")
		elseif (msgType == VIEW_LIST.AGENT_AGREATMENT) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.AgentAgreatment")
		elseif (msgType == VIEW_LIST.GAME_RULE) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.GameRule")
		elseif (msgType ==  VIEW_LIST.RECHARGE_RIGHT_NOW) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.RechargeRightNow")
		elseif (msgType == VIEW_LIST.ROATEWAIT_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.view.component.PopRoateWait")
		elseif (msgType == VIEW_LIST.SELECT_LINK_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.SelectLinkLayer")
		elseif (msgType ==  VIEW_LIST.HALL_MESSAGE_LAYER) then
			mediatorClass = nil
			viewClass = require("client.src.plaza.views.layer.plaza.HallMessageLayer")
		else
			assert(false, "not support view type")
		end
		
		local genid = guid()
		
		self:sendNotification(GAME_COMMAMD.LOAD_VIEW, {id = genid, viewClass = viewClass, mediatorClass = mediatorClass, parm = msgData}, msgType)
		
		--self:sendNotification(GAME_COMMAMD.LOAD_VIEW, )
		
	elseif (msgName == GAME_COMMAMD.POP_VIEW) then
		if (msgData ~= nil) then
			-- load context nData
			self:sendNotification(GAME_COMMAMD.UNLOAD_VIEW, msgData)
		else
			self:sendNotification(GAME_COMMAMD.UNLOAD_VIEW)
		end
	elseif (msgName == GAME_COMMAMD.PLAY_ONE_EFFECT) then
		--只播放一次的 音效
		local musicPath = msgType
		assert(type(msgType) == "string")
		--获取音乐播放配置
		local musicProxy = AppFacade:getInstance():retrieveProxy("MusicRecordProxy")
		local refCount = musicProxy:addRef(musicPath)
		if (refCount == 1) then
			AudioEngine.playEffect(cc.FileUtils:getInstance():fullPathForFilename(musicPath),false)
		end
	elseif (msgName == GAME_COMMAMD.PLAY_EFFECT) then
		local musicPath = msgType
		assert(type(msgType) == "string")
		--获取音乐播放配置
		AudioEngine.playEffect(cc.FileUtils:getInstance():fullPathForFilename(musicPath),false)
	else
		assert(false, "Command not support now!")
    end
end

function ViewMediator:onRegister()
    self.super.onRegister(self)
end

function ViewMediator:onRemove()
    self.super.onRemove(self)
end

return ViewMediator