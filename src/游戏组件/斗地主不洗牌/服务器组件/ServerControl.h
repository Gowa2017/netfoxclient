#pragma once

//游戏控制基类
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//服务器控制
	virtual bool __cdecl ServerControl(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame) = NULL;
	virtual bool __cdecl ServerControl(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem) = NULL;
};
