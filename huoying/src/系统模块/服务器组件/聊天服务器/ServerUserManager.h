#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once
#include <math.h>
#include "AfxTempl.h"

#define PI                      3.1415926		//π常量
#define EARTH_RADIUS            6378.137        //地球半径
//经度纬度
class Cltladistance
{
public:
	Cltladistance();
	virtual ~Cltladistance();

public:
	//获取弧度
    inline double GetRadian(double d);
	//四舍五入
	inline DWORD GetRound(double number);
	//获取距离
	inline double GetDistance(double lat1, double lng1, double lat2, double lng2);
};
//////////////////////////////////////////////////////////////////////////////////
//用户类型
enum enUserItemKind
{
	enNoneKind=0,
	enLocalKind,
	enRemoteKind,
};

//本地用户
struct tagInsertLocalUserInfo
{
	DWORD								dwSocketID;						//连接标识
	DWORD								dwLogonTime;					//登录时间
};

//////////////////////////////////////////////////////////////////////////////////
//用户信息
class CServerUserItem
{
	//友元定义
	friend class CServerUserManager;

	//变量定义
protected:
	enUserItemKind					m_enUserKind;						//用户类型

	//函数定义
protected:
	//构造函数
	CServerUserItem(enUserItemKind enUserKind);
	//析构函数
	virtual ~CServerUserItem();

	//用户属性
public:
	//用户标识
	virtual DWORD GetUserID()=NULL;

	//在线信息
public:
	//修改状态
	virtual VOID SetMainStatus(BYTE cbMainStatus)=NULL;
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID)=NULL;
	//用户状态
	virtual BYTE GetMainStatus()=NULL;
	//用户状态
	virtual BYTE GetGameStatus()=NULL;
	//房间标识
	virtual WORD GetServerID()=NULL;
	//桌子标识
	virtual WORD GetTableID()=NULL;
	//连接地址
	virtual DWORD GetClientAddr()=NULL;

	//效验接口
public:
	//对比帐号
	virtual bool ContrastNickName(LPCTSTR pszNickName) = NULL;
	//对比密码
	virtual bool ContrastLogonPass(LPCTSTR pszPassword) = NULL;
	//对比帐号
	virtual bool ContrastAccounts(LPCTSTR pszAccounts) = NULL;
	//辅助变量
private:
	//重置数据
	virtual VOID ResetUserItem()=NULL;

	//辅助变量
public:
	//获取类型
	enUserItemKind GetUserItemKind() { return m_enUserKind; }
};

//////////////////////////////////////////////////////////////////////////////////

//用户信息
class CLocalUserItem : public CServerUserItem
{
	//友元定义
	friend class CServerUserManager;

	//属性变量
protected:
	tagFriendUserInfo				m_UserInfo;							//用户信息
	DWORD							m_dwSocketID;						//连接标识
	
	//游戏信息
protected:
	WORD							m_wServerID;						//房间标识
	WORD							m_wTableID;							//桌子标识

	//辅助变量
protected:
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//用户密码
	DWORD							m_dwLogonTime;						//登录时间
	bool							m_bLogonLock;						//登录锁

	//函数定义
protected:
	//构造函数
	CLocalUserItem();
	//析构函数
	virtual ~CLocalUserItem();

	//用户信息
public:
	//用户信息
	virtual tagFriendUserInfo * GetUserInfo() { return &m_UserInfo; }

	//属性信息
public:
	//用户性别
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//用户标识
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//用户标识
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//用户昵称
	virtual LPCTSTR GetNickName() { return m_UserInfo.szNickName; }

	//在线信息
public:
	//修改状态
	virtual VOID SetMainStatus(BYTE cbMainStatus) { m_UserInfo.cbMainStatus=cbMainStatus; }
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus,WORD wServerID,WORD wTableID);
	//连接标识
	virtual DWORD GetSocketID() { return m_dwSocketID; }
	//获取登录锁状态
	virtual bool GetLogonLock() { return m_bLogonLock;}
	//房间标识
	virtual WORD GetServerID() { return m_wServerID; }
	//桌子标识
	virtual WORD GetTableID() { return m_wTableID; }	
	//用户状态
	virtual BYTE GetMainStatus() { return m_UserInfo.cbMainStatus; }
	//用户状态
	virtual BYTE GetGameStatus() { return m_UserInfo.cbGameStatus; }	
	//连接地址
	virtual DWORD GetClientAddr() { return m_UserInfo.dwClientAddr; }

	//效验接口
public:
	//对比帐号
	virtual bool ContrastNickName(LPCTSTR pszNickName);
	//对比密码
	virtual bool ContrastLogonPass(LPCTSTR pszPassword);
	//对比帐号
	virtual bool ContrastAccounts(LPCTSTR pszAccounts);

	//辅助函数
public:
	//修改密码
	VOID ModifyLogonPassword(LPCTSTR pszPassword);
	//设置标识
	virtual VOID SetSocketID(DWORD dwSocketID) { m_dwSocketID=dwSocketID; }
	//设置登录锁状态
	virtual void SetLogonLock(bool lock) {m_bLogonLock = lock;}
	
	//登录信息
public:
	//登录时间
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//设置时间
	virtual VOID SetLogonTime(DWORD dwLogonTime) { m_dwLogonTime=dwLogonTime; }

	//辅助函数
private:	
	//重置数据
	virtual VOID ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////////////

//用户索引类
typedef CWHArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

typedef CWHArray<tagNearUserInfo *> CNearUserInfoItemArray;
typedef CMap<DWORD,DWORD,tagNearUserInfo *,tagNearUserInfo *> CNearUserInfoItemMap;

//附近用户类
class CNearUserManager
{
	//用户变量
protected:
	CNearUserInfoItemArray				m_UserItemArray;					//用户数组
	//函数定义
public:
	//构造函数
	CNearUserManager();
	//析构函数
	virtual ~CNearUserManager();
public:
	//枚举用户
	virtual tagNearUserInfo * EnumUserItem(DWORD dwEnumIndex);
	//插入用户
	virtual bool InsertNearUserItem(tagNearUserInfo & NearUserItem);
	//在线人数
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }
};

//用户管理类
class CServerUserManager
{
	//用户变量
protected:
	CServerUserItemMap				m_UserIDMap;						//用户索引
	CServerUserItemArray			m_UserItemArray;					//用户数组
	CServerUserItemArray			m_UserItemStore;					//存储用户
	Cltladistance					m_ltladistance;						//获取距离

	//函数定义
public:
	//构造函数
	CServerUserManager();
	//析构函数
	virtual ~CServerUserManager();

	//查找函数
public:
	//枚举用户
	virtual CServerUserItem * EnumUserItem(WORD wEnumIndex);
	//查找用户
	virtual CServerUserItem * SearchUserItem(DWORD dwUserID);
	//查找用户
	virtual CServerUserItem * SearchUserItem(LPCTSTR pszAccounts);

	//统计函数
public:
	//在线人数
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }

	//管理函数
public:
	//删除用户
	virtual bool DeleteUserItem();
	//删除用户
	virtual bool DeleteUserItem(DWORD dwUserID);
	//插入用户
	virtual bool InsertLocalUserItem(tagInsertLocalUserInfo & InsertLocalUserInfo,tagFriendUserInfo & UserInfo,LPCTSTR szPassword);

	//附近的人
public:
	//附近的人
	virtual DWORD GetNearUserItem(DWORD dwUserID,CNearUserManager & NearUserManager);
	//查询附近
	virtual DWORD QueryNearUserItem(DWORD dwUserID,DWORD dwNearUserID,CNearUserManager & NearUserManager);
};

//////////////////////////////////////////////////////////////////////////////////

//好友分组
class CUserFriendGroup
{
	//友元定义
	friend class CFriendGroupManager;

	//变量定义
protected:
	DWORD										m_dwOwnerUserID;				//用户标识
	WORD										m_wGroupContent;				//分组容量
	WORD										m_wFriendCount;					//好友数目

protected:
	tagServerFriendInfo *						m_pFriendInfo;					//用户信息	

	//函数定义
public:
	//构造函数
	CUserFriendGroup();
	//析构函数
	virtual ~CUserFriendGroup();

	//辅助函数
public:
	//设置标识
	VOID SetOwnerUserID(DWORD dwUserID) { m_dwOwnerUserID=dwUserID; }
	//获取标识
	DWORD GetOwnerUserID() { return m_dwOwnerUserID; }
	//获取容量
	WORD GetGroupContent() { return m_wGroupContent; }
	//获取数目
	WORD GetFriendCount() { return m_wFriendCount; }

	//功能函数
public:
	//初始化分组
	bool InitFriendGroup(WORD wGroupContent);
	//添加好友
	bool AppendFriendInfo(tagServerFriendInfo & FriendInfo);
	//移除好友
	bool RemoveFriendInfo(DWORD dwUserID);
	//查找好友
	tagServerFriendInfo * SearchFriendItem(DWORD dwUserID);
	//枚举好友
	tagServerFriendInfo * EnumFriendItem(WORD wEnumIndex);
	//重置分组
	VOID ResetFriendGroup();


	//辅助变量
private:
	//分配容量
	tagServerFriendInfo * AllocateContent(WORD wGroupContent);
	//扩展容量
	bool ExtendGroupContent(WORD wExtendCount);	
};

//////////////////////////////////////////////////////////////////////////////////

//用户索引类
typedef CWHArray<CUserFriendGroup *> CUserFriendGroupArray;
typedef CMap<DWORD,DWORD,CUserFriendGroup *,CUserFriendGroup *> CUserFriendGroupMap;

//好友管理类
class CFriendGroupManager
{
	//用户变量
protected:
	CUserFriendGroupMap				m_UserIDMap;						//用户索引
	CUserFriendGroupArray			m_UserFriendGroupArray;				//用户数组
	CUserFriendGroupArray			m_UserFriendGroupStore;				//存储用户

	//函数定义
public:
	//构造函数
	CFriendGroupManager();
	//析构函数
	virtual ~CFriendGroupManager();

	//查找函数
public:
	//枚举用户
	virtual CUserFriendGroup * EnumGroupItem(WORD wEnumIndex);
	//查找用户
	virtual CUserFriendGroup * SearchGroupItem(DWORD dwUserID);

	//管理函数
public:
	//删除用户
	virtual bool DeleteFriendGroup();
	//删除用户
	virtual bool DeleteFriendGroup(DWORD dwUserID);
	//插入好友
	virtual bool InsertFriendGroup(DWORD dwUserID,CUserFriendGroup * pUserFriendGroup);

	//辅助函数
public:
	//获取分组
	CUserFriendGroup * ActiveFriendGroup(WORD wFriendCount);
};

//////////////////////////////////////////////////////////////////////////////////

#endif