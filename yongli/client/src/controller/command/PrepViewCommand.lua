local PrepViewCommand = class("PrepViewCommand", puremvc.SimpleCommand)

function PrepViewCommand:ctor()
    self.super.ctor(self)
end

function PrepViewCommand:execute(note)
	self.super.ctor(self)
	
	local ViewMediator = require("client.src.view.mediator.ViewMediator")
	
    self.facade:registerMediator(ViewMediator.new())
end

return PrepViewCommand
