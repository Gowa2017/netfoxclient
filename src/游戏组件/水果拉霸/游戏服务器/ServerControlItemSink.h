#pragma once
#include "ServerControl.h"
#define UR_GAME_CONTROL					0x20000000L

class CServerControlItemSink : public IServerControl
{
	//���Ʊ���
protected:
	static const CPoint				m_ptXian[ITEM_COUNT][ITEM_X_COUNT];			//�˿�����
public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//����������
	virtual bool __cdecl ServerControl(USERCONTROL &UserContorl, BYTE cbCardData[][ITEM_X_COUNT]);
	int GetCardType(USERCONTROL &UserContorl, BYTE cbCardData[ITEM_X_COUNT * ITEM_Y_COUNT]);

};
