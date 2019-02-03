#pragma once
#include "../��Ϸ������/ServerControl.h"
#include "../��Ϸ������/GameLogic.h"
//��������
#define INDEX_PLAYER				0									//��
#define INDEX_BANKER				1									//��

class CServerControlItemSink : public IServerControl
{
	//���Ʊ���
protected:
	BYTE							m_cbWinSideControl;					//������Ӯ
	BYTE							m_cbExcuteTimes;					//ִ�д���
	int								m_nSendCardCount;					//���ʹ���
	BYTE							m_bWinArea[AREA_MAX];				//������Ӯ����
	int								m_nControlCount;					//������������


	//�˿���Ϣ
protected:
	BYTE							m_cbCardCount[2];						//�˿���Ŀ
	BYTE							m_cbTableCardArray[2][1];					//�����˿�
	CGameLogic						m_GameLogic;							//��Ϸ�߼�

public:
	CServerControlItemSink(void);
	virtual ~CServerControlItemSink(void);

public:
	//����������
	virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize,
		IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	//��Ҫ����
	virtual bool __cdecl NeedControl();
	//��ʼ����
	virtual void __cdecl GetControlArea(BYTE Area[], BYTE & cbControlTimes);

	//���ؿ�������
	virtual bool __cdecl ControlResult(BYTE	cbTableCardArray[], BYTE cbCardCount[]);

	//������Ϣ
	VOID ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	// ��¼����
	VOID WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );
	//�����˿�
	bool  DispatchCard();
	//�ƶ�Ӯ��
	void DeduceWinner(BYTE* pWinArea);
};
