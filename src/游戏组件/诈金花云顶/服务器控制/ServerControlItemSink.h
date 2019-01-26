#pragma once
#include "../��Ϸ������/ServerControl.h"
#include "GameLogic.h"

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����


class CServerControlItemSink : public IServerControl
{
public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

	CGameLogic						m_GameLogic;							//��Ϸ�߼�

public:
	//����������
	virtual bool __cdecl ServerControl(BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], ITableFrame * pITableFrame, WORD wPlayerCount);

	//��ȡ����Ϣ
	CString GetCradInfo( BYTE cbCardData );

	//���ؿ�������
	virtual bool __cdecl ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT], ROOMUSERCONTROL Keyroomusercontrol);
};
