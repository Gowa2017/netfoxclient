#include "StdAfx.h"
#include "TimerEngine.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define NO_TIME_LEAVE				DWORD(-1)							//不响应时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CTimerThread::CTimerThread()
{
	//设置变量
	m_dwLastTickCount=0L;
	m_dwTimerSpace=30;

	//组件接口
	m_pTimerEngine=NULL;

	return;
}

//析构函数
CTimerThread::~CTimerThread()
{
}

//配置函数
bool CTimerThread::InitThread(CTimerEngine * pTimerEngine, DWORD dwTimerSpace)
{
	//效益参数
	ASSERT(dwTimerSpace>=10L);
	ASSERT(pTimerEngine!=NULL);
	if (pTimerEngine==NULL) return false;

	//设置变量
	m_dwLastTickCount=0L;
	m_dwTimerSpace=dwTimerSpace;
	m_pTimerEngine=pTimerEngine;

	return true;
}

//运行函数
bool CTimerThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_pTimerEngine!=NULL);

	//获取时间
	DWORD dwTimerSpace=m_dwTimerSpace;
	DWORD dwNowTickCount=GetTickCount();

	//等待调整
	if ((m_dwLastTickCount!=0L)&&(dwNowTickCount>m_dwLastTickCount))
	{
		DWORD dwHandleTickCount=dwNowTickCount-m_dwLastTickCount;
		dwTimerSpace=(dwTimerSpace>dwHandleTickCount)?(dwTimerSpace-dwHandleTickCount):0L;
	}

	//定时处理
	Sleep(dwTimerSpace);
	m_dwLastTickCount=GetTickCount();

	//时间处理
	m_pTimerEngine->OnTimerThreadSink();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTimerEngine::CTimerEngine()
{
	//状态变量
	m_bService=false;
	m_dwTimePass=0L;
	m_dwTimeLeave=NO_TIME_LEAVE;

	//配置变量
	m_dwTimerSpace=30;
	m_pITimerEngineEvent=NULL;

	return;
}

//析构函数
CTimerEngine::~CTimerEngine()
{
	//停止服务
	ConcludeService();

	//清理内存
	tagTimerItem * pTimerItem=NULL;
	for (INT_PTR i=0;i<m_TimerItemFree.GetCount();i++)
	{
		pTimerItem=m_TimerItemFree[i];
		ASSERT(pTimerItem!=NULL);
		SafeDelete(pTimerItem);
	}
	for (INT_PTR i = 0; i<m_TimerItemActive.GetCount(); i++)
	{
		pTimerItem=m_TimerItemActive[i];
		ASSERT(pTimerItem!=NULL);
		SafeDelete(pTimerItem);
	}
	m_TimerItemFree.RemoveAll();
	m_TimerItemActive.RemoveAll();

	return;
}

//接口查询
VOID * CTimerEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITimerEngine,Guid,dwQueryVer);
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITimerEngine,Guid,dwQueryVer);
	return NULL;
}

//设置定时器
bool CTimerEngine::SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//锁定资源
	CWHDataLocker ThreadLock(m_CriticalSection);

	//效验参数
	ASSERT(dwRepeat>0L);
	if (dwRepeat==0) return false;
	dwElapse=(dwElapse+m_dwTimerSpace-1)/m_dwTimerSpace*m_dwTimerSpace;

	//查找定时器
	bool bTimerExist=false;
	tagTimerItem * pTimerItem=NULL;
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
	{
		pTimerItem=m_TimerItemActive[i];
		if (pTimerItem->dwTimerID==dwTimerID) 
		{
			bTimerExist=true;
			break;
		}
	}

	//创建定时器
	if (bTimerExist==false)
	{
		INT_PTR nFreeCount=m_TimerItemFree.GetCount();
		if (nFreeCount>0)
		{
			pTimerItem=m_TimerItemFree[nFreeCount-1];
			ASSERT(pTimerItem!=NULL);
			m_TimerItemFree.RemoveAt(nFreeCount-1);
		}
		else
		{
			try
			{
				pTimerItem=new tagTimerItem;
				ASSERT(pTimerItem!=NULL);
				if (pTimerItem==NULL) return false;
			}
			catch (...) { return false; }
		}
	}

	//设置参数
	ASSERT(pTimerItem!=NULL);
	pTimerItem->dwElapse=dwElapse;
	pTimerItem->dwTimerID=dwTimerID;
	pTimerItem->dwRepeatTimes=dwRepeat;
	pTimerItem->dwBindParameter=dwBindParameter;
	pTimerItem->dwTimeLeave=dwElapse+m_dwTimePass;

	//激活定时器
	m_dwTimeLeave=__min(m_dwTimeLeave,dwElapse);
	if (bTimerExist==false) m_TimerItemActive.Add(pTimerItem);

	return false;
}

//设置定时器
bool CTimerEngine::SetTimeCell(DWORD dwTimerID)
{
	//锁定资源
	CWHDataLocker ThreadLock(m_CriticalSection);

	DWORD dwElapse=1; DWORD dwRepeat=1; WPARAM dwBindParameter=0;
	//效验参数
	dwElapse = (dwElapse + m_dwTimerSpace - 1) / m_dwTimerSpace*m_dwTimerSpace;

	//查找定时器
	bool bTimerExist = false;
	tagTimerItem * pTimerItem = NULL;
	for (INT_PTR i = 0; i<m_TimerItemActive.GetCount(); i++)
	{
		pTimerItem = m_TimerItemActive[i];
		if (pTimerItem->dwTimerID == dwTimerID)
		{
			bTimerExist = true;
			break;
		}
	}

	//创建定时器
	if (bTimerExist == false)
	{
		INT_PTR nFreeCount = m_TimerItemFree.GetCount();
		if (nFreeCount>0)
		{
			pTimerItem = m_TimerItemFree[nFreeCount - 1];
			ASSERT(pTimerItem != NULL);
			m_TimerItemFree.RemoveAt(nFreeCount - 1);
		}
		else
		{
			try
			{
				pTimerItem = new tagTimerItem;
				ASSERT(pTimerItem != NULL);
				if (pTimerItem == NULL) return false;
			}
			catch (...) { return false; }
		}
	}

	//设置参数
	ASSERT(pTimerItem != NULL);
	pTimerItem->dwElapse = dwElapse;
	pTimerItem->dwTimerID = dwTimerID;
	pTimerItem->dwRepeatTimes = dwRepeat;
	pTimerItem->dwBindParameter = dwBindParameter;
	pTimerItem->dwTimeLeave = dwElapse + m_dwTimePass;

	//激活定时器
	m_dwTimeLeave = __min(m_dwTimeLeave, dwElapse);
	if (bTimerExist == false) m_TimerItemActive.Add(pTimerItem);

	return false;
}

//删除定时器
bool CTimerEngine::KillTimer(DWORD dwTimerID)
{
	//锁定资源
	CWHDataLocker ThreadLock(m_CriticalSection);

	//查找定时器
	for (INT_PTR i=0;i<m_TimerItemActive.GetCount();i++)
	{
		//获取对象
		tagTimerItem * pTimerItem=m_TimerItemActive[i];
		if (pTimerItem->dwTimerID!=dwTimerID) continue;

		//设置对象
		m_TimerItemActive.RemoveAt(i);
		m_TimerItemFree.Add(pTimerItem);

		//停止计数
		if (m_TimerItemActive.GetCount()==0)
		{
			m_dwTimePass=0L;
			m_dwTimeLeave=NO_TIME_LEAVE;
		}

		return true;
	}

	return false;
}

//删除定时器
bool CTimerEngine::KillAllTimer()
{
	//锁定资源
	CWHDataLocker ThreadLock(m_CriticalSection);

	//删除定时器
	m_TimerItemFree.Append(m_TimerItemActive);
	m_TimerItemActive.RemoveAll();

	//设置变量
	m_dwTimePass=0L;
	m_dwTimeLeave=NO_TIME_LEAVE;

	return true;
}

//开始服务
bool CTimerEngine::StartService()
{
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//设置变量
	m_dwTimePass=0L;
	m_dwTimeLeave=NO_TIME_LEAVE;
	if (m_TimerThread.InitThread(this,m_dwTimerSpace)==false) return false;

	//启动服务
	if (m_TimerThread.StartThread()==false) return false;

	//设置变量
	m_bService=true;

	return true;
}

//停止服务
bool CTimerEngine::ConcludeService()
{
	//设置变量
	m_bService=false;

	//停止线程
	m_TimerThread.ConcludeThread(INFINITE);

	//删除定时器
	KillAllTimer();

	return true;
}

//设置接口
bool CTimerEngine::SetTimerEngineEvent(IUnknownEx * pIUnknownEx)
{
	//状态效验
	ASSERT(m_bService==false);
	if (m_bService==true) return false;

	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITimerEngineEvent)!=NULL);
		m_pITimerEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITimerEngineEvent);

		//成功判断
		if (m_pITimerEngineEvent==NULL) return false;
	}
	else m_pITimerEngineEvent=NULL;

    return true;
}

//定时器通知
VOID CTimerEngine::OnTimerThreadSink()
{
	//锁定资源
	CWHDataLocker ThreadLock(m_CriticalSection);

	//倒计时间
	if (m_dwTimeLeave==NO_TIME_LEAVE)
	{
		ASSERT(m_TimerItemActive.GetCount()==0);
		return;
	}

	//减少时间
	m_dwTimePass+=m_dwTimerSpace;
	m_dwTimeLeave-=m_dwTimerSpace;

	//查询定时器
	if (m_dwTimeLeave==0)
	{
		//变量定义
		bool bKillTimer=false;
		DWORD dwTimeLeave=NO_TIME_LEAVE;

		//时间搜索
		for (INT_PTR i=0;i<m_TimerItemActive.GetCount();)
		{
			//获取对象
			tagTimerItem * pTimerItem=m_TimerItemActive[i];
			ASSERT(pTimerItem->dwTimeLeave>=m_dwTimePass);

			//设置变量
			bKillTimer=false;
			pTimerItem->dwTimeLeave-=m_dwTimePass;

			//通知判断
			if (pTimerItem->dwTimeLeave==0L)
			{
				//发送通知
				ASSERT(m_pITimerEngineEvent!=NULL);
				m_pITimerEngineEvent->OnEventTimer(pTimerItem->dwTimerID,pTimerItem->dwBindParameter);

				//设置次数
				if (pTimerItem->dwRepeatTimes!=TIMES_INFINITY)
				{
					if (pTimerItem->dwRepeatTimes==1L)
					{
						bKillTimer=true;
						m_TimerItemActive.RemoveAt(i);
						m_TimerItemFree.Add(pTimerItem);
					}
					else pTimerItem->dwRepeatTimes--;
				}

				//设置时间
				if (bKillTimer==false) pTimerItem->dwTimeLeave=pTimerItem->dwElapse;
			}

			//增加索引
			if (bKillTimer==false) 
			{
				i++;
				dwTimeLeave=__min(dwTimeLeave,pTimerItem->dwTimeLeave);
				ASSERT(dwTimeLeave%m_dwTimerSpace==0);
			}
		}

		//设置响应
		m_dwTimePass=0L;
		m_dwTimeLeave=dwTimeLeave;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(TimerEngine);

//////////////////////////////////////////////////////////////////////////
