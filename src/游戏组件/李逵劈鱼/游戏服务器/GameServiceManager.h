#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CGameServiceManager : public IGameServiceManager
{
	//��������
protected:
	tagGameServiceAttrib			m_GameServiceAttrib;				//��������

	//�������
	CGameServiceManagerHelper		m_AndroidServiceHelper;			//�����˷���

	//��������
public:
	//���캯��
	CGameServiceManager();
	//��������
	virtual ~CGameServiceManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�����ӿ�
public:
	//��������
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer);
	//��������
	virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer);
	//��������
	virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer);

	//�����ӿ�
public:
	//�������
	virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//��������
	virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption);
};

//////////////////////////////////////////////////////////////////////////////////

#endif