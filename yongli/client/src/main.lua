cc.FileUtils:getInstance():setPopupNotify(false)
cc.FileUtils:getInstance():addSearchPath(cc.FileUtils:getInstance():getWritablePath() .. "client/res/")
cc.FileUtils:getInstance():addSearchPath(cc.FileUtils:getInstance():getWritablePath() .. "client/src/")
cc.FileUtils:getInstance():addSearchPath("client/res/")
cc.FileUtils:getInstance():addSearchPath("client/src/")


--是否允许调试client代码
--[[local debugURL = "192.192.168.18"
if (debugURL and debugURL ~= "") then
	local breakInfoFun, xpCallFun = require ("client.src.LuaDebugjit")(debugURL, 8172)
	cc.Director:getInstance():getScheduler():scheduleScriptFunc(breakInfoFun, 1, false)
end--]]

require "config"
require "cocos.init"
--初始化一些基础配置全局变量
require "BaseConfig"
require "framework.puremvc.init"
require "init"
require "AppFacade"

cjson = require("cjson")
	
	
setBackGroundCallback(function (code)
	--code 定义
	--enum AppStatus
	--{
	--	APP_NONE =			0,			//初始状态
	--	APP_FOCUS =			1,			//其他顶层activity在覆盖cocos2d activity
	--	APP_UNFOCUS =		2,		//cocos2d activity跳转到其他层activity(暂未实现)
	--	APP_FOREGROUND =	3,		//进入前台
	--	APP_BACKGROUND =	4,		//进入后台
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
