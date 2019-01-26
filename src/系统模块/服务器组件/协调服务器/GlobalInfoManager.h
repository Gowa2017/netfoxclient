#ifndef GLOBAL_USER_MANAGER_HEAD_FILE
#define GLOBAL_USER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "vector"
//////////////////////////////////////////////////////////////////////////////////

//类说明
class CGlobalUserItem;
class CGlobalPlazaItem;
class CGlobalServerItem;
class CGlobalChatServerItem;
class CGlobalChatUserItem;
class CGlobalDummyItem;

//数组定义
typedef CWHArray<CGlobalUserItem *> CGlobalUserItemArray;
typedef CWHArray<CGlobalChatUserItem *> CGlobalChatUserItemArray;
typedef CWHArray<CGlobalPlazaItem *> CGlobalPlazaItemArray;
typedef CWHArray<CGlobalServerItem *> CGlobalServerItemArray;
typedef CWHArray<CGlobalChatServerItem*> CGlobalChatServerItemArray;

//索引定义
typedef CMap<DWORD,DWORD,CGlobalUserItem *,CGlobalUserItem *> CMapUserID;
typedef CMap<DWORD,DWORD,CGlobalChatUserItem *,CGlobalChatUserItem *> CMapChatUserID;
typedef CMap<DWORD,DWORD,CGlobalUserItem *,CGlobalUserItem *> CMapGameID;
typedef CMap<WORD,WORD,CGlobalPlazaItem *,CGlobalPlazaItem *> CMapPlazaID;
typedef CMap<WORD,WORD,CGlobalServerItem *,CGlobalServerItem *> CMapServerID;
typedef CMap<WORD,WORD,CGlobalChatServerItem *,CGlobalChatServerItem *> CMapChatServerID;
typedef CMap<CString,LPCTSTR,tagPersonalTableInfo*,tagPersonalTableInfo* &> CMapPersonalTableInfo;
typedef CMap<WORD,WORD,CGlobalDummyItem*,CGlobalDummyItem*> CMapDummyInfo;

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//游戏用户信息
struct tagGlobalUserInfo
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称

	//辅助信息
	BYTE							cbGender;							//用户性别
	BYTE							cbMemberOrder;						//会员等级
	BYTE							cbMasterOrder;						//管理等级

	BYTE							cbAndroidUser;						//机器人玩家

	//详细信息
	tagUserInfo						userInfo;							//用户详细信息
};

//聊天用户信息
struct tagGlobalChatUserInfo
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
};

struct tagDummyInfo
{
	WORD							wServerID;							//服务端ID
	DWORD							dwDummyCount;						//虚拟人数
};

//私人房间信息
struct tagPersonalTable
{
	DWORD							dwServerID;							//房间I D
	DWORD							dwTableID;							//桌子I D
};

struct tagServerTableCount
{
	DWORD							dwKindID;							//游戏I D
	DWORD							dwServerID;							//房间I D
	DWORD							dwTableCount;						//桌子数目
};

typedef CWHArray<tagServerTableCount *> CServerTableCountArray;

//////////////////////////////////////////////////////////////////////////////////

//用户子项
class CGlobalUserItem
{
	//友元定义
	friend class CGlobalInfoManager;

	//用户属性
public:
	DWORD							m_dwUserID;							//用户标识
	DWORD							m_dwGameID;							//游戏标识
	TCHAR							m_szNickName[LEN_NICKNAME];			//用户昵称

	//辅助信息
public:
	BYTE							m_cbGender;							//用户性别
	BYTE							m_cbMemberOrder;					//会员等级
	BYTE							m_cbMasterOrder;					//管理等级
	BYTE							m_cbAndroidUser;					//机器人用户
	tagUserInfo						m_UserInfo;							//用户详细信息

	//房间信息
public:
	CGlobalServerItemArray			m_GlobalServerItemArray;			//房间数组

	//链表属性
protected:
	CGlobalUserItem *				m_pNextUserItemPtr;					//对象指针

	//函数定义
protected:
	//构造函数
	CGlobalUserItem();
	//析构函数
	virtual ~CGlobalUserItem();

	//功能函数
public:
	//用户标识
	DWORD GetUserID() { return m_dwUserID; }
	//游戏标识
	DWORD GetGameID() { return m_dwGameID; }
	//用户昵称
	LPCTSTR GetNickName() { return m_szNickName; }
	//机器人
	bool IsAndroid() { return m_cbAndroidUser; }

	//用户信息
public:
	//用户性别
	BYTE GetGender() { return m_cbGender; }
	//会员等级
	BYTE GetMemberOrder() { return m_cbMemberOrder; }
	//会员等级
	BYTE GetMasterOrder() { return m_cbMasterOrder; }
	//详细信息
	tagUserInfo* GetUserInfo() {return &m_UserInfo; }
	//更新状态
	void UpdateStatus(const WORD wTableID, const WORD wChairID, const BYTE cbUserStatus);

	//其他信息
public:
	//枚举房间
	CGlobalServerItem * EnumServerItem(WORD wIndex);
};

//////////////////////////////////////////////////////////////////////////////////

//用户子项
class CGlobalDummyItem
{
	//友元定义
	friend class CGlobalInfoManager;

	//变量定义
public:
	WORD							wServerID;						//ServerID
	tagDummyInfo					m_DummyInfo;					//虚拟人数

	//链表属性
protected:
	CGlobalDummyItem *				m_pNextItemPtr;					//对象指针

	//函数定义
protected:
	//构造函数
	CGlobalDummyItem();
	//析构函数
	virtual ~CGlobalDummyItem();
};


//////////////////////////////////////////////////////////////////////////////////

//广场子项
class CGlobalPlazaItem
{
	//友元定义
	friend class CGlobalInfoManager;

	//变量定义
public:
	WORD							m_wIndex;							//绑定索引
	tagGamePlaza					m_GamePlaza;						//游戏广场

	//索引变量
public:
	CMapUserID						m_MapUserID;						//用户索引

	//链表属性
protected:
	CGlobalPlazaItem *				m_pNextPlazaItemPtr;				//对象指针

	//函数定义
protected:
	//构造函数
	CGlobalPlazaItem();
	//析构函数
	virtual ~CGlobalPlazaItem();
};

//////////////////////////////////////////////////////////////////////////////////

//房间子项
class CGlobalServerItem
{
	//友元定义
	friend class CGlobalInfoManager;

	//变量定义
public:
	WORD							m_wIndex;							//绑定索引
	tagGameServer					m_GameServer;						//游戏房间
	tagGameMatch					m_GameMatch;

	//索引变量
public:
	CMapUserID						m_MapUserID;						//用户索引

	//链表属性
protected:
	CGlobalServerItem *				m_pNextServerItemPtr;				//对象指针

	//函数定义
protected:
	//构造函数
	CGlobalServerItem();
	//析构函数
	virtual ~CGlobalServerItem();

	//功能函数
public:
	//绑定索引
	WORD GetIndex() { return m_wIndex; }
	//获取类型
	WORD GetKindID() { return m_GameServer.wKindID; }
	//获取房间
	WORD GetServerID() { return m_GameServer.wServerID; }
	//用户数目
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }
	//机器人数目
	DWORD GetAndroidUserItemCount();
	//真实人数目
	DWORD GetRealUserItemCount();
	//桌子数目
	WORD GetTabelCount() { return m_GameServer.wTableCount; }
	//比赛房间
	bool IsMatchServer() { return (m_GameMatch.wServerID==m_GameServer.wServerID) && m_GameMatch.dwMatchID!=0; }

	//查找函数
public:
	//寻找用户
	CGlobalUserItem * SearchUserItem(DWORD dwUserID);
};

//////////////////////////////////////////////////////////////////////////////////

//聊天子项
class CGlobalChatServerItem
{
	//友元定义
	friend class CGlobalInfoManager;

	//变量定义
public:
	WORD							m_wIndex;							//绑定索引
	tagChatServer					m_ChatServer;						//聊天服务器信息

	//索引变量
public:
	CMapChatUserID						m_MapUserID;					//用户索引

	//链表属性
protected:
	CGlobalChatServerItem *				m_pNextChatServerPtr;				//对象指针

	//函数定义
protected:
	//构造函数
	CGlobalChatServerItem();
	//析构函数
	virtual ~CGlobalChatServerItem();

	//功能函数
public:
	//绑定索引
	WORD GetIndex() { return m_wIndex; }
	//用户数目
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }

	//查找函数
public:
	//寻找用户
	CGlobalChatUserItem * SearchChatUserItem(DWORD dwUserID);
};

//////////////////////////////////////////////////////////////////////////////////

//用户子项
class CGlobalChatUserItem
{
	//友元定义
	friend class CGlobalInfoManager;

	//用户属性
public:
	DWORD							m_dwUserID;							//用户标识
	TCHAR							m_szNickName[LEN_NICKNAME];			//用户昵称

	//房间信息
public:
	CGlobalChatServerItemArray			m_GlobalServerItemArray;		//聊天服务器数组

	//链表属性
protected:
	CGlobalChatUserItem *				m_pNextUserItemPtr;					//对象指针

	//函数定义
protected:
	//构造函数
	CGlobalChatUserItem();
	//析构函数
	virtual ~CGlobalChatUserItem();

	//功能函数
public:
	//用户标识
	DWORD GetUserID() { return m_dwUserID; }
	//用户昵称
	LPCTSTR GetNickName() { return m_szNickName; }
};

//////////////////////////////////////////////////////////////////////////////////

//全局信息
class CGlobalInfoManager
{
	//索引变量
protected:
	CMapUserID						m_MapUserID;						//游戏用户标识
	CMapGameID						m_MapGameID;						//标识索引
	CMapStringToPtr					m_MapNickName;						//昵称索引


	CMapChatUserID					m_MapChatUserID;					//聊天用户标识
	CMapDummyInfo					m_MapDummyInfo;						//虚拟人数汇总
	CMapPlazaID						m_MapPlazaID;						//广场服务端标识
	CMapServerID					m_MapServerID;						//房间服务端标识
	CMapChatServerID				m_MapChatID;						//聊天服务端标识

	//++++++++++++++++++++++++++++++约战房间
	CServerTableCountArray			m_ServerTableCountArray;//房间空余桌子数量
	CMapPersonalTableInfo			m_MapPersonalTableInfo;				//私人房间信息
	std::vector<tagPersonalTableInfo>			m_VecDissumePersonalTableInfo;				//被解散的私人房间信息
	std::vector<CMD_CS_S_RegisterPersonal> m_vecPersonalRoomMaxCreate;				//私人房可以创建的最大房间数
	//辅助索引
protected:


	//存储变量
protected:
	CGlobalUserItem *				m_pGlobalUserItem;					//用户存储
	CGlobalChatUserItem *			m_pGlobalChatUserItem;				//聊天用户存储
	CGlobalPlazaItem *				m_pGlobalPlazaItem;					//广场存储
	CGlobalServerItem *				m_pGlobalServerItem;				//房间存储
	CGlobalChatServerItem *			m_pGlobalChatServerItem;			//聊天服务器存储
	CGlobalDummyItem *				m_pGlobalDummyItem;					//虚拟用户

	//函数定义
public:
	//构造函数
	CGlobalInfoManager();
	//析构函数
	virtual ~CGlobalInfoManager();

	//管理函数
public:
	//重置数据
	VOID ResetData();
	//用户数目
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }
	//大厅数目
	DWORD GetPlazaItemCount() { return (DWORD)m_MapPlazaID.GetCount(); }
	//房间数目
	DWORD GetServerItemCount() { return (DWORD)m_MapServerID.GetCount(); }
	//聊天数目
	DWORD GetChatItemCount() { return (DWORD)m_MapChatID.GetCount(); }

	//私人房间
public:
	//查找桌子
	tagPersonalTableInfo* SearchTableByID(LPCTSTR lpszTableID);
	//查找桌子
	tagPersonalTableInfo* SearchTableByTableIDAndServerID(DWORD dwServerID, DWORD dwTableID);
	//添加桌子
	bool AddFreeServerTable(DWORD dwServerID);
	//添加桌子
	bool AddServerTable(CString strServerID, tagPersonalTableInfo PersonalTable);
	//移除桌子
	bool RemoveFreeServerTable(DWORD dwServerID);
	//移除桌子
	bool RemoveServerTable(DWORD dwServerID, DWORD dwTableID);
	//移除桌子
	bool RemoveServerTable(DWORD dwServerID);
	//获取房间
	DWORD GetFreeServer(DWORD dwUserID, DWORD dwKindID, BYTE cbIsJoinGame);
	//生成房间ID
	VOID RandServerID(LPTSTR pszServerID, WORD wMaxCount);

	//获取房主创建的所有房间
	VOID GetHostCreatePersonalRoom(tagHostCreatRoomInfo & HostCreatRoomInfo);

	//获取房主创建的房间的数量
	INT GetHostCreatePersonalRoomCount(DWORD dwUserID);
	//添加桌子
	bool AddPersonalMaxCreate(CMD_CS_S_RegisterPersonal RegisterPersonal);
	//是否可以再创建房间
	bool CanCreatePersonalRoom(DWORD dwKindID,  DWORD dwUserID);

	//游戏用户管理
public:
	//删除用户
	bool DeleteUserItem(DWORD dwUserID, WORD wServerID);
	//激活用户
	bool ActiveUserItem(tagGlobalUserInfo & GlobalUserInfo, WORD wServerID);

	//聊天用户管理
public:
	//删除聊天用户
	bool DeleteChatUserItem(DWORD dwUserID, WORD wServerID);
	//激活用户
	bool ActiveChatUserItem(tagGlobalChatUserInfo & GlobalUserInfo, WORD wServerID);

	//虚拟用户管理
public:
	//删除虚拟用户
	bool DeleteDummyItem(WORD wServerID);
	//清楚所有虚拟用户
	bool ClearDummyItem();
	//激活虚拟用户
	bool ActiveDummyItem(tagDummyInfo & DummyInfo, WORD wServerID);

	//广场管理
public:
	//删除广场
	bool DeletePlazaItem(WORD wPlazaID);
	//激活广场
	bool ActivePlazaItem(WORD wBindIndex, tagGamePlaza & GamePlaza);

	//房间管理
public:
	//删除房间
	bool DeleteServerItem(WORD wServerID);
	//激活房间
	bool ActiveServerItem(WORD wBindIndex, tagGameServer & GameServer);

	//聊天管理
public:
	//删除聊天
	bool DeleteChatItem(WORD wChatID);
	//激活聊天
	bool ActiveChatItem(WORD wBindIndex, tagChatServer & ChatServer);

	//服务查找
public:
	//寻找广场
	CGlobalPlazaItem * SearchPlazaItem(WORD wPlazaID);
	//寻找房间
	CGlobalServerItem * SearchServerItem(WORD wServerID);
	//寻找聊天
	CGlobalChatServerItem * SearchChatServerItem(WORD wChatID);
	//寻找虚拟人数
	CGlobalDummyItem * SearchDummyItem(WORD wServerID);

	//用户查找
public:
	//寻找用户
	CGlobalUserItem * SearchUserItemByUserID(DWORD dwUserID);
	//寻找用户
	CGlobalChatUserItem * SearchChatUserItemByUserID(DWORD dwUserID);
	//寻找用户
	CGlobalUserItem * SearchUserItemByGameID(DWORD dwGameID);
	//寻找用户
	CGlobalUserItem * SearchUserItemByNickName(LPCTSTR pszNickName);

	//枚举函数
public:
	//枚举用户
	CGlobalUserItem * EnumUserItem(POSITION & Position);
	//枚举用户
	CGlobalChatUserItem * EnumChatUserItem(POSITION & Position);
	//枚举广场
	CGlobalPlazaItem * EnumPlazaItem(POSITION & Position);
	//枚举房间
	CGlobalServerItem * EnumServerItem(POSITION & Position);
	//枚举聊天
	CGlobalChatServerItem * EnumChatItem(POSITION & Position);

	//创建函数
private:
	//创建用户
	CGlobalUserItem * CreateGlobalUserItem();
	//创建聊天用户
	CGlobalChatUserItem * CreateGlobalChatUserItem();
	//创建广场
	CGlobalPlazaItem * CreateGlobalPlazaItem();
	//创建房间
	CGlobalServerItem * CreateGlobalServerItem();
	//创建聊天
	CGlobalChatServerItem * CreateGlobalChatItem();
	//创建虚拟用户
	CGlobalDummyItem * CreateGlobalDummyItem();

	//释放函数
private:
	//释放用户
	bool FreeGlobalUserItem(CGlobalUserItem * pGlobalUserItem);
	//释放聊天用户
	bool FreeGlobalChatUserItem(CGlobalChatUserItem * pGlobalUserItem);
	//释放广场
	bool FreeGlobalPlazaItem(CGlobalPlazaItem * pGlobalPlazaItem);
	//释放房间
	bool FreeGlobalServerItem(CGlobalServerItem * pGlobalServerItem);
	//释放聊天
	bool FreeGlobalChatItem(CGlobalChatServerItem * pGlobalChatItem);
	//释放虚拟用户
	bool FreeGlobalDummyItem(CGlobalDummyItem * pDummyItem);
};

//////////////////////////////////////////////////////////////////////////////////

#endif