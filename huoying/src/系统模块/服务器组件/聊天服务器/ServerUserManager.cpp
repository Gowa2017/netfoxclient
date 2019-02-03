#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////////////

Cltladistance::Cltladistance()
{

}

Cltladistance::~Cltladistance()
{

}

inline double Cltladistance::GetRadian(double d)
{
	return d * PI / 180.0;
}

inline DWORD Cltladistance::GetRound(double number)
{
	return (DWORD)((number > 0.0) ? floor(number + 0.5) : ceil(number - 0.5));
}

inline double Cltladistance::GetDistance(double lat1, double lng1, double lat2, double lng2)
{
	double radLat1 = GetRadian(lat1);
	double radLat2 = GetRadian(lat2);
	double a = radLat1 - radLat2;
	double b = GetRadian(lng1) - GetRadian(lng2);

	double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));

	dst = dst * EARTH_RADIUS;
	dst= GetRound(dst * 10000000) / 10000;
	return dst;
}

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CServerUserItem::CServerUserItem(enUserItemKind enUserKind):m_enUserKind(enUserKind)
{
}

//��������
CServerUserItem::~CServerUserItem()
{
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CLocalUserItem::CLocalUserItem():CServerUserItem(enLocalKind)
{
	//���ñ���
	m_dwSocketID=0L;
	m_dwLogonTime=0;
	m_wServerID = FRIEND_INVALID_SERVERID;
	m_wTableID = FRIEND_INVALID_TABLEID;
	m_bLogonLock = false;

	//��������
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//���Ա���
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));

	return;
}

//��������
CLocalUserItem::~CLocalUserItem()
{
}

//����״̬
VOID  CLocalUserItem::SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID)
{
	//���ñ���
	m_UserInfo.cbGameStatus = cbGameStatus;
	m_wServerID = wServerID;
	m_wTableID = wTableID;
}

//�Ա��ʺ�
bool CLocalUserItem::ContrastNickName(LPCTSTR pszNickName)
{
	//Ч�����
	ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//���ȶԱ�
	INT nContrastLen=lstrlen(pszNickName);
	INT nSourceLen=lstrlen(m_UserInfo.szNickName);

	//�ַ��Ա�
	if (nContrastLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszNickName,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//�Ա�����
bool CLocalUserItem::ContrastLogonPass(LPCTSTR pszPassword)
{
	//Ч�����
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//���ȶԱ�
	INT nTargetLen=lstrlen(pszPassword);
	INT nSourceLen=lstrlen(m_szLogonPass);

	//����Ա�
	if (nTargetLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszPassword,nTargetLen,m_szLogonPass,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//�Ա��ʺ�
bool CLocalUserItem::ContrastAccounts(LPCTSTR pszAccounts)
{
	//Ч�����
	ASSERT(pszAccounts!=NULL);
	if (pszAccounts==NULL) return false;

	//���ȶԱ�
	INT nContrastLen=lstrlen(pszAccounts);
	INT nSourceLen=lstrlen(m_UserInfo.szAccounts);

	//�ַ��Ա�
	if (nContrastLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszAccounts,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//��������
VOID CLocalUserItem::ResetUserItem()
{
	//���ñ���
	m_dwSocketID=0L;

	//��������
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//���Ա���
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));

	return;
}

//�޸�����
VOID CLocalUserItem::ModifyLogonPassword(LPCTSTR pszPassword)
{
	//�����ַ�
	lstrcpyn(m_szLogonPass,pszPassword,CountArray(m_szLogonPass));

	return;
}

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CUserFriendGroup::CUserFriendGroup()
{
	//��������
	m_dwOwnerUserID=NULL;
	m_wFriendCount=0;	
	m_wGroupContent=0;
	m_pFriendInfo=NULL;
}

//��������
CUserFriendGroup::~CUserFriendGroup()
{
	//�ͷ���Դ
	if(m_pFriendInfo!=NULL)	SafeDeleteArray(m_pFriendInfo);
}

//��ʼ������
bool CUserFriendGroup::InitFriendGroup(WORD wGroupContent)
{
	//���ñ���
	m_wGroupContent = wGroupContent;

	//��������
	m_pFriendInfo = AllocateContent(m_wGroupContent);

	return m_pFriendInfo!=NULL;
}

//��Ӻ���
bool CUserFriendGroup::AppendFriendInfo(tagServerFriendInfo & FriendInfo)
{
	//ָ��У��
	ASSERT(m_pFriendInfo!=NULL);
	if(m_pFriendInfo==NULL) return false;

	//��������
	CopyMemory((m_pFriendInfo+m_wFriendCount++),&FriendInfo,sizeof(tagServerFriendInfo));

	//��չ����
	if(m_wFriendCount==m_wGroupContent) ExtendGroupContent(m_wFriendCount/10);

	return true;
}

//�Ƴ�����
bool CUserFriendGroup::RemoveFriendInfo(DWORD dwUserID)
{
	//ָ��У��
	ASSERT(m_pFriendInfo!=NULL);
	if(m_pFriendInfo==NULL) return false;

	//��������
	tagServerFriendInfo * pFriendInfo=SearchFriendItem(dwUserID);

	//�ƶ��ڴ�
	if(pFriendInfo!=NULL) 
	{
		--m_wFriendCount;
		if(pFriendInfo!=(tagServerFriendInfo *)(m_pFriendInfo+m_wGroupContent-1))
		{
			pFriendInfo = pFriendInfo++;
		}		
	}

	return true;
}

//���Һ���
tagServerFriendInfo * CUserFriendGroup::SearchFriendItem(DWORD dwUserID)
{
	//��������
	tagServerFriendInfo * pFriendInfo=NULL;

	//���Һ���
	for(WORD wIndex=0;wIndex<m_wFriendCount;++wIndex)
	{
		pFriendInfo = (tagServerFriendInfo *)(m_pFriendInfo+wIndex);
		if(pFriendInfo->dwUserID==dwUserID) return pFriendInfo;
	}

	return NULL;
}

//ö�ٺ���
tagServerFriendInfo * CUserFriendGroup::EnumFriendItem(WORD wEnumIndex)
{
	//����У��
	if(wEnumIndex>=m_wFriendCount) return NULL;

	return (tagServerFriendInfo *)(m_pFriendInfo+wEnumIndex);
}

//��������
tagServerFriendInfo * CUserFriendGroup::AllocateContent(WORD wGroupContent)
{
	//��������
	tagServerFriendInfo * pFriendInfo=NULL;

	//��������
	try
	{
		pFriendInfo = new tagServerFriendInfo[wGroupContent];
		ASSERT(pFriendInfo!=NULL);
		if(pFriendInfo==NULL) throw(TEXT("ϵͳ�ڴ���Դ����,�޷������ڴ�!"));

		//��ʼ���ڴ�
		ZeroMemory(pFriendInfo,wGroupContent*sizeof(tagServerFriendInfo));
	}
	catch(...)
	{
		SafeDeleteArray(pFriendInfo);
		pFriendInfo=NULL;
	}

	return pFriendInfo;
}

//��չ����
bool CUserFriendGroup::ExtendGroupContent(WORD wExtendCount)
{
	//���ñ���
	tagServerFriendInfo * pFriendInfo = AllocateContent(m_wGroupContent+wExtendCount);
	if(pFriendInfo==NULL) return false;

	try
	{
		//�ƶ�����
		MoveMemory(pFriendInfo,m_pFriendInfo,m_wGroupContent*sizeof(tagServerFriendInfo));

		//�ͷ��ڴ�
		SafeDeleteArray(m_pFriendInfo);

		//���ñ���
		m_pFriendInfo=pFriendInfo;
		m_wGroupContent += wExtendCount;
	}
	catch(...)
	{
		return false;
	}

	return true;
}

//���÷���
VOID CUserFriendGroup::ResetFriendGroup()
{
	//���ñ���	
	m_wFriendCount=0;
	m_dwOwnerUserID=0;
	if(m_pFriendInfo!=NULL)
	{
		ZeroMemory(m_pFriendInfo,m_wGroupContent*sizeof(tagServerFriendInfo));
	}
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerUserManager::CServerUserManager()
{
	//��������
	m_UserIDMap.InitHashTable(PRIME_PLATFORM_USER);

	return;
}

//��������
CServerUserManager::~CServerUserManager()
{
	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemStore[i];
		SafeDelete(pServerUserItem);
	}

	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemArray[i];
		SafeDelete(pServerUserItem);
	}

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//ö���û�
CServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//�����û�
CServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//�����û�
CServerUserItem * CServerUserManager::SearchUserItem(LPCTSTR pszAccounts)
{
	//��������
	CServerUserItem * pServerUserItem=NULL;

	//�����û�
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->ContrastAccounts(pszAccounts)==true) return pServerUserItem;
	}

	return NULL;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem()
{
	//�洢����
	m_UserItemStore.Append(m_UserItemArray);

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem(DWORD dwUserID)
{
	//��������
	CServerUserItem * pTempUserItem=NULL;

	//Ѱ�Ҷ���
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//��ȡ�û�
		pTempUserItem=m_UserItemArray[i];
		if (pTempUserItem->GetUserID()!=dwUserID) continue;

		//���ö���
		pTempUserItem->ResetUserItem();

		//ɾ������
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�����û�
bool CServerUserManager::InsertLocalUserItem(tagInsertLocalUserInfo & InsertLocalUserInfo,tagFriendUserInfo & UserInfo,LPCTSTR pszPassword)
{
	//��������
	CLocalUserItem * pServerUserItem=NULL;

	//��ȡָ��
	if (m_UserItemStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserItemStore.GetCount()-1;
		pServerUserItem=(CLocalUserItem *)m_UserItemStore[nItemPostion];
		m_UserItemStore.RemoveAt(nItemPostion);
	}
	else
	{
		try
		{
			pServerUserItem=new CLocalUserItem;
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//���Ա���
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));

	//��������
	lstrcpyn(pServerUserItem->m_szLogonPass,pszPassword,CountArray(pServerUserItem->m_szLogonPass));

	//�����û�
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	//���ñ���
	pServerUserItem->SetLogonTime(InsertLocalUserInfo.dwLogonTime);
	pServerUserItem->SetSocketID(InsertLocalUserInfo.dwSocketID);

	return true;
}

DWORD CServerUserManager::GetNearUserItem(DWORD dwUserID,CNearUserManager & NearUserManager)
{
	DWORD dwCount =GetUserItemCount();
	DWORD dwCurrCount = 0;
	CLocalUserItem *pUserItem = (CLocalUserItem *)SearchUserItem(dwUserID);
	tagNearUserInfo NearUserInfo;
	for (WORD i = 0;i<dwCount;i++)
	{

		CLocalUserItem * pServerUserItem = (CLocalUserItem *)	EnumUserItem(i);
		tagFriendUserInfo *pUserInfo = &(pServerUserItem->m_UserInfo);
		if (pUserInfo->cbCoordinate ==1 && pUserItem->m_UserInfo.cbCoordinate == 1)
		{
			dwCurrCount++;
			ZeroMemory(&NearUserInfo,sizeof(NearUserInfo));
			NearUserInfo.dwUserID = pUserInfo->dwUserID;
			NearUserInfo.dwGameID = pUserInfo->dwGameID;
			NearUserInfo.dwFaceID = pUserInfo->dwFaceID;
			NearUserInfo.dwCustomID = pUserInfo->dwCustomID;
			NearUserInfo.cbGender = pUserInfo->cbGender;
			NearUserInfo.wMemberOrder = pUserInfo->wMemberOrder;
			NearUserInfo.wGrowLevel = pUserInfo->wGrowLevel;
			NearUserInfo.dLongitude = pUserInfo->dLongitude;
			NearUserInfo.dLatitude = pUserInfo->dLatitude;
			NearUserInfo.dwClientAddr = pUserInfo->dwClientAddr;
			lstrcpyn(NearUserInfo.szNickName,pUserInfo->szNickName,CountArray(NearUserInfo.szNickName));
			lstrcpyn(NearUserInfo.szUnderWrite,pUserInfo->szUnderWrite,CountArray(NearUserInfo.szUnderWrite));
			NearUserInfo.dwDistance =(DWORD)(m_ltladistance.GetDistance(NearUserInfo.dLatitude,NearUserInfo.dLongitude,pUserItem->m_UserInfo.dLatitude,pUserItem->m_UserInfo.dLongitude));
			NearUserManager.InsertNearUserItem(NearUserInfo);
		}
		
	}

	return dwCurrCount;
}

DWORD CServerUserManager::QueryNearUserItem(DWORD dwUserID,DWORD dwNearUserID,CNearUserManager & NearUserManager)
{
	DWORD dwCount = 0;
	CLocalUserItem *pUserItem = (CLocalUserItem *)SearchUserItem(dwUserID);
	CLocalUserItem *pNearUserItem = (CLocalUserItem *)SearchUserItem(dwNearUserID);
	if (pNearUserItem != NULL)
	{
		tagFriendUserInfo *pUserInfo = &(pNearUserItem->m_UserInfo);
		{
			tagNearUserInfo NearUserInfo;
			ZeroMemory(&NearUserInfo,sizeof(NearUserInfo));
			NearUserInfo.dwUserID = pUserInfo->dwUserID;
			NearUserInfo.dwGameID = pUserInfo->dwGameID;
			NearUserInfo.dwFaceID = pUserInfo->dwFaceID;
			NearUserInfo.dwCustomID = pUserInfo->dwCustomID;
			NearUserInfo.cbGender = pUserInfo->cbGender;
			NearUserInfo.wMemberOrder = pUserInfo->wMemberOrder;
			NearUserInfo.wGrowLevel = pUserInfo->wGrowLevel;
			NearUserInfo.dLongitude = pUserInfo->dLongitude;
			NearUserInfo.dLatitude = pUserInfo->dLatitude;
			NearUserInfo.dwClientAddr = pUserInfo->dwClientAddr;
			lstrcpyn(NearUserInfo.szNickName,pUserInfo->szNickName,CountArray(NearUserInfo.szNickName));
			lstrcpyn(NearUserInfo.szUnderWrite,pUserInfo->szUnderWrite,CountArray(NearUserInfo.szUnderWrite));
			if (pUserItem->m_UserInfo.cbCoordinate == 1 && pUserInfo->cbCoordinate ==1)
			{
				NearUserInfo.dwDistance =(DWORD)(m_ltladistance.GetDistance(NearUserInfo.dLatitude,NearUserInfo.dLongitude,pUserItem->m_UserInfo.dLatitude,pUserItem->m_UserInfo.dLongitude));
			}
			NearUserManager.InsertNearUserItem(NearUserInfo);
			dwCount=NearUserManager.GetUserItemCount();
		}

	}

	return dwCount;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CFriendGroupManager::CFriendGroupManager()
{
	//��������
	m_UserIDMap.InitHashTable(PRIME_PLATFORM_USER);
}

//��������
CFriendGroupManager::~CFriendGroupManager()
{	
	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserFriendGroupArray.GetCount();i++)
	{
		CUserFriendGroup * pUserFriendGroup = m_UserFriendGroupArray[i];
		SafeDelete(pUserFriendGroup);
	}

	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserFriendGroupStore.GetCount();i++)
	{
		CUserFriendGroup * pUserFriendGroup = m_UserFriendGroupStore[i];
		SafeDelete(pUserFriendGroup);
	}

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserFriendGroupStore.RemoveAll();
	m_UserFriendGroupArray.RemoveAll();
}

//ö���û�
CUserFriendGroup * CFriendGroupManager::EnumGroupItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserFriendGroupArray.GetCount()) return NULL;
	return m_UserFriendGroupArray[wEnumIndex];
}

//�����û�
CUserFriendGroup * CFriendGroupManager::SearchGroupItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//ɾ���û�
bool CFriendGroupManager::DeleteFriendGroup()
{
	//�洢����
	m_UserFriendGroupStore.Append(m_UserFriendGroupArray);

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserFriendGroupArray.RemoveAll();

	return true;
}

//ɾ���û�
bool CFriendGroupManager::DeleteFriendGroup(DWORD dwUserID)
{
	//��������
	CUserFriendGroup * pTempFriendGroup=NULL;

	//Ѱ�Ҷ���
	for (INT_PTR i=0;i<m_UserFriendGroupArray.GetCount();i++)
	{
		//��ȡ�û�
		pTempFriendGroup=m_UserFriendGroupArray[i];
		if (pTempFriendGroup==NULL || pTempFriendGroup->GetOwnerUserID()!=dwUserID) continue;

		//���ö���
		pTempFriendGroup->ResetFriendGroup();

		//ɾ������
		m_UserFriendGroupArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserFriendGroupStore.Add(pTempFriendGroup);

		return true;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�������
bool CFriendGroupManager::InsertFriendGroup(DWORD dwUserID,CUserFriendGroup * pUserFriendGroup)
{
	//���ñ�ʶ
	pUserFriendGroup->SetOwnerUserID(dwUserID);

	//�����û�
	m_UserFriendGroupArray.Add(pUserFriendGroup);
	m_UserIDMap[dwUserID]=pUserFriendGroup;

	return true;
}

//��ȡ����
CUserFriendGroup * CFriendGroupManager::ActiveFriendGroup(WORD wFriendCount)
{
	//��������
	CUserFriendGroup * pUserFriendGroup=NULL;
	WORD wGroupContent = __min(wFriendCount+wFriendCount/10,MAX_FRIEND_COUNT);
	wGroupContent = __max(MIN_FRIEND_CONTENT,wGroupContent);

	//��ȡָ��
	if (m_UserFriendGroupStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserFriendGroupStore.GetCount()-1;
		for(INT_PTR nIndex=nItemPostion;nIndex>=0;--nIndex)
		{
			pUserFriendGroup=(CUserFriendGroup *)m_UserFriendGroupStore[nIndex];
			if(pUserFriendGroup->GetGroupContent()>=wGroupContent)
			{
				pUserFriendGroup->ResetFriendGroup();
				m_UserFriendGroupStore.RemoveAt(nItemPostion);
				break;
			}
		}
	}

	if(pUserFriendGroup==NULL || pUserFriendGroup->GetGroupContent()<wGroupContent)
	{
		try
		{
			pUserFriendGroup=new CUserFriendGroup();
			if(pUserFriendGroup!=NULL) pUserFriendGroup->InitFriendGroup(wGroupContent);
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return pUserFriendGroup;
}
//////////////////////////////////////////////////////////////////////////

CNearUserManager::CNearUserManager()
{

}

CNearUserManager::~CNearUserManager()
{
	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		tagNearUserInfo * pServerUserItem = m_UserItemArray[i];
		SafeDelete(pServerUserItem);
	}

	//ɾ������
	m_UserItemArray.RemoveAll();
}

tagNearUserInfo * CNearUserManager::EnumUserItem(DWORD dwEnumIndex)
{
	if (dwEnumIndex>=(DWORD)m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[dwEnumIndex];
}

bool CNearUserManager::InsertNearUserItem(tagNearUserInfo & NearUserItem)
{
	//��������
	tagNearUserInfo * pNearUserItem=NULL;

	try
	{
		pNearUserItem=new tagNearUserInfo;
	}
	catch (...)
	{
		ASSERT(FALSE);
		return false;
	}

	//���Ա���
	CopyMemory(pNearUserItem,&NearUserItem,sizeof(NearUserItem));

	//�����û�
	DWORD dwCount =GetUserItemCount();
	bool bInsert = false;
	for (WORD i = 0;i<dwCount;i++)
	{
		tagNearUserInfo * pNextNearUserInfo =	EnumUserItem(i);
		if (pNextNearUserInfo->dwDistance>pNearUserItem->dwDistance)
		{
			m_UserItemArray.InsertAt(i,pNearUserItem);
			bInsert =true;
			break;
		}
	}
	if (!bInsert)
	{
		m_UserItemArray.Add(pNearUserItem);
	}
	return true;
}
