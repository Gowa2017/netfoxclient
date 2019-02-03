#ifndef SERVER_INFO_MANAGER_HEAD_FILE
#define SERVER_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagGameServerInfo *> CGameServerInfoArray;
typedef CWHArray<tagGameMatchOption *> CGameMatchOptionArray;

//��������
typedef CMap<WORD,WORD,tagGameServerInfo *,tagGameServerInfo *> CGameServerInfoMap;
typedef CMap<DWORD,DWORD,tagGameMatchOption *,tagGameMatchOption *> CGameMatchOptionMap;

//////////////////////////////////////////////////////////////////////////////////

//ģ������
class MODULE_MANAGER_CLASS CServerInfoBuffer
{
	//��������
public:
	CGameServerInfoMap				m_GameServerInfoMap;				//ģ������
	CGameServerInfoArray			m_GameServerInfoArray;				//ģ������

	//��������
public:
	//���캯��
	CServerInfoBuffer();
	//��������
	virtual ~CServerInfoBuffer();

	//������
public:
	//��������
	bool ResetServerInfo();
	//ɾ������
	bool DeleteServerInfo(WORD wServerID);
	//��������
	bool InsertServerInfo(tagGameServerInfo * pGameServerInfo);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetServerInfoCount();
	//��������
	tagGameServerInfo * SearchServerInfo(WORD wServerID);

	//�ڲ�����
private:
	//��������
	tagGameServerInfo * CreateServerInfo();
};

//////////////////////////////////////////////////////////////////////////////////
//ģ������
class MODULE_MANAGER_CLASS CMatchOptionBuffer
{
	//��������
public:
	//CGameMatchOptionMap			m_GameMatchOptionMap;				//ģ������
	CGameMatchOptionArray		m_GameMatchOptionArray;			    //ģ������
	CGameMatchOptionArray		m_GameMatchOptionBuffer;			//ģ������	

	//��������
public:
	//���캯��
	CMatchOptionBuffer();
	//��������
	virtual ~CMatchOptionBuffer();

	//������
public:
	//��������
	bool ResetMatchOption();
	//ɾ������
	bool DeleteMatchOption(DWORD dwMatchID,DWORD wMatchNO);
	//��������
	bool InsertMatchOption(tagGameMatchOption * pGameMatchOption);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetMatchOptionCount();
	//��������
	tagGameMatchOption * SearchMatchOption(DWORD dwMatchID,LONGLONG wMatchNO);

	//�ڲ�����
private:
	//��������
	tagGameMatchOption * CreateMatchOption();
};

//////////////////////////////////////////////////////////////////////////////////

//ģ����Ϣ
class MODULE_MANAGER_CLASS CServerInfoManager
{
	//��������
public:
	//���캯��
	CServerInfoManager();
	//��������
	virtual ~CServerInfoManager();

	//���ط���
public:
	//���ط���
	bool LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer);
	//���ط���
	bool LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], WORD wServerID, tagGameServerInfo & GameServerResult);

	//ע�����
public:
	//ɾ������
	bool DeleteGameServer(WORD wServerID);
	//���뷿��
	bool InsertGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult);
	//�޸ķ���
	bool ModifyGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult);

	//�ڲ�����
private:
	//��������
	bool ConnectPlatformDB(CDataBaseHelper & PlatformDBModule);
	//��ȡ����
	bool ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameServerInfo & GameServerResult);

	//˽�˷�����Ϣ
public:
	tagPersonalRoomOption m_PersonalRoomOption;
	tagPersonalRoomOptionRightAndFee m_PersonalRoomOptionRightAndFee;

	//����˽�˷������
	void SetPersonalRoomOption(tagPersonalRoomOption PesronalRoomOption, tagPersonalRoomOptionRightAndFee  PersonalRoomOptionRightAndFee);

	//����˽�˷�������
	bool LoadPersonalRoomParameter(tagPersonalRoomOption  & pesronalRoomOption,  tagPersonalRoomOptionRightAndFee  &personalRoomOptionRightAndFee,  WORD wServerID);

	//����˽�˷������
	bool InsertPersonalRoomParameter(CDataBaseAide PlatformDBAide, tagPersonalRoomOption  PersonalRoomOption,  tagPersonalRoomOptionRightAndFee PersonalRoomOptionRightAndFee , WORD dwServerID,  WORD dwKindID);

	//�޸�˽�˷������
	bool ModifyPersonalRoomParameter(CDataBaseAide PlatformDBAide, tagPersonalRoomOption  PersonalRoomOption, tagPersonalRoomOptionRightAndFee PersonalRoomOptionRightAndFee , WORD dwServerID, WORD dwKindID);



};

//////////////////////////////////////////////////////////////////////////////////

#endif