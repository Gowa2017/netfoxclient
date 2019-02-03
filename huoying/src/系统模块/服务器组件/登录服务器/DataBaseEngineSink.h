#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//�û����ݿ�
protected:
	CDataBaseAide					m_AccountsDBAide;					//�û����ݿ�
	CDataBaseHelper					m_AccountsDBModule;					//�û����ݿ�

	//��Ϸ�����ݿ�
protected:
	CDataBaseAide					m_TreasureDBAide;					//��Ϸ�����ݿ�
	CDataBaseHelper					m_TreasureDBModule;					//��Ϸ�����ݿ�

	//ƽ̨���ݿ�
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

	//�������
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�

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
	bool OnRequestLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ʺŵ�¼
	bool OnRequestLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ʺ�ע��
	bool OnRequestRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��֤����
	bool OnRequestVerifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ο͵�½
	bool OnRequestLogonVisitor(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�ֻ���¼
protected:
	//I D ��¼
	bool OnMobileLogonGameID(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ʺŵ�¼
	bool OnMobileLogonAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ֻ���ע��
	bool OnMobilePhoneRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ʺ�ע��
	bool OnMobileRegisterAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�����
	bool OnMobileModifyPass(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�������Ϣ
	bool OnMobileModifyBankInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�û�����
	bool OnMobileQueryUserData(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������¼
	bool OnMobileLogonOtherPlatform(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ο͵�¼
	bool OnMobileLogonVisitor(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//Ͷ�ߴ���
	bool OnMobileTouSuAgent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���������
	bool OnMobileShenQingAgentCheck(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������
	bool OnMobileShenQingAgent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ��Ϸ������Ϣ
	bool OnMobileQueryGameLockInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ʺŷ���
protected:
	//�޸Ļ���
	bool OnRequestModifyMachine(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�����
	bool OnRequestModifyLogonPass(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�����
	bool OnRequestModifyInsurePass(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�ǩ��
	bool OnRequestModifyUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ʵ����֤
	bool OnRequestModifyRealAuth(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�ͷ��
	bool OnRequestModifySystemFace(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�ͷ��
	bool OnRequestModifyCustomFace(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�����
	bool OnRequestModifyIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//���з���
protected:
	//��ͨ����
	bool OnRequestUserEnableInsure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϸ��
	bool OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�û���������Ϣ
	bool OnRequestQueryBankInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����п���Ϣ
	bool OnRequestBindBankInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�һ����
	bool OnRequestExchangeBankScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ȡ��Ϸ��
	bool OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ת����Ϸ��
	bool OnRequestUserTransferScore(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��������
protected:
	//��ȡ����
	bool OnRequestGetParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ӳ���
	bool OnRequestAddParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸Ĳ���
	bool OnRequestModifyParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ������
	bool OnRequestDeleteParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��Ϣ��ѯ
protected:
	//��ѯ����
	bool OnRequestQueryIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����
	bool OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�û�
	bool OnRequestQueryTransferUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����
	bool OnRequestQueryTransferRebate(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//ǩ������
protected:
	//���ؽ���
	bool OnRequestCheckInReward(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯǩ��
	bool OnRequestCheckInQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ִ��ǩ��
	bool OnRequestCheckInDone(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�������
protected:
	//��������
	bool OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestTaskGiveUp(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ȡ����
	bool OnRequestTaskTake(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ȡ����
	bool OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����
	bool OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�ͱ�����
protected:
	//���صͱ�
	bool OnRequestLoadBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ȡ�ͱ�
	bool OnRequestTakeBaseEnsure(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�ƹ����
protected:
	//�ƹ���Ϣ
	bool OnRequestQuerySpreadInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//ʵ������
protected:
	//ʵ����Ϣ
	bool OnRequestLoadRealAuth(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�ȼ�����
protected:
	//�ȼ�����
	bool OnRequestLoadGrowLevelConfig(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�ȼ�
	bool OnRequestQueryGrowLevelParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//���߷���
protected:
	//���ص���
	bool OnRequestLoadGamePropertyList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������
	bool OnRequestBuyGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ʹ�õ���
	bool OnRequestUseProperty(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����
	bool OnRequestPropertyQuerySingle(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����
	bool OnRequestUserBackpackProperty(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����Buff
	bool OnRequestPropertyBuff(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnRequestPropertyPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����
	bool OnRequestQuerySendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ȡ����
	bool OnRequestGetSendPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����������������
	bool OnRequestServerDummyOnLine(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��Ա����
protected:
	//��Ա����
	bool OnRequestMemberLoadParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ա��ѯ
	bool OnRequestMemberQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ա�ͽ�
	bool OnRequestMemberDayPresent(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ա���
	bool OnRequestMemberDayGift(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�һ�����
protected:
	//�����Ա
	bool OnRequestPurchaseMember(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�һ���Ϸ��
	bool OnRequestExchangeScoreByIngot(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�һ���Ϸ��
	bool OnRequestExchangeScoreByBeans(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�齱����
protected:
	//��������
	bool OnRequestLotteryConfigReq(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�齱��ʼ
	bool OnRequestLotteryStart(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��Ϸ����
protected:
	//��������
	bool OnRequestQueryUserGameData(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//˽�˷���
protected:
	//˽�˷�������
	bool OnRequestGetPersonalParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����˽�˷�����Ϣ
	bool OnRequestqueryPersonalRoomInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	bool OnRequsetDissumeRoomTest(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//��һ�ȡ����ɼ�
	bool OnRequestQueryUserRoomScore(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//д�뷿�����ʱ��
	bool OnRequestCloseRoomWriteDissumeTime(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����˽�˷��ķ�����Ϣ
	bool OnRequestPersonalRoomUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����һ���Ϸ��
	bool OnRequestRoomCardExchangeToScore(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�ʺŰ�
protected:
	///�ʺŰ�
	bool OnRequestAccountBind(DWORD dwContextID, VOID * pData, WORD wDataSize);
	///�ʺŰ�
	bool OnRequestAccountBindExists(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//IP������
	bool OnRequestIpLocation(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//ϵͳ����
protected:
	//�����б�
	bool OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	//ƽ̨����
	bool OnRequestPlatformParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�������
protected:
	//��¼���
	VOID OnLogonDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString,DWORD dwCheckUserRight,bool bMobileClient);
	//���н��
	VOID OnInsureDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient);
	//�������
	VOID OnOperateDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient,bool bCloseSocket=true);
	//�������
	VOID OnAndroidDisposeResult(DWORD dwContextID,DWORD dwErrorCode,WORD wSubCommdID,WORD wServerID);
	//���Ͻ��
	VOID OnIndividualDisposeResult(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);
};

//////////////////////////////////////////////////////////////////////////////////

#endif