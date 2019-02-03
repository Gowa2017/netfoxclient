#ifndef GAME_PERSONAL_ROOM_SERVICE_MANAGER_HEAD_FILE
#define GAME_PERSONAL_ROOM_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "PersonalRoomServiceHead.h"

////////////////////////////////////////////////////////////////////////
																	
//˽�˷����������									
class PERSONAL_ROOM_SERVICE_CLASS CPersonalRoomServiceManager : public IPersonalRoomServiceManager
{
	//״̬����
protected:
	bool								m_bIsService;					//�����ʶ	
	
	//�ӿڱ���
protected:	
	IPersonalRoomItem *					m_pIPersonalRoomItem;				//˽�˷�����

	//����ӿ�
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//���ܽӿ�

	//��������
public:
	//���캯��
	CPersonalRoomServiceManager(void);
	//��������
	virtual ~CPersonalRoomServiceManager(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);
	
	//���ƽӿ�
public:
	//ֹͣ����
	virtual bool StopService();
	//��������
	virtual bool StartService();
	
	//����ӿ�

public:

	////��ʼ���ӿ�
	//virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter);	

	

	//�ӿ���Ϣ
public:
	//�û��ӿ�
	virtual IUnknownEx * GetServerUserItemSink();

	//����
public:
	virtual void TestPersonal();

	//��ѯ����
	virtual bool OnTCPNetworkSubMBQueryGameServer(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService);

	//������������
	virtual bool OnTCPNetworkSubMBSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService);

	//ǿ�ƽ�ɢ������������
	virtual bool OnTCPNetworkSubMBDissumeSearchServerTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, tagBindParameter *	pBindParameter,  ITCPSocketService * pITCPSocketService);

	//˽�˷�������
	virtual bool OnTCPNetworkSubMBPersonalParameter(VOID * pData, WORD wDataSize, DWORD dwSocketID, IDataBaseEngine *	 pIDataBaseEngine);

	//��ѯ˽�˷����б�
	virtual bool OnTCPNetworkSubMBQueryPersonalRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID,  ITCPSocketService * pITCPSocketService);

	///������󷿼�ɼ�
	virtual bool OnTCPNetworkSubQueryUserRoomScore(VOID * pData, WORD wDataSize, DWORD dwSocketID,  IDataBaseEngine *	 pIDataBaseEngine);

};

//////////////////////////////////////////////////////////////////////////
#endif