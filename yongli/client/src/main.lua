cc.FileUtils:getInstance():setPopupNotify(false)
cc.FileUtils:getInstance():addSearchPath(cc.FileUtils:getInstance():getWritablePath() .. "client/res/")
cc.FileUtils:getInstance():addSearchPath(cc.FileUtils:getInstance():getWritablePath() .. "client/src/")
cc.FileUtils:getInstance():addSearchPath("client/res/")
cc.FileUtils:getInstance():addSearchPath("client/src/")


--�Ƿ��������client����
--[[local debugURL = "192.192.168.18"
if (debugURL and debugURL ~= "") then
	local breakInfoFun, xpCallFun = require ("client.src.LuaDebugjit")(debugURL, 8172)
	cc.Director:getInstance():getScheduler():scheduleScriptFunc(breakInfoFun, 1, false)
end--]]

require "config"
require "cocos.init"
--��ʼ��һЩ��������ȫ�ֱ���
require "BaseConfig"
require "framework.puremvc.init"
require "init"
require "AppFacade"

cjson = require("cjson")
	
	
setBackGroundCallback(function (code)
	--code ����
	--enum AppStatus
	--{
	--	APP_NONE =			0,			//��ʼ״̬
	--	APP_FOCUS =			1,			//��������activity�ڸ���cocos2d activity
	--	APP_UNFOCUS =		2,		//cocos2d activity��ת��������activity(��δʵ��)
	--	APP_FOREGROUND =	3,		//����ǰ̨
	--	APP_BACKGROUND =	4,		//�����̨
	--};
	local targetPlatform = cc.Application:getInstance():getTargetPlatform()
	if not (cc.PLATFORM_OS_IPHONE == targetPlatform  or  cc.PLATFORM_OS_IPAD == targetPlatform or cc.PLATFORM_OS_MAC == targetPlatform) then
		local customEvent = cc.EventCustom:new(BaseConfig.RY_APPLICATION_NOTIFY)
		customEvent.obj = code
		cc.Director:getInstance():getEventDispatcher():dispatchEvent(customEvent)
    end
end)

local function main()
	AppFacade:getInstance():startup()
end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end
