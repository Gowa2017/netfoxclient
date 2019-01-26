#ifndef ATTEMPER_ENGINE_HEAD_FILE
#define ATTEMPER_ENGINE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"
#include "AsynchronismEngine.h"

//////////////////////////////////////////////////////////////////////////

//调度引擎
class CAttemperEngine : public IAttemperEngine, public IAsynchronismEngineSink,
	public IDataBaseEngineEvent, public ITimerEngineEvent, public ITCPSocketEvent,
	public ITCPNetworkEngineEvent
{
	//组件变量
protected:
	CCriticalSection				m_CriticalLocker;					//缓冲锁定
	CAsynchronismEngine				m_AsynchronismEngine;				//异步引擎

	//接口变量
protected:
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络接口
	IAttemperEngineSink	*			m_pIAttemperEngineSink;				//钩子接口

	//辅助变量
protected:
	BYTE							m_cbBuffer[MAX_ASYNCHRONISM_DATA];	//临时对象

	//函数定义
public:
	//构造函数
	CAttemperEngine();
	//析构函数
	virtual ~CAttemperEngine();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//服务接口
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();

	//配置接口
public:
	//网络接口
	virtual bool SetNetworkEngine(IUnknownEx * pIUnknownEx);
	//回调接口
	virtual bool SetAttemperEngineSink(IUnknownEx * pIUnknownEx);

	//控制事件
public:
	//自定事件
	virtual bool OnEventCustom(WORD wRequestID, VOID * pData, WORD wDataSize);
	//控制事件
	virtual bool OnEventControl(WORD wControlID, VOID * pData, WORD wDataSize);

	//内核事件
public:
	//定时器事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//数据库结果
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//连接事件
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwSocketID, DWORD dwClientAddr);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwSocketID, DWORD dwClientAddr, DWORD dwActiveTime);
	//读取事件
	virtual bool OnEventTCPNetworkRead(DWORD dwSocketID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//异步接口
public:
	//启动事件
	virtual bool OnAsynchronismEngineStart();
	//停止事件
	virtual bool OnAsynchronismEngineConclude();
	//异步数据
	virtual bool OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif