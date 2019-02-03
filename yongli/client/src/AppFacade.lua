AppFacade = class("AppFacade", puremvc.Facade)

function AppFacade:ctor(key)
    self.super.ctor(self, key)
    --invote base method
end

function AppFacade:initializeController()
    --invote base method
    self.super.initializeController(self)
	self:initCommand()
end

--◊¢≤·”Œœ∑Command
function AppFacade:initCommand()
	local StartupCommand = require("client.src.controller.command.StartupCommand")
	local LoadViewCommand = require("client.src.controller.command.LoadViewCommand")
	local UnLoadViewCommand = require("client.src.controller.command.UnLoadViewCommand")
	
    self:registerCommand(GAME_COMMAMD.START_UP, StartupCommand)
    self:registerCommand(GAME_COMMAMD.LOAD_VIEW, LoadViewCommand)
	self:registerCommand(GAME_COMMAMD.UNLOAD_VIEW, UnLoadViewCommand)
end

function AppFacade:startup()
    self:sendNotification(GAME_COMMAMD.START_UP)
end

--Ω≈±æ÷ÿ∆Ù
function AppFacade.restartup(key)
	local key = key or "DefaultKey"
	AppFacade.removeCore(key)
    AppFacade:getInstance():sendNotification(GAME_COMMAMD.START_UP)
end

function AppFacade:getInstance(key)
	local key = key or "DefaultKey"
    local instance = self.instanceMap[key]
    if nil ~= instance then
        return instance
    end
    if nil == self.instanceMap[key] then
        self.instanceMap[key] = AppFacade.new(key)
    end
	
    return self.instanceMap[key]	
end