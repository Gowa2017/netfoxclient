#pragma once
#include "ServerControl.h"
#define UR_GAME_CONTROL					0x20000000L

class CServerControlItemSink : public IServerControl
{
	//控制变量
protected:
	static const CPoint				m_ptXian[ITEM_COUNT][ITEM_X_COUNT];			//扑克数据
public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//服务器控制
	virtual bool __cdecl ServerControl(USERCONTROL &UserContorl, BYTE cbCardData[][ITEM_X_COUNT]);
	int GetCardType(USERCONTROL &UserContorl, BYTE cbCardData[ITEM_X_COUNT * ITEM_Y_COUNT]);

};
