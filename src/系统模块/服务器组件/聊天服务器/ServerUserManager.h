#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once
#include <math.h>
#include "AfxTempl.h"

#define PI                      3.1415926		//�г���
#define EARTH_RADIUS            6378.137        //����뾶
//����γ��
class Cltladistance
{
public:
	Cltladistance();
	virtual ~Cltladistance();

public:
	//��ȡ����
    inline double GetRadian(double d);
	//��������
	inline DWORD GetRound(double number);
	//��ȡ����
	inline double GetDistance(double lat1, double lng1, double lat2, double lng2);
};
//////////////////////////////////////////////////////////////////////////////////
//�û�����
enum enUserItemKind
{
	enNoneKind=0,
	enLocalKind,
	enRemoteKind,
};

//�����û�
struct tagInsertLocalUserInfo
{
	DWORD								dwSocketID;						//���ӱ�ʶ
	DWORD								dwLogonTime;					//��¼ʱ��
};

//////////////////////////////////////////////////////////////////////////////////
//�û���Ϣ
class CServerUserItem
{
	//��Ԫ����
	friend class CServerUserManager;

	//��������
protected:
	enUserItemKind					m_enUserKind;						//�û�����

	//��������
protected:
	//���캯��
	CServerUserItem(enUserItemKind enUserKind);
	//��������
	virtual ~CServerUserItem();

	//�û�����
public:
	//�û���ʶ
	virtual DWORD GetUserID()=NULL;

	//������Ϣ
public:
	//�޸�״̬
	virtual VOID SetMainStatus(BYTE cbMainStatus)=NULL;
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID)=NULL;
	//�û�״̬
	virtual BYTE GetMainStatus()=NULL;
	//�û�״̬
	virtual BYTE GetGameStatus()=NULL;
	//�����ʶ
	virtual WORD GetServerID()=NULL;
	//���ӱ�ʶ
	virtual WORD GetTableID()=NULL;
	//���ӵ�ַ
	virtual DWORD GetClientAddr()=NULL;

	//Ч��ӿ�
public:
	//�Ա��ʺ�
	virtual bool ContrastNickName(LPCTSTR pszNickName) = NULL;
	//�Ա�����
	virtual bool ContrastLogonPass(LPCTSTR pszPassword) = NULL;
	//�Ա��ʺ�
	virtual bool ContrastAccounts(LPCTSTR pszAccounts) = NULL;
	//��������
private:
	//��������
	virtual VOID ResetUserItem()=NULL;

	//��������
public:
	//��ȡ����
	enUserItemKind GetUserItemKind() { return m_enUserKind; }
};

//////////////////////////////////////////////////////////////////////////////////

//�û���Ϣ
class CLocalUserItem : public CServerUserItem
{
	//��Ԫ����
	friend class CServerUserManager;

	//���Ա���
protected:
	tagFriendUserInfo				m_UserInfo;							//�û���Ϣ
	DWORD							m_dwSocketID;						//���ӱ�ʶ
	
	//��Ϸ��Ϣ
protected:
	WORD							m_wServerID;						//�����ʶ
	WORD							m_wTableID;							//���ӱ�ʶ

	//��������
protected:
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//�û�����
	DWORD							m_dwLogonTime;						//��¼ʱ��
	bool							m_bLogonLock;						//��¼��

	//��������
protected:
	//���캯��
	CLocalUserItem();
	//��������
	virtual ~CLocalUserItem();

	//�û���Ϣ
public:
	//�û���Ϣ
	virtual tagFriendUserInfo * GetUserInfo() { return &m_UserInfo; }

	//������Ϣ
public:
	//�û��Ա�
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//�û���ʶ
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//�û���ʶ
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//�û��ǳ�
	virtual LPCTSTR GetNickName() { return m_UserInfo.szNickName; }

	//������Ϣ
public:
	//�޸�״̬
	virtual VOID SetMainStatus(BYTE cbMainStatus) { m_UserInfo.cbMainStatus=cbMainStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID);
	//���ӱ�ʶ
	virtual DWORD GetSocketID() { return m_dwSocketID; }
	//��ȡ��¼��״̬
	virtual bool GetLogonLock() { return m_bLogonLock;}
	//�����ʶ
	virtual WORD GetServerID() { return m_wServerID; }
	//���ӱ�ʶ
	virtual WORD GetTableID() { return m_wTableID; }	
	//�û�״̬
	virtual BYTE GetMainStatus() { return m_UserInfo.cbMainStatus; }
	//�û�״̬
	virtual BYTE GetGameStatus() { return m_UserInfo.cbGameStatus; }	
	//���ӵ�ַ
	virtual DWORD GetClientAddr() { return m_UserInfo.dwClientAddr; }

	//Ч��ӿ�
public:
	//�Ա��ʺ�
	virtual bool ContrastNickName(LPCTSTR pszNickName);
	//�Ա�����
	virtual bool ContrastLogonPass(LPCTSTR pszPassword);
	//�Ա��ʺ�
	virtual bool ContrastAccounts(LPCTSTR pszAccounts);

	//��������
public:
	//�޸�����
	VOID ModifyLogonPassword(LPCTSTR pszPassword);
	//���ñ�ʶ
	virtual VOID SetSocketID(DWORD dwSocketID) { m_dwSocketID=dwSocketID; }
	//���õ�¼��״̬
	virtual void SetLogonLock(bool lock) {m_bLogonLock = lock;}
	
	//��¼��Ϣ
public:
	//��¼ʱ��
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//����ʱ��
	virtual VOID SetLogonTime(DWORD dwLogonTime) { m_dwLogonTime=dwLogonTime; }

	//��������
private:	
	//��������
	virtual VOID ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////////////

//�û�������
typedef CWHArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

typedef CWHArray<tagNearUserInfo *> CNearUserInfoItemArray;
typedef CMap<DWORD,DWORD,tagNearUserInfo *,tagNearUserInfo *> CNearUserInfoItemMap;

//�����û���
class CNearUserManager
{
	//�û�����
protected:
	CNearUserInfoItemArray				m_UserItemArray;					//�û�����
	//��������
public:
	//���캯��
	CNearUserManager();
	//��������
	virtual ~CNearUserManager();
public:
	//ö���û�
	virtual tagNearUserInfo * EnumUserItem(DWORD dwEnumIndex);
	//�����û�
	virtual bool InsertNearUserItem(tagNearUserInfo & NearUserItem);
	//��������
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }
};

//�û�������
class CServerUserManager
{
	//�û�����
protected:
	CServerUserItemMap				m_UserIDMap;						//�û�����
	CServerUserItemArray			m_UserItemArray;					//�û�����
	CServerUserItemArray			m_UserItemStore;					//�洢�û�
	Cltladistance					m_ltladistance;						//��ȡ����

	//��������
public:
	//���캯��
	CServerUserManager();
	//��������
	virtual ~CServerUserManager();

	//���Һ���
public:
	//ö���û�
	virtual CServerUserItem * EnumUserItem(WORD wEnumIndex);
	//�����û�
	virtual CServerUserItem * SearchUserItem(DWORD dwUserID);
	//�����û�
	virtual CServerUserItem * SearchUserItem(LPCTSTR pszAccounts);

	//ͳ�ƺ���
public:
	//��������
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }

	//������
public:
	//ɾ���û�
	virtual bool DeleteUserItem();
	//ɾ���û�
	virtual bool DeleteUserItem(DWORD dwUserID);
	//�����û�
	virtual bool InsertLocalUserItem(tagInsertLocalUserInfo & InsertLocalUserInfo,tagFriendUserInfo & UserInfo,LPCTSTR szPassword);

	//��������
public:
	//��������
	virtual DWORD GetNearUserItem(DWORD dwUserID,CNearUserManager & NearUserManager);
	//��ѯ����
	virtual DWORD QueryNearUserItem(DWORD dwUserID,DWORD dwNearUserID,CNearUserManager & NearUserManager);
};

//////////////////////////////////////////////////////////////////////////////////

//���ѷ���
class CUserFriendGroup
{
	//��Ԫ����
	friend class CFriendGroupManager;

	//��������
protected:
	DWORD										m_dwOwnerUserID;				//�û���ʶ
	WORD										m_wGroupContent;				//��������
	WORD										m_wFriendCount;					//������Ŀ

protected:
	tagServerFriendInfo *						m_pFriendInfo;					//�û���Ϣ	

	//��������
public:
	//���캯��
	CUserFriendGroup();
	//��������
	virtual ~CUserFriendGroup();

	//��������
public:
	//���ñ�ʶ
	VOID SetOwnerUserID(DWORD dwUserID) { m_dwOwnerUserID=dwUserID; }
	//��ȡ��ʶ
	DWORD GetOwnerUserID() { return m_dwOwnerUserID; }
	//��ȡ����
	WORD GetGroupContent() { return m_wGroupContent; }
	//��ȡ��Ŀ
	WORD GetFriendCount() { return m_wFriendCount; }

	//���ܺ���
public:
	//��ʼ������
	bool InitFriendGroup(WORD wGroupContent);
	//��Ӻ���
	bool AppendFriendInfo(tagServerFriendInfo & FriendInfo);
	//�Ƴ�����
	bool RemoveFriendInfo(DWORD dwUserID);
	//���Һ���
	tagServerFriendInfo * SearchFriendItem(DWORD dwUserID);
	//ö�ٺ���
	tagServerFriendInfo * EnumFriendItem(WORD wEnumIndex);
	//���÷���
	VOID ResetFriendGroup();


	//��������
private:
	//��������
	tagServerFriendInfo * AllocateContent(WORD wGroupContent);
	//��չ����
	bool ExtendGroupContent(WORD wExtendCount);	
};

//////////////////////////////////////////////////////////////////////////////////

//�û�������
typedef CWHArray<CUserFriendGroup *> CUserFriendGroupArray;
typedef CMap<DWORD,DWORD,CUserFriendGroup *,CUserFriendGroup *> CUserFriendGroupMap;

//���ѹ�����
class CFriendGroupManager
{
	//�û�����
protected:
	CUserFriendGroupMap				m_UserIDMap;						//�û�����
	CUserFriendGroupArray			m_UserFriendGroupArray;				//�û�����
	CUserFriendGroupArray			m_UserFriendGroupStore;				//�洢�û�

	//��������
public:
	//���캯��
	CFriendGroupManager();
	//��������
	virtual ~CFriendGroupManager();

	//���Һ���
public:
	//ö���û�
	virtual CUserFriendGroup * EnumGroupItem(WORD wEnumIndex);
	//�����û�
	virtual CUserFriendGroup * SearchGroupItem(DWORD dwUserID);

	//������
public:
	//ɾ���û�
	virtual bool DeleteFriendGroup();
	//ɾ���û�
	virtual bool DeleteFriendGroup(DWORD dwUserID);
	//�������
	virtual bool InsertFriendGroup(DWORD dwUserID,CUserFriendGroup * pUserFriendGroup);

	//��������
public:
	//��ȡ����
	CUserFriendGroup * ActiveFriendGroup(WORD wFriendCount);
};

//////////////////////////////////////////////////////////////////////////////////

#endif