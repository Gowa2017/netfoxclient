local PrepControllerCommand = class("PrepControllerCommand", puremvc.SimpleCommand)

function PrepControllerCommand:ctor()
    self.super.ctor(self)
end

function PrepControllerCommand:execute(note)
    self.super.execute(self)
end

return PrepControllerCommand
