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
//构造函数
CServerUserItem::CServerUserItem(enUserItemKind enUserKind):m_enUserKind(enUserKind)
{
}

//析构函数
CServerUserItem::~CServerUserItem()
{
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CLocalUserItem::CLocalUserItem():CServerUserItem(enLocalKind)
{
	//设置变量
	m_dwSocketID=0L;
	m_dwLogonTime=0;
	m_wServerID = FRIEND_INVALID_SERVERID;
	m_wTableID = FRIEND_INVALID_TABLEID;
	m_bLogonLock = false;

	//辅助变量
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));

	return;
}

//析构函数
CLocalUserItem::~CLocalUserItem()
{
}

//设置状态
VOID  CLocalUserItem::SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID)
{
	//设置变量
	m_UserInfo.cbGameStatus = cbGameStatus;
	m_wServerID = wServerID;
	m_wTableID = wTableID;
}

//对比帐号
bool CLocalUserItem::ContrastNickName(LPCTSTR pszNickName)
{
	//效验参数
	ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//长度对比
	INT nContrastLen=lstrlen(pszNickName);
	INT nSourceLen=lstrlen(m_UserInfo.szNickName);

	//字符对比
	if (nContrastLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszNickName,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//对比密码
bool CLocalUserItem::ContrastLogonPass(LPCTSTR pszPassword)
{
	//效验参数
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//长度对比
	INT nTargetLen=lstrlen(pszPassword);
	INT nSourceLen=lstrlen(m_szLogonPass);

	//密码对比
	if (nTargetLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszPassword,nTargetLen,m_szLogonPass,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//对比帐号
bool CLocalUserItem::ContrastAccounts(LPCTSTR pszAccounts)
{
	//效验参数
	ASSERT(pszAccounts!=NULL);
	if (pszAccounts==NULL) return false;

	//长度对比
	INT nContrastLen=lstrlen(pszAccounts);
	INT nSourceLen=lstrlen(m_UserInfo.szAccounts);

	//字符对比
	if (nContrastLen!=nSourceLen) return false;
	if (CompareString(LOCALE_SYSTEM_DEFAULT,NORM_IGNORECASE,pszAccounts,nContrastLen,m_UserInfo.szNickName,nSourceLen)!=CSTR_EQUAL) return false;

	return true;
}

//重置数据
VOID CLocalUserItem::ResetUserItem()
{
	//设置变量
	m_dwSocketID=0L;

	//辅助变量
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));

	return;
}

//修改密码
VOID CLocalUserItem::ModifyLogonPassword(LPCTSTR pszPassword)
{
	//拷贝字符
	lstrcpyn(m_szLogonPass,pszPassword,CountArray(m_szLogonPass));

	return;
}

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CUserFriendGroup::CUserFriendGroup()
{
	//变量定义
	m_dwOwnerUserID=NULL;
	m_wFriendCount=0;	
	m_wGroupContent=0;
	m_pFriendInfo=NULL;
}

//析构函数
CUserFriendGroup::~CUserFriendGroup()
{
	//释放资源
	if(m_pFriendInfo!=NULL)	SafeDeleteArray(m_pFriendInfo);
}

//初始化分组
bool CUserFriendGroup::InitFriendGroup(WORD wGroupContent)
{
	//设置变量
	m_wGroupContent = wGroupContent;

	//分配容量
	m_pFriendInfo = AllocateContent(m_wGroupContent);

	return m_pFriendInfo!=NULL;
}

//添加好友
bool CUserFriendGroup::AppendFriendInfo(tagServerFriendInfo & FriendInfo)
{
	//指针校验
	ASSERT(m_pFriendInfo!=NULL);
	if(m_pFriendInfo==NULL) return false;

	//拷贝数据
	CopyMemory((m_pFriendInfo+m_wFriendCount++),&FriendInfo,sizeof(tagServerFriendInfo));

	//扩展容量
	if(m_wFriendCount==m_wGroupContent) ExtendGroupContent(m_wFriendCount/10);

	return true;
}

//移除好友
bool CUserFriendGroup::RemoveFriendInfo(DWORD dwUserID)
{
	//指针校验
	ASSERT(m_pFriendInfo!=NULL);
	if(m_pFriendInfo==NULL) return false;

	//变量定义
	tagServerFriendInfo * pFriendInfo=SearchFriendItem(dwUserID);

	//移动内存
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

//查找好友
tagServerFriendInfo * CUserFriendGroup::SearchFriendItem(DWORD dwUserID)
{
	//变量定义
	tagServerFriendInfo * pFriendInfo=NULL;

	//查找好友
	for(WORD wIndex=0;wIndex<m_wFriendCount;++wIndex)
	{
		pFriendInfo = (tagServerFriendInfo *)(m_pFriendInfo+wIndex);
		if(pFriendInfo->dwUserID==dwUserID) return pFriendInfo;
	}

	return NULL;
}

//枚举好友
tagServerFriendInfo * CUserFriendGroup::EnumFriendItem(WORD wEnumIndex)
{
	//参数校验
	if(wEnumIndex>=m_wFriendCount) return NULL;

	return (tagServerFriendInfo *)(m_pFriendInfo+wEnumIndex);
}

//分配容量
tagServerFriendInfo * CUserFriendGroup::AllocateContent(WORD wGroupContent)
{
	//变量定义
	tagServerFriendInfo * pFriendInfo=NULL;

	//分配类型
	try
	{
		pFriendInfo = new tagServerFriendInfo[wGroupContent];
		ASSERT(pFriendInfo!=NULL);
		if(pFriendInfo==NULL) throw(TEXT("系统内存资源不足,无法分配内存!"));

		//初始化内存
		ZeroMemory(pFriendInfo,wGroupContent*sizeof(tagServerFriendInfo));
	}
	catch(...)
	{
		SafeDeleteArray(pFriendInfo);
		pFriendInfo=NULL;
	}

	return pFriendInfo;
}

//扩展容量
bool CUserFriendGroup::ExtendGroupContent(WORD wExtendCount)
{
	//设置变量
	tagServerFriendInfo * pFriendInfo = AllocateContent(m_wGroupContent+wExtendCount);
	if(pFriendInfo==NULL) return false;

	try
	{
		//移动数据
		MoveMemory(pFriendInfo,m_pFriendInfo,m_wGroupContent*sizeof(tagServerFriendInfo));

		//释放内存
		SafeDeleteArray(m_pFriendInfo);

		//设置变量
		m_pFriendInfo=pFriendInfo;
		m_wGroupContent += wExtendCount;
	}
	catch(...)
	{
		return false;
	}

	return true;
}

//重置分组
VOID CUserFriendGroup::ResetFriendGroup()
{
	//设置变量	
	m_wFriendCount=0;
	m_dwOwnerUserID=0;
	if(m_pFriendInfo!=NULL)
	{
		ZeroMemory(m_pFriendInfo,m_wGroupContent*sizeof(tagServerFriendInfo));
	}
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserManager::CServerUserManager()
{
	//设置质数
	m_UserIDMap.InitHashTable(PRIME_PLATFORM_USER);

	return;
}

//析构函数
CServerUserManager::~CServerUserManager()
{
	//释放用户
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemStore[i];
		SafeDelete(pServerUserItem);
	}

	//释放用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		CServerUserItem * pServerUserItem = m_UserItemArray[i];
		SafeDelete(pServerUserItem);
	}

	//删除数据
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//枚举用户
CServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//查找用户
CServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//查找用户
CServerUserItem * CServerUserManager::SearchUserItem(LPCTSTR pszAccounts)
{
	//变量定义
	CServerUserItem * pServerUserItem=NULL;

	//搜索用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->ContrastAccounts(pszAccounts)==true) return pServerUserItem;
	}

	return NULL;
}

//删除用户
bool CServerUserManager::DeleteUserItem()
{
	//存储对象
	m_UserItemStore.Append(m_UserItemArray);

	//删除对象
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//删除用户
bool CServerUserManager::DeleteUserItem(DWORD dwUserID)
{
	//变量定义
	CServerUserItem * pTempUserItem=NULL;

	//寻找对象
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//获取用户
		pTempUserItem=m_UserItemArray[i];
		if (pTempUserItem->GetUserID()!=dwUserID) continue;

		//重置对象
		pTempUserItem->ResetUserItem();

		//删除对象
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//插入用户
bool CServerUserManager::InsertLocalUserItem(tagInsertLocalUserInfo & InsertLocalUserInfo,tagFriendUserInfo & UserInfo,LPCTSTR pszPassword)
{
	//变量定义
	CLocalUserItem * pServerUserItem=NULL;

	//获取指针
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

	//属性变量
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));

	//辅助变量
	lstrcpyn(pServerUserItem->m_szLogonPass,pszPassword,CountArray(pServerUserItem->m_szLogonPass));

	//插入用户
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	//设置变量
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

//构造函数
CFriendGroupManager::CFriendGroupManager()
{
	//设置质数
	m_UserIDMap.InitHashTable(PRIME_PLATFORM_USER);
}

//析构函数
CFriendGroupManager::~CFriendGroupManager()
{	
	//释放用户
	for (INT_PTR i=0;i<m_UserFriendGroupArray.GetCount();i++)
	{
		CUserFriendGroup * pUserFriendGroup = m_UserFriendGroupArray[i];
		SafeDelete(pUserFriendGroup);
	}

	//释放用户
	for (INT_PTR i=0;i<m_UserFriendGroupStore.GetCount();i++)
	{
		CUserFriendGroup * pUserFriendGroup = m_UserFriendGroupStore[i];
		SafeDelete(pUserFriendGroup);
	}

	//删除数据
	m_UserIDMap.RemoveAll();
	m_UserFriendGroupStore.RemoveAll();
	m_UserFriendGroupArray.RemoveAll();
}

//枚举用户
CUserFriendGroup * CFriendGroupManager::EnumGroupItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserFriendGroupArray.GetCount()) return NULL;
	return m_UserFriendGroupArray[wEnumIndex];
}

//查找用户
CUserFriendGroup * CFriendGroupManager::SearchGroupItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//删除用户
bool CFriendGroupManager::DeleteFriendGroup()
{
	//存储对象
	m_UserFriendGroupStore.Append(m_UserFriendGroupArray);

	//删除对象
	m_UserIDMap.RemoveAll();
	m_UserFriendGroupArray.RemoveAll();

	return true;
}

//删除用户
bool CFriendGroupManager::DeleteFriendGroup(DWORD dwUserID)
{
	//变量定义
	CUserFriendGroup * pTempFriendGroup=NULL;

	//寻找对象
	for (INT_PTR i=0;i<m_UserFriendGroupArray.GetCount();i++)
	{
		//获取用户
		pTempFriendGroup=m_UserFriendGroupArray[i];
		if (pTempFriendGroup==NULL || pTempFriendGroup->GetOwnerUserID()!=dwUserID) continue;

		//重置对象
		pTempFriendGroup->ResetFriendGroup();

		//删除对象
		m_UserFriendGroupArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserFriendGroupStore.Add(pTempFriendGroup);

		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//插入好友
bool CFriendGroupManager::InsertFriendGroup(DWORD dwUserID,CUserFriendGroup * pUserFriendGroup)
{
	//设置标识
	pUserFriendGroup->SetOwnerUserID(dwUserID);

	//插入用户
	m_UserFriendGroupArray.Add(pUserFriendGroup);
	m_UserIDMap[dwUserID]=pUserFriendGroup;

	return true;
}

//获取分组
CUserFriendGroup * CFriendGroupManager::ActiveFriendGroup(WORD wFriendCount)
{
	//变量定义
	CUserFriendGroup * pUserFriendGroup=NULL;
	WORD wGroupContent = __min(wFriendCount+wFriendCount/10,MAX_FRIEND_COUNT);
	wGroupContent = __max(MIN_FRIEND_CONTENT,wGroupContent);

	//获取指针
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
	//释放用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		tagNearUserInfo * pServerUserItem = m_UserItemArray[i];
		SafeDelete(pServerUserItem);
	}

	//删除数据
	m_UserItemArray.RemoveAll();
}

tagNearUserInfo * CNearUserManager::EnumUserItem(DWORD dwEnumIndex)
{
	if (dwEnumIndex>=(DWORD)m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[dwEnumIndex];
}

bool CNearUserManager::InsertNearUserItem(tagNearUserInfo & NearUserItem)
{
	//变量定义
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

	//属性变量
	CopyMemory(pNearUserItem,&NearUserItem,sizeof(NearUserItem));

	//插入用户
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
