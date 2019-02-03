local AppConfigMediator = class("AppConfigMediator", puremvc.Mediator)

function AppConfigMediator:ctor()
    self.super.ctor(self)
end

function AppConfigMediator:listNotificationInterests()
    self.super.listNotificationInterests(self)
    --这里返回该mediator关心的消息
    return {GAME.LOAD_APPCONFIG}
end

function AppConfigMediator:handleNotification(notification)
    self.super.handleNotification(self, notification)

    local n = notification:getName()
    local d = notification:getBody()
	local t = notification:getType()
	
	
end

function AppConfigMediator:onRegister()
    self.super.onRegister(self)
end

function AppConfigMediator:onRemove()
    self.super.onRemove(self)
end

return AppConfigMediator