#ifndef ANDROID_USER_MANAGER_HEAD_FILE
#define ANDROID_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "AndroidUserItem.h"
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////////
//参数扩展
struct tagAndroidParameterEx
{
	//参数变量
	tagAndroidParameter					AndroidParameter;					//机器参数

	//标识变量
	bool								bIsValided;							//有效标识
	bool								bIsLoadAndroid;						//加载机器	
};

///////////////////////////////////////////////////////////////////////////////////////

//数组数组
typedef CWHArray<CAndroidUserItem *> CAndroidUserItemArray;
typedef CWHArray<tagAndroidParameterEx *> CAndroidParameterArray;
typedef CMap<DWORD,DWORD,CAndroidUserItem *, CAndroidUserItem *> CAndroidUserItemMap;
//typedef CMap<DWORD,DWORD,tagAndroidParameter *, tagAndroidParameter *> CAndroidParameterMap;

//////////////////////////////////////////////////////////////////////////////////

//机器人管理
class GAME_SERVICE_CLASS CAndroidUserManager : public IAndroidUserManager
{
	//配置变量
protected:
	WORD							m_wAutoAndroidCount;				//自动数目

	//库存变量
protected:
	WORD							m_wStockCount;						//库存数目
	tagAndroidItemConfig			m_AndroidItemConfig[MAX_ANDROID];	//机器配置

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IServerUserManager *			m_pIServerUserManager;				//用户管理
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//时间配置
	ITCPNetworkEngineEvent *		m_pITCPNetworkEngineEvent;			//事件接口
	IMainServiceFrame *				m_pIMainServiceFrame;				//调度服务

	//服务配置
protected:	
	bool							m_bServiceContinue;					//服务延续
	DWORD							m_dwMinSitInterval;					//休息时间
	DWORD							m_dwMaxSitInterval;					//休息时间	

	//系统配置
protected:
	tagGameParameter *				m_pGameParameter;					//配置参数
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置
	tagGameMatchOption *			m_pGameMatchOption;					//比赛配置

	//用户数组
protected:
	CAndroidUserItemMap				m_AndroidUserItemMap;				//用户索引
	CAndroidParameterArray			m_AndroidParameterArray;			//参数数组
	CAndroidUserItemArray			m_AndroidUserItemArray;				//存储对象
	CAndroidUserItemArray			m_AndroidUserItemBuffer;			//空闲对象

	//函数定义
public:
	//构造函数
	CAndroidUserManager();
	//析构函数
	virtual ~CAndroidUserManager();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();

	//配置接口
public:	
	//配置组件
	virtual bool InitAndroidUser(tagAndroidUserParameter & AndroidUserParameter);
	//移除参数
	virtual bool RemoveAndroidParameter(DWORD dwBatchID);
	//设置参数
	virtual bool AddAndroidParameter(tagAndroidParameter AndroidParameter[], WORD wParameterCount);
	//插入机器
	virtual bool InsertAndroidInfo(tagAndroidAccountsInfo AndroidAccountsInfo[],WORD wAndroidCount,DWORD dwBatchID);

	//管理接口
public:
	//删除机器
	virtual bool DeleteAndroidUserItem(DWORD dwAndroidID,bool bStockRetrieve);
	//查找机器
	virtual IAndroidUserItem * SearchAndroidUserItem(DWORD dwUserID, DWORD dwContextID);
	//创建机器
	virtual IAndroidUserItem * CreateAndroidUserItem(tagAndroidItemConfig & AndroidItemConfig);
	//设置接口
	VOID SetMainServiceFrame(IMainServiceFrame *pIMainServiceFrame);

	//系统事件
public:
	//脉冲事件
	virtual bool OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);

	//状态接口
public:
	//机器数目
	virtual WORD GetAndroidCount() { return (WORD)m_AndroidUserItemMap.GetCount(); }
	//库存数目
	virtual WORD GetAndroidStock() { return m_wStockCount; }
	//加载机器
	virtual bool GetAndroidLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount);
	//用户状况
	virtual WORD GetAndroidUserInfo(tagAndroidUserInfo & AndroidUserInfo, DWORD dwServiceMode);
	//获取房间配置
	virtual tagGameServiceOption* GetGameServiceOption() { return m_pGameServiceOption; }
	//获取游戏属性
	virtual tagGameServiceAttrib* GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//获取比赛配置
	virtual tagGameMatchOption* GetGameMatchOption() { return m_pGameMatchOption; }

	//网络接口
public:
	//发送数据
	virtual bool SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendDataToClient(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendDataToServer(DWORD dwAndroidID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//对象管理
protected:
	//释放对象
	VOID FreeAndroidUserItem(DWORD dwUserID);	
	//获取对象
	CAndroidUserItem * GetAndroidUserItem(WORD wIndex);
	//激活对象
	CAndroidUserItem * ActiveAndroidUserItem(tagAndroidItemConfig & AndroidItemConfig);
	//强制起立
	VOID ForceAndroidUserItemStandup(CAndroidUserItem * pAndroidUserItem);

public:
	//获取参数
	tagAndroidParameterEx * GetAndroidParameter(DWORD dwBatchID);
	//辅助函数
protected:	 
	//获取索引
	WORD GetAndroidParameterIndex(tagAndroidParameterEx * pAndroidParameter);
	//转换时间
	DWORD GetTodayTickCount(SYSTEMTIME & SystemTime);
	//服务时间
	DWORD BatchServiceRemaindTime(tagAndroidParameter * pAndroidParameter,DWORD dwTodayTickCount);
	//等待时间
	DWORD BatchServiceAwaitTime(tagAndroidParameter * pAndroidParameter,DWORD dwTodayTickCount);
};

//////////////////////////////////////////////////////////////////////////////////

#endif