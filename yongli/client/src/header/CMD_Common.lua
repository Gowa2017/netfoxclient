local command_cmd = {}

--系统消息
command_cmd.CMD_CM_SystemMessage = 
{
	{t = "word",	k = "wType"},					--消息类型
	{t = "word",	k = "wLength"},					--消息长度
	{t = "tchar",	k = "szSign"},			--消息内容
}

return command_cmd