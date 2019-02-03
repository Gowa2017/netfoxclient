#pragma once

//游戏控制基类
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//服务器控制
	virtual bool __cdecl ServerControl(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame * pITableFrame, WORD wPlayerCount) = NULL;

	//返回控制区域
	virtual bool __cdecl ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERCONTROL Keyroomusercontrol) = NULL;
};
