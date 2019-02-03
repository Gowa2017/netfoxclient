local PlayerProxy = class("PlayerProxy", puremvc.Proxy)

function PlayerProxy:ctor()
    self.super.ctor(self)
    self.proxyName = "PlayerProxy"
    self.config = nil
	self._score = 0
end

function PlayerProxy:setUserScore(var)
	assert(type(var) == "number", "expected number, got else")
	self._score = var
    self:sendNotification("UPDATE_USER_SCORE", {}, self._score);
end

function PlayerProxy:onRegister()
    self.super.onRegister(self)
    self:loadData()
end

function PlayerProxy:onRemove()
    self.super.onRemove(self)
end

return PlayerProxy