#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
class CDataBaseEngineSink : public IDataBaseEngineSink, public IGameDataBaseEngine
{
	//��Ԫ����
	friend class CServiceUnits;

	//��������
protected:
	DBO_GR_LogonFailure				m_LogonFailure;						//��¼ʧ��
	DBO_GR_LogonSuccess				m_LogonSuccess;						//��¼�ɹ�

	//��Ϸ���ݿ�
protected:
	CDataBaseAide					m_GameDBAide;						//��Ϸ����
	CDataBaseHelper					m_GameDBModule;						//��Ϸ����

	//��Ϸ�����ݿ�
protected:
	CDataBaseAide					m_TreasureDBAide;					//��Ϸ�����ݿ�
	CDataBaseHelper					m_TreasureDBModule;					//��Ϸ�����ݿ�

	//ƽ̨���ݿ�
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

	//���ñ���
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagDataBaseParameter *			m_pDataBaseParameter;				//������Ϣ
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//�������
protected:
	IDataBaseEngine *				m_pIDataBaseEngine;					//����ӿ�
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�

	//��ѯ�ӿ�
protected:
	IGameDataBaseEngineSink *		m_pIGameDataBaseEngineSink;			//���ݽӿ�

	//�������
public:
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //����Э��

	//��������
public:
	//���캯��
	CDataBaseEngineSink();
	//��������
	virtual ~CDataBaseEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ò���
public:
	//�Զ�����
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��������
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//��ȡ����
public:
	//��ȡ����
	virtual VOID * GetDataBase(REFGUID Guid, DWORD dwQueryVer);
	//��ȡ����
	virtual VOID * GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer);

	//���ܽӿ�
public:
	//Ͷ�ݽ��
	virtual bool PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��¼����
protected:
	//I D ��¼
	bool OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID,BOOL bMatch=false);
	//I D ��¼
	bool OnRequestLogonMobile(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�ʺŵ�¼
	bool OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//ϵͳ����
protected:
	//��Ϸд��
	bool OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�뿪����
	bool OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��Ϸ��¼
	bool OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���ز���
	bool OnRequestLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�����б�
	bool OnRequestLoadGameColumn(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���ػ���
	bool OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���ػ���
	bool OnRequestLoadAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�ȼ�����
	bool OnRequestLoadGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�������
	bool OnRequestBuyGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestPropertyBackpack(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ʹ�õ���
	bool OnRequestPropertyUse(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ѯ����
	bool OnRequestQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���͵���
	bool OnRequestPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ȡ����
	bool OnRequestGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//����Buff
	bool OnRequestLoadGameBuff(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestLoadGameTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestSendTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�û���Ϸ����
	bool OnRequestWriteUserGameData(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�û�Ȩ��
	bool OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//����Ȩ��
	bool OnRequestManageMatchRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ϵͳ��Ϣ
	bool OnRequestLoadSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�������д�
	bool OnRequestLoadSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//����������
	bool OnRequestUnlockAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//��������
protected:
	//��������
	bool OnRequestMatchSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�˳�����
	bool OnRequestMatchUnSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//������ʼ
	bool OnRequestMatchSignupStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ѯ�ʸ�
	bool OnRequestMatchQueryQualify(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//������
	bool OnRequestMatchBuySafeCard(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//������ʼ
	bool OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//����ȡ��
	bool OnRequestMatchCancel(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//������̭
	bool OnRequestMatchEliminate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��¼�ɼ�
	bool OnRequestMatchRecordGrades(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//��Ҹ���
	bool OnRequestMatchUserRevive(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ѯ����
	bool OnRequestMatchQueryReviveInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//�ȼ�����
protected:
	//��ѯ�ȼ�
	bool OnRequestQueryGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//���з���
protected:
	//��ͨ����
	bool OnRequestUserEnableInsure(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//������Ϸ��
	bool OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ȡ��Ϸ��
	bool OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ת����Ϸ��
	bool OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ѯ����
	bool OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ѯ�û�
	bool OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//�������
protected:
	//��������
	bool OnRequestTaskGiveUp(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ȡ����
	bool OnRequestTaskTake(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ȡ����
	bool OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//��ѯ����
	bool OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�ƽ�����
	bool OnRequestRoomTaskProgress(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��Ա����
protected:
	//��Ա����
	bool OnRequestMemberLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��Ա��ѯ
	bool OnRequestMemberQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��Ա�ͽ�
	bool OnRequestMemberDayPresent(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��Ա���
	bool OnRequestMemberDayGift(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//�һ�����
protected:
	//�����Ա
	bool OnRequestPurchaseMember(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�һ���Ϸ��
	bool OnRequestExchangeScoreByIngot(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�һ���Ϸ��
	bool OnRequestExchangeScoreByBeans(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//˽�˷���
protected:
	//��������
	bool OnRequestCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���봴�����Ӽ�¼
	bool OnRequsetInsertCreateRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ɢ����
	bool OnRequsetDissumeRoom(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ȡ������
	bool OnRequestCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���ǿ��ȡ��
	bool OnRequestHostCancelCreateTable(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//˽������
	bool OnRequestLoadPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��һ�ȡ����ɼ�
	bool OnRequestQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��Ϸд��
	bool OnRequestWritePersonalGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//д�������Ϣ
	bool OnRequestWriteJoinInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ǩ������
protected:
	//����ǩ��
	bool OnRequestLoadCheckIn(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ѯǩ��
	bool OnRequestCheckInQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ִ��ǩ��
	bool OnRequestCheckInDone(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//�ͱ�����
protected:
	//���صͱ�
	bool OnRequestLoadBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ȡ�ͱ�
	bool OnRequestTakeBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//��������
private:
	//��¼���
	VOID OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbDeviceType=DEVICE_TYPE_PC,WORD wBehaviorFlags=0,WORD wPageTableCount=0);
	//���н��
	VOID OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, SCORE lFrozenedScore, LPCTSTR pszErrorString, bool bMobileClient,BYTE cbActivityGame=FALSE);
};

//////////////////////////////////////////////////////////////////////////////////

#endif