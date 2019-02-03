#pragma once
#include "../��Ϸ������/ServerControl.h"


class CServerControlItemSink : public IServerControl
{
	//���Ʊ���
protected:
	BYTE							m_cbWinSideControl;					//������Ӯ
	BYTE							m_cbExcuteTimes;					//ִ�д���
	int								m_nSendCardCount;					//���ʹ���

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
	virtual void __cdecl GetControlArea(BYTE Area[], BYTE cbControlTimes);

	//���ؿ�������
	virtual bool __cdecl ControlResult(BYTE	cbTableCardArray[], BYTE cbCardCount[]);

	//������Ϣ
	VOID ControlInfo(const void * pBuffer, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame,const tagGameServiceOption * pGameServiceOption);

	// ��¼����
	VOID WriteInfo( LPCTSTR pszFileName, LPCTSTR pszString );

};
