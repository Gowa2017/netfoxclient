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
protected:
	DBO_GR_LogonFailure				m_LogonFailure;						//��¼ʧ��
	DBO_GR_LogonSuccess				m_LogonSuccess;						//��¼�ɹ�

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
	bool OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�˺ŵ�¼
	bool OnRequestLogonUserAccounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸ĺ���
	bool OnRequestModifyFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ӻ���
	bool OnRequestAddFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ɾ������
	bool OnRequestDeleteFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸ķ���
	bool OnRequestModifyGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸ı�ע
	bool OnRequestModifyRemarks(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����û�
	bool OnRequestSearchUser(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�洢��Ϣ
	bool OnRequestSaveOfflineMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnRequestTrumpet(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��������
private:
	//�������
	VOID OnUserOperateResult(DWORD dwContextID,bool bOperateSucess,DWORD dwErrorCode,WORD wOperateCode,LPCTSTR pszDescribeString);
};

//////////////////////////////////////////////////////////////////////////////////

#endif