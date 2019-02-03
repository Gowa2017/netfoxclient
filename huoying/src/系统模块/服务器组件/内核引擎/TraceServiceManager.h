#ifndef TRACE_SERVICE_MANAGER_HEAD_FILE
#define TRACE_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////

//事件服务管理
class CTraceServiceManager : public ITraceServiceManager
{
	//配置变量
protected:
	bool							m_bTraceFlag[5];					//输出标志

	//内核变量
protected:
	ITraceService *					m_pITraceService;					//事件服务
	CCriticalSection				m_CriticalSection;					//锁定对象

	//函数定义
public:
	//构造函数
	CTraceServiceManager();
	//析构函数
	virtual ~CTraceServiceManager();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//状态管理
public:
	//输出状态
	virtual bool IsEnableTrace(enTraceLevel TraceLevel);
	//输出控制
	virtual bool EnableTrace(enTraceLevel TraceLevel, bool bEnableTrace);

	//服务配置
public:
	//设置接口
	virtual bool SetTraceService(IUnknownEx * pIUnknownEx);
	//获取接口
	virtual VOID * GetTraceService(REFGUID Guid, DWORD dwQueryVer);

	//服务接口
public:
	//字串输出
	virtual bool TraceString(LPCTSTR pszString, enTraceLevel TraceLevel);
};

//////////////////////////////////////////////////////////////////////////

//全局变量
extern CTraceServiceManager g_TraceServiceManager;

//////////////////////////////////////////////////////////////////////////

#endif