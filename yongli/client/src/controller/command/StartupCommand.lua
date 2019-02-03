local StartupCommand = class("StartupCommand", puremvc.MacroCommand)

function StartupCommand:ctor()
    self.super.ctor(self)
end

function StartupCommand:initializeMacroCommand()
   self.super.initializeMacroCommand(self)

	local PrepModelCommand = require ("client.src.controller.command.PrepModelCommand")
	local PrepControllerCommand = require ("client.src.controller.command.PrepControllerCommand")
	local PrepViewCommand = require ("client.src.controller.command.PrepViewCommand")

	self:addSubCommand(PrepModelCommand)
	self:addSubCommand(PrepControllerCommand)
	self:addSubCommand(PrepViewCommand)
end

function StartupCommand:execute(note)
    self.super.execute(self, note)
	
    --@parm1 ��Ϣ����
    --@parm2 �ϲ��� context.data �У���Ϊ״̬�����ݵ� Mediator ʹ�ã�ʹ�� context.data �ڳ���֮�䴫����Ϣ�ǳ�����
    --@parm3 ��Ϣ�������Ϣ
    self:sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.WELLCOME_SCENE)
	--self:sendNotification(GAME_COMMAMD.PUSH_VIEW, {}, VIEW_LIST.WELLCOME_SCENE)
end

return StartupCommand
