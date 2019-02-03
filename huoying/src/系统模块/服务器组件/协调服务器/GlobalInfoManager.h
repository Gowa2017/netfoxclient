#ifndef GLOBAL_USER_MANAGER_HEAD_FILE
#define GLOBAL_USER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "vector"
//////////////////////////////////////////////////////////////////////////////////

//��˵��
class CGlobalUserItem;
class CGlobalPlazaItem;
class CGlobalServerItem;
class CGlobalChatServerItem;
class CGlobalChatUserItem;
class CGlobalDummyItem;

//���鶨��
typedef CWHArray<CGlobalUserItem *> CGlobalUserItemArray;
typedef CWHArray<CGlobalChatUserItem *> CGlobalChatUserItemArray;
typedef CWHArray<CGlobalPlazaItem *> CGlobalPlazaItemArray;
typedef CWHArray<CGlobalServerItem *> CGlobalServerItemArray;
typedef CWHArray<CGlobalChatServerItem*> CGlobalChatServerItemArray;

//��������
typedef CMap<DWORD,DWORD,CGlobalUserItem *,CGlobalUserItem *> CMapUserID;
typedef CMap<DWORD,DWORD,CGlobalChatUserItem *,CGlobalChatUserItem *> CMapChatUserID;
typedef CMap<DWORD,DWORD,CGlobalUserItem *,CGlobalUserItem *> CMapGameID;
typedef CMap<WORD,WORD,CGlobalPlazaItem *,CGlobalPlazaItem *> CMapPlazaID;
typedef CMap<WORD,WORD,CGlobalServerItem *,CGlobalServerItem *> CMapServerID;
typedef CMap<WORD,WORD,CGlobalChatServerItem *,CGlobalChatServerItem *> CMapChatServerID;
typedef CMap<CString,LPCTSTR,tagPersonalTableInfo*,tagPersonalTableInfo* &> CMapPersonalTableInfo;
typedef CMap<WORD,WORD,CGlobalDummyItem*,CGlobalDummyItem*> CMapDummyInfo;

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//��Ϸ�û���Ϣ
struct tagGlobalUserInfo
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�

	//������Ϣ
	BYTE							cbGender;							//�û��Ա�
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	BYTE							cbMasterOrder;						//����ȼ�

	BYTE							cbAndroidUser;						//���������

	//��ϸ��Ϣ
	tagUserInfo						userInfo;							//�û���ϸ��Ϣ
};

//�����û���Ϣ
struct tagGlobalChatUserInfo
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
};

struct tagDummyInfo
{
	WORD							wServerID;							//�����ID
	DWORD							dwDummyCount;						//��������
};

//˽�˷�����Ϣ
struct tagPersonalTable
{
	DWORD							dwServerID;							//����I D
	DWORD							dwTableID;							//����I D
};

struct tagServerTableCount
{
	DWORD							dwKindID;							//��ϷI D
	DWORD							dwServerID;							//����I D
	DWORD							dwTableCount;						//������Ŀ
};

typedef CWHArray<tagServerTableCount *> CServerTableCountArray;

//////////////////////////////////////////////////////////////////////////////////

//�û�����
class CGlobalUserItem
{
	//��Ԫ����
	friend class CGlobalInfoManager;

	//�û�����
public:
	DWORD							m_dwUserID;							//�û���ʶ
	DWORD							m_dwGameID;							//��Ϸ��ʶ
	TCHAR							m_szNickName[LEN_NICKNAME];			//�û��ǳ�

	//������Ϣ
public:
	BYTE							m_cbGender;							//�û��Ա�
	BYTE							m_cbMemberOrder;					//��Ա�ȼ�
	BYTE							m_cbMasterOrder;					//����ȼ�
	BYTE							m_cbAndroidUser;					//�������û�
	tagUserInfo						m_UserInfo;							//�û���ϸ��Ϣ

	//������Ϣ
public:
	CGlobalServerItemArray			m_GlobalServerItemArray;			//��������

	//��������
protected:
	CGlobalUserItem *				m_pNextUserItemPtr;					//����ָ��

	//��������
protected:
	//���캯��
	CGlobalUserItem();
	//��������
	virtual ~CGlobalUserItem();

	//���ܺ���
public:
	//�û���ʶ
	DWORD GetUserID() { return m_dwUserID; }
	//��Ϸ��ʶ
	DWORD GetGameID() { return m_dwGameID; }
	//�û��ǳ�
	LPCTSTR GetNickName() { return m_szNickName; }
	//������
	bool IsAndroid() { return m_cbAndroidUser; }

	//�û���Ϣ
public:
	//�û��Ա�
	BYTE GetGender() { return m_cbGender; }
	//��Ա�ȼ�
	BYTE GetMemberOrder() { return m_cbMemberOrder; }
	//��Ա�ȼ�
	BYTE GetMasterOrder() { return m_cbMasterOrder; }
	//��ϸ��Ϣ
	tagUserInfo* GetUserInfo() {return &m_UserInfo; }
	//����״̬
	void UpdateStatus(const WORD wTableID, const WORD wChairID, const BYTE cbUserStatus);

	//������Ϣ
public:
	//ö�ٷ���
	CGlobalServerItem * EnumServerItem(WORD wIndex);
};

//////////////////////////////////////////////////////////////////////////////////

//�û�����
class CGlobalDummyItem
{
	//��Ԫ����
	friend class CGlobalInfoManager;

	//��������
public:
	WORD							wServerID;						//ServerID
	tagDummyInfo					m_DummyInfo;					//��������

	//��������
protected:
	CGlobalDummyItem *				m_pNextItemPtr;					//����ָ��

	//��������
protected:
	//���캯��
	CGlobalDummyItem();
	//��������
	virtual ~CGlobalDummyItem();
};


//////////////////////////////////////////////////////////////////////////////////

//�㳡����
class CGlobalPlazaItem
{
	//��Ԫ����
	friend class CGlobalInfoManager;

	//��������
public:
	WORD							m_wIndex;							//������
	tagGamePlaza					m_GamePlaza;						//��Ϸ�㳡

	//��������
public:
	CMapUserID						m_MapUserID;						//�û�����

	//��������
protected:
	CGlobalPlazaItem *				m_pNextPlazaItemPtr;				//����ָ��

	//��������
protected:
	//���캯��
	CGlobalPlazaItem();
	//��������
	virtual ~CGlobalPlazaItem();
};

//////////////////////////////////////////////////////////////////////////////////

//��������
class CGlobalServerItem
{
	//��Ԫ����
	friend class CGlobalInfoManager;

	//��������
public:
	WORD							m_wIndex;							//������
	tagGameServer					m_GameServer;						//��Ϸ����
	tagGameMatch					m_GameMatch;

	//��������
public:
	CMapUserID						m_MapUserID;						//�û�����

	//��������
protected:
	CGlobalServerItem *				m_pNextServerItemPtr;				//����ָ��

	//��������
protected:
	//���캯��
	CGlobalServerItem();
	//��������
	virtual ~CGlobalServerItem();

	//���ܺ���
public:
	//������
	WORD GetIndex() { return m_wIndex; }
	//��ȡ����
	WORD GetKindID() { return m_GameServer.wKindID; }
	//��ȡ����
	WORD GetServerID() { return m_GameServer.wServerID; }
	//�û���Ŀ
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }
	//��������Ŀ
	DWORD GetAndroidUserItemCount();
	//��ʵ����Ŀ
	DWORD GetRealUserItemCount();
	//������Ŀ
	WORD GetTabelCount() { return m_GameServer.wTableCount; }
	//��������
	bool IsMatchServer() { return (m_GameMatch.wServerID==m_GameServer.wServerID) && m_GameMatch.dwMatchID!=0; }

	//���Һ���
public:
	//Ѱ���û�
	CGlobalUserItem * SearchUserItem(DWORD dwUserID);
};

//////////////////////////////////////////////////////////////////////////////////

//��������
class CGlobalChatServerItem
{
	//��Ԫ����
	friend class CGlobalInfoManager;

	//��������
public:
	WORD							m_wIndex;							//������
	tagChatServer					m_ChatServer;						//�����������Ϣ

	//��������
public:
	CMapChatUserID						m_MapUserID;					//�û�����

	//��������
protected:
	CGlobalChatServerItem *				m_pNextChatServerPtr;				//����ָ��

	//��������
protected:
	//���캯��
	CGlobalChatServerItem();
	//��������
	virtual ~CGlobalChatServerItem();

	//���ܺ���
public:
	//������
	WORD GetIndex() { return m_wIndex; }
	//�û���Ŀ
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }

	//���Һ���
public:
	//Ѱ���û�
	CGlobalChatUserItem * SearchChatUserItem(DWORD dwUserID);
};

//////////////////////////////////////////////////////////////////////////////////

//�û�����
class CGlobalChatUserItem
{
	//��Ԫ����
	friend class CGlobalInfoManager;

	//�û�����
public:
	DWORD							m_dwUserID;							//�û���ʶ
	TCHAR							m_szNickName[LEN_NICKNAME];			//�û��ǳ�

	//������Ϣ
public:
	CGlobalChatServerItemArray			m_GlobalServerItemArray;		//�������������

	//��������
protected:
	CGlobalChatUserItem *				m_pNextUserItemPtr;					//����ָ��

	//��������
protected:
	//���캯��
	CGlobalChatUserItem();
	//��������
	virtual ~CGlobalChatUserItem();

	//���ܺ���
public:
	//�û���ʶ
	DWORD GetUserID() { return m_dwUserID; }
	//�û��ǳ�
	LPCTSTR GetNickName() { return m_szNickName; }
};

//////////////////////////////////////////////////////////////////////////////////

//ȫ����Ϣ
class CGlobalInfoManager
{
	//��������
protected:
	CMapUserID						m_MapUserID;						//��Ϸ�û���ʶ
	CMapGameID						m_MapGameID;						//��ʶ����
	CMapStringToPtr					m_MapNickName;						//�ǳ�����


	CMapChatUserID					m_MapChatUserID;					//�����û���ʶ
	CMapDummyInfo					m_MapDummyInfo;						//������������
	CMapPlazaID						m_MapPlazaID;						//�㳡����˱�ʶ
	CMapServerID					m_MapServerID;						//�������˱�ʶ
	CMapChatServerID				m_MapChatID;						//�������˱�ʶ

	//++++++++++++++++++++++++++++++Լս����
	CServerTableCountArray			m_ServerTableCountArray;//���������������
	CMapPersonalTableInfo			m_MapPersonalTableInfo;				//˽�˷�����Ϣ
	std::vector<tagPersonalTableInfo>			m_VecDissumePersonalTableInfo;				//����ɢ��˽�˷�����Ϣ
	std::vector<CMD_CS_S_RegisterPersonal> m_vecPersonalRoomMaxCreate;				//˽�˷����Դ�������󷿼���
	//��������
protected:


	//�洢����
protected:
	CGlobalUserItem *				m_pGlobalUserItem;					//�û��洢
	CGlobalChatUserItem *			m_pGlobalChatUserItem;				//�����û��洢
	CGlobalPlazaItem *				m_pGlobalPlazaItem;					//�㳡�洢
	CGlobalServerItem *				m_pGlobalServerItem;				//����洢
	CGlobalChatServerItem *			m_pGlobalChatServerItem;			//����������洢
	CGlobalDummyItem *				m_pGlobalDummyItem;					//�����û�

	//��������
public:
	//���캯��
	CGlobalInfoManager();
	//��������
	virtual ~CGlobalInfoManager();

	//������
public:
	//��������
	VOID ResetData();
	//�û���Ŀ
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }
	//������Ŀ
	DWORD GetPlazaItemCount() { return (DWORD)m_MapPlazaID.GetCount(); }
	//������Ŀ
	DWORD GetServerItemCount() { return (DWORD)m_MapServerID.GetCount(); }
	//������Ŀ
	DWORD GetChatItemCount() { return (DWORD)m_MapChatID.GetCount(); }

	//˽�˷���
public:
	//��������
	tagPersonalTableInfo* SearchTableByID(LPCTSTR lpszTableID);
	//��������
	tagPersonalTableInfo* SearchTableByTableIDAndServerID(DWORD dwServerID, DWORD dwTableID);
	//�������
	bool AddFreeServerTable(DWORD dwServerID);
	//�������
	bool AddServerTable(CString strServerID, tagPersonalTableInfo PersonalTable);
	//�Ƴ�����
	bool RemoveFreeServerTable(DWORD dwServerID);
	//�Ƴ�����
	bool RemoveServerTable(DWORD dwServerID, DWORD dwTableID);
	//�Ƴ�����
	bool RemoveServerTable(DWORD dwServerID);
	//��ȡ����
	DWORD GetFreeServer(DWORD dwUserID, DWORD dwKindID, BYTE cbIsJoinGame);
	//���ɷ���ID
	VOID RandServerID(LPTSTR pszServerID, WORD wMaxCount);

	//��ȡ�������������з���
	VOID GetHostCreatePersonalRoom(tagHostCreatRoomInfo & HostCreatRoomInfo);

	//��ȡ���������ķ��������
	INT GetHostCreatePersonalRoomCount(DWORD dwUserID);
	//�������
	bool AddPersonalMaxCreate(CMD_CS_S_RegisterPersonal RegisterPersonal);
	//�Ƿ�����ٴ�������
	bool CanCreatePersonalRoom(DWORD dwKindID,  DWORD dwUserID);

	//��Ϸ�û�����
public:
	//ɾ���û�
	bool DeleteUserItem(DWORD dwUserID, WORD wServerID);
	//�����û�
	bool ActiveUserItem(tagGlobalUserInfo & GlobalUserInfo, WORD wServerID);

	//�����û�����
public:
	//ɾ�������û�
	bool DeleteChatUserItem(DWORD dwUserID, WORD wServerID);
	//�����û�
	bool ActiveChatUserItem(tagGlobalChatUserInfo & GlobalUserInfo, WORD wServerID);

	//�����û�����
public:
	//ɾ�������û�
	bool DeleteDummyItem(WORD wServerID);
	//������������û�
	bool ClearDummyItem();
	//���������û�
	bool ActiveDummyItem(tagDummyInfo & DummyInfo, WORD wServerID);

	//�㳡����
public:
	//ɾ���㳡
	bool DeletePlazaItem(WORD wPlazaID);
	//����㳡
	bool ActivePlazaItem(WORD wBindIndex, tagGamePlaza & GamePlaza);

	//�������
public:
	//ɾ������
	bool DeleteServerItem(WORD wServerID);
	//�����
	bool ActiveServerItem(WORD wBindIndex, tagGameServer & GameServer);

	//�������
public:
	//ɾ������
	bool DeleteChatItem(WORD wChatID);
	//��������
	bool ActiveChatItem(WORD wBindIndex, tagChatServer & ChatServer);

	//�������
public:
	//Ѱ�ҹ㳡
	CGlobalPlazaItem * SearchPlazaItem(WORD wPlazaID);
	//Ѱ�ҷ���
	CGlobalServerItem * SearchServerItem(WORD wServerID);
	//Ѱ������
	CGlobalChatServerItem * SearchChatServerItem(WORD wChatID);
	//Ѱ����������
	CGlobalDummyItem * SearchDummyItem(WORD wServerID);

	//�û�����
public:
	//Ѱ���û�
	CGlobalUserItem * SearchUserItemByUserID(DWORD dwUserID);
	//Ѱ���û�
	CGlobalChatUserItem * SearchChatUserItemByUserID(DWORD dwUserID);
	//Ѱ���û�
	CGlobalUserItem * SearchUserItemByGameID(DWORD dwGameID);
	//Ѱ���û�
	CGlobalUserItem * SearchUserItemByNickName(LPCTSTR pszNickName);

	//ö�ٺ���
public:
	//ö���û�
	CGlobalUserItem * EnumUserItem(POSITION & Position);
	//ö���û�
	CGlobalChatUserItem * EnumChatUserItem(POSITION & Position);
	//ö�ٹ㳡
	CGlobalPlazaItem * EnumPlazaItem(POSITION & Position);
	//ö�ٷ���
	CGlobalServerItem * EnumServerItem(POSITION & Position);
	//ö������
	CGlobalChatServerItem * EnumChatItem(POSITION & Position);

	//��������
private:
	//�����û�
	CGlobalUserItem * CreateGlobalUserItem();
	//���������û�
	CGlobalChatUserItem * CreateGlobalChatUserItem();
	//�����㳡
	CGlobalPlazaItem * CreateGlobalPlazaItem();
	//��������
	CGlobalServerItem * CreateGlobalServerItem();
	//��������
	CGlobalChatServerItem * CreateGlobalChatItem();
	//���������û�
	CGlobalDummyItem * CreateGlobalDummyItem();

	//�ͷź���
private:
	//�ͷ��û�
	bool FreeGlobalUserItem(CGlobalUserItem * pGlobalUserItem);
	//�ͷ������û�
	bool FreeGlobalChatUserItem(CGlobalChatUserItem * pGlobalUserItem);
	//�ͷŹ㳡
	bool FreeGlobalPlazaItem(CGlobalPlazaItem * pGlobalPlazaItem);
	//�ͷŷ���
	bool FreeGlobalServerItem(CGlobalServerItem * pGlobalServerItem);
	//�ͷ�����
	bool FreeGlobalChatItem(CGlobalChatServerItem * pGlobalChatItem);
	//�ͷ������û�
	bool FreeGlobalDummyItem(CGlobalDummyItem * pDummyItem);
};

//////////////////////////////////////////////////////////////////////////////////

#endif