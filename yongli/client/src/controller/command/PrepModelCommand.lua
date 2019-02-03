local PrepModelCommand = class("PrepModelCommand", puremvc.SimpleCommand)

function PrepModelCommand:ctor()
    self.super.ctor(self)
end

function PrepModelCommand:execute(note)
	self.super.execute(self)

	local AppConfigProxy = require("client.src.model.proxy.AppConfigProxy")
	local VersionProxy = require("client.src.model.proxy.VersionProxy")
	local MusicRecordProxy = require("client.src.model.proxy.MusicRecordProxy")
	local HotFixUpdateProxy = require("client.src.model.proxy.HotFixUpdateProxy")
	local ContextProxy = require("client.src.model.proxy.ContextProxy")
	local GameListAccessProxy = require("client.src.model.proxy.GameListAccessProxy")
	
    self.facade:registerProxy(AppConfigProxy.new())
    self.facade:registerProxy(VersionProxy.new())
	self.facade:registerProxy(MusicRecordProxy.new())
	self.facade:registerProxy(HotFixUpdateProxy.new())
	self.facade:registerProxy(ContextProxy.new())
	self.facade:registerProxy(GameListAccessProxy.new())
end

return PrepModelCommand
