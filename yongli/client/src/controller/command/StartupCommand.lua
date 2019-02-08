local StartupCommand = class("StartupCommand", puremvc.MacroCommand)

function StartupCommand:ctor()
    self.super.ctor(self)
end

function StartupCommand:initializeMacroCommand()
    self.super.initializeMacroCommand(self)

    local PrepModelCommand = require("client.src.controller.command.PrepModelCommand")
    local PrepControllerCommand = require("client.src.controller.command.PrepControllerCommand")
    local PrepViewCommand = require("client.src.controller.command.PrepViewCommand")

    self:addSubCommand(PrepModelCommand)
    self:addSubCommand(PrepControllerCommand)
    self:addSubCommand(PrepViewCommand)
end

function StartupCommand:execute(note)
    self.super.execute(self, note)

    --@parm1 消息命令
    --@parm2 合并到 context.data 中，作为状态被传递到 Mediator 使用，使用 context.data 在场景之间传递信息非常方便
    --@parm3 消息命令附带信息 ? 这个信息是发到哪里去了？
    self:sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.WELLCOME_SCENE)
    --self:sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.WELLCOME_SCENE)
end

return StartupCommand
