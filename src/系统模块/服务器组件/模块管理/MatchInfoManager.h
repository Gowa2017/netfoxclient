#ifndef MATCH_INFO_MANAGER_HEAD_FILE
#define MATCH_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<tagGameMatchOption *> CGameMatchOptionArray;
typedef CWHArray<tagMatchRewardInfo *> CMatchRewardInfoArray;

//索引定义
typedef CMap<DWORD,DWORD,tagGameMatchOption *,tagGameMatchOption *> CGameMatchOptionMap;
typedef CMap<DWORD,DWORD,tagMatchRewardInfo *,tagMatchRewardInfo *> CMatchRewardInfoMap;

//////////////////////////////////////////////////////////////////////////
//比赛数据
class MODULE_MANAGER_CLASS CMatchInfoBuffer
{
	//变量定义
public:
	CGameMatchOptionArray		m_GameMatchOptionArray;			    //模块数组
	CGameMatchOptionArray		m_GameMatchOptionBuffer;			//模块数组	

	//函数定义
public:
	//构造函数
	CMatchInfoBuffer();
	//析构函数
	virtual ~CMatchInfoBuffer();

	//管理函数
public:
	//重置数据
	bool ResetMatchOption();
	//删除数据
	bool DeleteMatchOption(DWORD dwMatchID);
	//插入数据
	tagGameMatchOption * InsertMatchOption(tagGameMatchOption * pGameMatchOption);

	//信息函数
public:
	//获取数目
	DWORD GetMatchOptionCount();
	//查找数据
	tagGameMatchOption * SearchMatchOption(DWORD dwMatchID);

	//内部函数
private:
	//创建对象
	tagGameMatchOption * CreateMatchOption();
};

//////////////////////////////////////////////////////////////////////////

//奖励数据
class MODULE_MANAGER_CLASS CRewardInfoBuffer
{
	//变量定义
public:
	CMatchRewardInfoArray		m_MatchRewardInfoArray;			    //奖励数组
	CMatchRewardInfoArray		m_MatchRewardInfoBuffer;			//奖励数组	

	//函数定义
public:
	//构造函数
	CRewardInfoBuffer();
	//析构函数
	virtual ~CRewardInfoBuffer();

	//管理函数
public:
	//重置数据
	bool ResetMatchRewardInfo();	
	//删除数据
	bool DeleteMatchRewardInfo(WORD wRankID);
	//插入数据
	tagMatchRewardInfo * InsertMatchRewardInfo(tagMatchRewardInfo & MatchRewardInfo);

	//信息函数
public:
	//获取数目
	DWORD GetMatchRewardCount();
	//查找数据
	tagMatchRewardInfo * SearchMatchReward(WORD wRankID);

	//内部函数
private:
	//创建对象
	tagMatchRewardInfo * CreateMatchRewardInfo();
};

//////////////////////////////////////////////////////////////////////////

//模块信息
class MODULE_MANAGER_CLASS CMatchInfoManager
{
	//函数定义
public:
	//构造函数
	CMatchInfoManager();
	//析构函数
	virtual ~CMatchInfoManager();

	//比赛奖励
public:
	//加载奖励
	WORD LoadGameMatchReward(DWORD dwMatchID,CRewardInfoBuffer & RewardInfoBuffer);

	//比赛管理
public:		
	//加载比赛
	bool LoadGameMatchOption(WORD wKindID,DWORD dwMatchID,tagGameMatchOption & GameMatchResult);
	//加载比赛
	bool LoadGameMatchOption(WORD wKindID,TCHAR szMachineID[LEN_MACHINE_ID],CMatchInfoBuffer & MatchInfoBuffer);	

	//比赛管理
public:
	//删除房间
	bool DeleteGameMatchOption(DWORD dwMatchID);
	//插入房间
	bool InsertGameMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagGameMatchOption & GameMatchResult);
	//修改房间
	bool ModifyGameMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagGameMatchOption & GameMatchResult);

	//内部函数
private:
	//连接数据
	bool ConnectPlatformDB(CDataBaseHelper & PlatformDBModule);
	//连接数据
	bool ConnectGameMatchDB(CDataBaseHelper & GameMatchDBModule);	
	//读取比赛
	bool ReadGameMatchOption(CDataBaseAide & GameMatchDBAide, tagGameMatchOption & GameMatchResult);
};

//////////////////////////////////////////////////////////////////////////

#endif