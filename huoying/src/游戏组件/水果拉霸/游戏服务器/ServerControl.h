#pragma once

//��Ϸ���ƻ���
class IServerControl
{
public:
	IServerControl(void){};
	virtual ~IServerControl(void){};

public:
	//����������
	virtual bool __cdecl ServerControl(USERCONTROL &UserContorl, BYTE cbCardBuffer[][ITEM_X_COUNT]) = NULL;
};
