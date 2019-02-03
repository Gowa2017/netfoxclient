#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "ChatServiceHead.h"
#include "ServerUserManager.h"
#include <list>


//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��

	//�汾��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwProcessVersion;					//���̰汾

	//�û�����
	BYTE							cbClientKind;						//��������
	CServerUserItem *				pServerUserItem;					//�û��ӿ�
};

//////////////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bCollectUser;						//���ܱ�־
	bool							m_bNeekCorrespond;					//Э����־

	//���Ʊ���
protected:
	DWORD							m_dwIntervalTime;					//���ʱ��
	DWORD							m_dwLastDisposeTime;				//����ʱ��
	std::list<DWORD>				m_LogonLockList;					//��¼������

	//����Ϣ
protected:
	tagBindParameter *				m_pNormalParameter;					//����Ϣ
	tagBindParameter *				m_pAndroidParameter;				//����Ϣ

	//������Ϣ
protected:
	CInitParameter *				m_pInitParameter;					//���ò���

	//�������
protected:
	CServerUserManager				m_ServerUserManager;				//�û�����
	CFriendGroupManager				m_FriendGroupManager;				//���ѹ���

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IAttemperEngine *				m_pIAttemperEngine;					//��������
	ITCPSocketService *				m_pITCPSocketService;				//�������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//�Զ��¼�
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);
	//���Ӵ���
protected:
	//�б��¼�
	bool OnEventTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�û��¼�
	bool OnEventTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�û�״̬
	bool OnEventTCPSocketSubUserStatus(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�û�ǿ�����ߣ�����ٸ��ʿ���״̬��
	bool OnEventTCPSocketSubForceOffline(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��������
public:
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);

//////////////////////////////////////////////////////////////////////////

	//�����¼�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�����¼�
protected:
	//�û�����
	bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��¼����
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ID��¼
	bool OnTCPNetworkSubMBLogonByUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�˺ŵ�¼
	bool OnTCPNetworkSubMBLogonByAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��������
	bool OnTCPNetworkSubApplyForFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���ѻ�Ӧ
	bool OnTCPNetworkSubRespondFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubRoomInvite(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//˽������
	bool OnTCPNetworkSubInvitePersonal(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����û�
	bool OnTCPNetworkSubSearchUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ɾ������
	bool OnTCPNetworkDeleteFriend(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkUpdateCoordinate(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������
	bool OnTCPNetworkGetNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ���
	bool OnTCPNetworkQueryNearuser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������Ϣ
	bool OnTCPNetworkSubUserShare(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��������
protected:
	//��������
	bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData = NULL, WORD wDataSize = 0);
	//��������
	bool SendChatServerConfig(DWORD dwUserID);
	//��������
	bool SendSystemMessage(BYTE cbSendMask, LPCTSTR lpszMessage, WORD wType);
	//��������
	bool SendSystemMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType);
	//��¼ʧ��
	bool SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID);
	//�������
	bool SendOperateFailure(LPCTSTR pszString, LONG lErrorCode,LONG lOperateCode,DWORD dwSocketID);
	//�������
	bool SendOperateSuccess(LPCTSTR pszString,LONG lOperateCode,DWORD dwSocketID);
	//��������
	bool SendDataToUserFriend(DWORD dwUserID,WORD wMainCmdID, WORD wSubCmdID,VOID * pData,WORD wDataSize);
	//�洢������Ϣ
	bool SaveOfflineMessage(DWORD dwUserID,WORD wMessageType,VOID * pData,WORD wDataSize,DWORD dwSocketID);
	//���û�
	CServerUserItem * GetBindUserItem(WORD wBindIndex);
	//�󶨲���
	tagBindParameter * GetBindParameter(WORD wBindIndex);

//////////////////////////////////////////////////////////////////////////

protected:
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����¼�

	//��¼�ɹ�
	bool OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBAccountLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBGameParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���ط���
	bool OnDBLoadUserGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���غ���
	bool OnDBLoadUserFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���ѱ�ע
	bool OnDBLoadUserRemarksInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBLoadUserSimpleIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBLoadUserOfflineMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBLoadDeleteFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�����
	bool OnDBModifyUserPassword(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�ǩ��
	bool OnDBModifyUserUnderWrite(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸�����
	bool OnDBModifyUserIndividual(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸ķ���
	bool OnDBModifyUserGroup(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�޸ĺ���
	bool OnDBModifyUserFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	//�޸ı�ע
	bool OnDBModifyUserRemarks(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ӻ���
	bool OnDBUserAddFriend(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������
	bool OnDBUserOperateResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����û�
	bool OnDBSearchUserResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBTrumpetResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

};

//////////////////////////////////////////////////////////////////////////////////

#endif