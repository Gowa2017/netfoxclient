#pragma once

//��Ϸ���ƻ���
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//����������
	virtual bool __cdecl ServerControl(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame * pITableFrame, WORD wPlayerCount) = NULL;

	//���ؿ�������
	virtual bool __cdecl ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERCONTROL Keyroomusercontrol) = NULL;
};
