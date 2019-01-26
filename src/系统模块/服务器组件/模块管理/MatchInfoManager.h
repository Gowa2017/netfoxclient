#ifndef MATCH_INFO_MANAGER_HEAD_FILE
#define MATCH_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagGameMatchOption *> CGameMatchOptionArray;
typedef CWHArray<tagMatchRewardInfo *> CMatchRewardInfoArray;

//��������
typedef CMap<DWORD,DWORD,tagGameMatchOption *,tagGameMatchOption *> CGameMatchOptionMap;
typedef CMap<DWORD,DWORD,tagMatchRewardInfo *,tagMatchRewardInfo *> CMatchRewardInfoMap;

//////////////////////////////////////////////////////////////////////////
//��������
class MODULE_MANAGER_CLASS CMatchInfoBuffer
{
	//��������
public:
	CGameMatchOptionArray		m_GameMatchOptionArray;			    //ģ������
	CGameMatchOptionArray		m_GameMatchOptionBuffer;			//ģ������	

	//��������
public:
	//���캯��
	CMatchInfoBuffer();
	//��������
	virtual ~CMatchInfoBuffer();

	//������
public:
	//��������
	bool ResetMatchOption();
	//ɾ������
	bool DeleteMatchOption(DWORD dwMatchID);
	//��������
	tagGameMatchOption * InsertMatchOption(tagGameMatchOption * pGameMatchOption);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetMatchOptionCount();
	//��������
	tagGameMatchOption * SearchMatchOption(DWORD dwMatchID);

	//�ڲ�����
private:
	//��������
	tagGameMatchOption * CreateMatchOption();
};

//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CRewardInfoBuffer
{
	//��������
public:
	CMatchRewardInfoArray		m_MatchRewardInfoArray;			    //��������
	CMatchRewardInfoArray		m_MatchRewardInfoBuffer;			//��������	

	//��������
public:
	//���캯��
	CRewardInfoBuffer();
	//��������
	virtual ~CRewardInfoBuffer();

	//������
public:
	//��������
	bool ResetMatchRewardInfo();	
	//ɾ������
	bool DeleteMatchRewardInfo(WORD wRankID);
	//��������
	tagMatchRewardInfo * InsertMatchRewardInfo(tagMatchRewardInfo & MatchRewardInfo);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetMatchRewardCount();
	//��������
	tagMatchRewardInfo * SearchMatchReward(WORD wRankID);

	//�ڲ�����
private:
	//��������
	tagMatchRewardInfo * CreateMatchRewardInfo();
};

//////////////////////////////////////////////////////////////////////////

//ģ����Ϣ
class MODULE_MANAGER_CLASS CMatchInfoManager
{
	//��������
public:
	//���캯��
	CMatchInfoManager();
	//��������
	virtual ~CMatchInfoManager();

	//��������
public:
	//���ؽ���
	WORD LoadGameMatchReward(DWORD dwMatchID,CRewardInfoBuffer & RewardInfoBuffer);

	//��������
public:		
	//���ر���
	bool LoadGameMatchOption(WORD wKindID,DWORD dwMatchID,tagGameMatchOption & GameMatchResult);
	//���ر���
	bool LoadGameMatchOption(WORD wKindID,TCHAR szMachineID[LEN_MACHINE_ID],CMatchInfoBuffer & MatchInfoBuffer);	

	//��������
public:
	//ɾ������
	bool DeleteGameMatchOption(DWORD dwMatchID);
	//���뷿��
	bool InsertGameMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagGameMatchOption & GameMatchResult);
	//�޸ķ���
	bool ModifyGameMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagGameMatchOption & GameMatchResult);

	//�ڲ�����
private:
	//��������
	bool ConnectPlatformDB(CDataBaseHelper & PlatformDBModule);
	//��������
	bool ConnectGameMatchDB(CDataBaseHelper & GameMatchDBModule);	
	//��ȡ����
	bool ReadGameMatchOption(CDataBaseAide & GameMatchDBAide, tagGameMatchOption & GameMatchResult);
};

//////////////////////////////////////////////////////////////////////////

#endif