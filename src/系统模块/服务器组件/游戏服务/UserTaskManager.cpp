#include "StdAfx.h"
#include "UserTaskManager.h"

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CUserTaskManager::CUserTaskManager()
{
	//初始化表
	m_TaskParameterMap.InitHashTable(TASK_MAX_COUNT+1);
}

//析构函数
CUserTaskManager::~CUserTaskManager()
{
	//删除缓冲
	for(INT_PTR nIndex=0;nIndex<m_UserTaskEntryBuffer.GetCount();nIndex++)
	{
		SafeDelete(m_UserTaskEntryBuffer[nIndex]);
	}

	//删除缓冲
	for(INT_PTR nIndex=0;nIndex<m_TaskParameterBuffer.GetCount();nIndex++)
	{
		SafeDelete(m_TaskParameterBuffer[nIndex]);
	}

	//变量定义
	WORD wKey=0;
	DWORD dwKey=0;
	POSITION Position=NULL;

	//删除参数
	tagTaskParameter * pTaskParameter=NULL;
	Position=m_TaskParameterMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TaskParameterMap.GetNextAssoc(Position,wKey,pTaskParameter);
		SafeDelete(pTaskParameter);
	}	

	//删除任务	
	tagUserTaskEntry * pUserTaskEntry=NULL;
	Position=m_UserTaskEntryMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserTaskEntryMap.GetNextAssoc(Position,dwKey,pUserTaskEntry);
		SafeDelete(pUserTaskEntry);
	}	

	//移除元素
	m_TaskParameterMap.RemoveAll();
	m_UserTaskEntryMap.RemoveAll();	
	m_TaskParameterBuffer.RemoveAll();
	m_UserTaskEntryBuffer.RemoveAll();
}

//接口查询
VOID * CUserTaskManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IUserTaskManagerSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IUserTaskManagerSink,Guid,dwQueryVer);
	return NULL;
}

//添加参数
bool CUserTaskManager::AddTaskParameter(tagTaskParameter TaskParameter[], WORD wPatemterCount)
{
	//参数校验
	if(wPatemterCount==0 || TaskParameter==NULL) return false;

	//变量定义
    for(WORD i=0;i<wPatemterCount;i++)
	{
		//变量定义
		tagTaskParameter * pTaskParameter=NULL;

		//查找现存
		if(m_TaskParameterBuffer.GetCount()>0)
		{
			//查找任务
			for(INT_PTR nIndex=0;nIndex<m_TaskParameterBuffer.GetCount();nIndex++)
			{
				if(m_TaskParameterBuffer[nIndex]->wTaskID==TaskParameter[i].wTaskID)
				{
					pTaskParameter=m_TaskParameterBuffer[nIndex];
					m_TaskParameterBuffer.RemoveAt(nIndex);
					break;
				}
			}			
		}

		//创建对象
		if(pTaskParameter==NULL)
		{
			try
			{
				pTaskParameter = new tagTaskParameter;
				if(pTaskParameter==NULL) throw(TEXT("内存不足！"));
			}
			catch(...)
			{
				ASSERT(FALSE);
				break;
			}
		}

		//拷贝数据
		CopyMemory(pTaskParameter,&TaskParameter[i],sizeof(tagTaskParameter));

		//保存任务
		m_TaskParameterMap[pTaskParameter->wTaskID] = pTaskParameter;		
	}

	return true;
}

//获取参数数目
WORD CUserTaskManager::GetTaskParameterCount()
{
	return (WORD)m_TaskParameterMap.GetCount();
}

//移除参数
VOID CUserTaskManager::RemoveTaskParameter()
{
	//变量定义
	WORD wKey=0;
	POSITION Position=NULL;

	//移除参数	
	tagTaskParameter * pTaskParameter=NULL;
	Position=m_TaskParameterMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TaskParameterMap.GetNextAssoc(Position,wKey,pTaskParameter);
		m_TaskParameterBuffer.Add(pTaskParameter);
	}

	//移除严肃
	m_TaskParameterMap.RemoveAll();
}

//查找参数
tagTaskParameter * CUserTaskManager::SearchTaskParameter(WORD wTaskID)
{
	//变量定义
	tagTaskParameter * pTaskParameter=NULL;
	m_TaskParameterMap.Lookup(wTaskID,pTaskParameter);

	return pTaskParameter;
}

//枚举参数
tagTaskParameter * CUserTaskManager::EnumTaskParameter(POSITION & Position)
{
	//参数调整
	if(Position==NULL) Position=m_TaskParameterMap.GetStartPosition();
	if(Position==NULL) return NULL;

	//变量定义
	WORD wKey=0;
	tagTaskParameter * pTaskParameter=NULL;
	m_TaskParameterMap.GetNextAssoc(Position,wKey,pTaskParameter);		

	return pTaskParameter;
}

//设置任务
VOID CUserTaskManager::SetUserTaskInfo(DWORD dwUserID,tagUserTaskInfo UserTaskInfo[],WORD wTaskCount)
{
	//参数校验
	ASSERT(wTaskCount>0 && UserTaskInfo!=NULL);
	if(wTaskCount==0 || UserTaskInfo==NULL) return;

	//变量定义
	tagUserTaskEntry * pUserTaskEntry=NULL;
	tagUserTaskEntry * pUserTaskEntryHead=NULL;

	//查找对象
	m_UserTaskEntryMap.Lookup(dwUserID,pUserTaskEntryHead);

	//查找链表尾部
	while(pUserTaskEntryHead!=NULL && pUserTaskEntryHead->pNextTaskEntry)
	{
		pUserTaskEntryHead=pUserTaskEntryHead->pNextTaskEntry;
	}

	//变量定义
    for(WORD i=0;i<wTaskCount;i++)
	{
		//创建对象
		pUserTaskEntry=CreateUserTaskEntry();
		if(pUserTaskEntry==NULL) break;

		//设置对象
		pUserTaskEntry->cbTaskStatus=UserTaskInfo[i].cbTaskStatus;		
		pUserTaskEntry->wTaskProgress=UserTaskInfo[i].wTaskProgress;
		pUserTaskEntry->dwResidueTime=UserTaskInfo[i].dwResidueTime;
		pUserTaskEntry->dwLastUpdateTime=UserTaskInfo[i].dwLastUpdateTime;

		//查找参数
		m_TaskParameterMap.Lookup(UserTaskInfo[i].wTaskID,pUserTaskEntry->pTaskParameter);

		//链接任务
		if(pUserTaskEntryHead!=NULL)
		{
			pUserTaskEntryHead->pNextTaskEntry=pUserTaskEntry;
			pUserTaskEntryHead=pUserTaskEntryHead->pNextTaskEntry;
		}

		//设置链表头
		if(i==0 && pUserTaskEntryHead==NULL) 
		{
			pUserTaskEntryHead=pUserTaskEntry;
			m_UserTaskEntryMap[dwUserID]=pUserTaskEntryHead;
		}

	}	

	return;
}

//移除任务
VOID CUserTaskManager::RemoveUserTask(DWORD dwUserID)
{
	//查找对象
	tagUserTaskEntry * pUserTaskEntry=NULL;
	if(m_UserTaskEntryMap.Lookup(dwUserID,pUserTaskEntry)==TRUE)
	{
		//添加到缓冲
		while(pUserTaskEntry!=NULL)
		{						
			m_UserTaskEntryBuffer.Add(pUserTaskEntry);
			pUserTaskEntry=pUserTaskEntry->pNextTaskEntry;
		}

		//移除对象
		m_UserTaskEntryMap.RemoveKey(dwUserID);
	}
}

//获取任务
tagUserTaskEntry * CUserTaskManager::GetUserTaskEntry(DWORD dwUserID)
{
	//变量定义
	tagUserTaskEntry * pUserTaskEntry=NULL;
	m_UserTaskEntryMap.Lookup(dwUserID,pUserTaskEntry);

	return pUserTaskEntry;
}

//获取任务
tagUserTaskEntry * CUserTaskManager::GetUserTaskEntry(DWORD dwUserID,BYTE cbTaskStatus)
{
	//变量定义
	tagUserTaskEntry * pUserTaskEntry=NULL;
	m_UserTaskEntryMap.Lookup(dwUserID,pUserTaskEntry);
	if(pUserTaskEntry==NULL) return NULL;

	//变量定义
	tagUserTaskEntry * pUserTaskEntryHead=NULL;
	tagUserTaskEntry * pUserTaskEntryLink=NULL;

	//遍历链表
	while(pUserTaskEntry!=NULL)
	{
		if(pUserTaskEntry->cbTaskStatus==cbTaskStatus)
		{
			if(pUserTaskEntryHead==NULL) 
			{
				pUserTaskEntryHead=pUserTaskEntry;
				pUserTaskEntryLink=pUserTaskEntryHead;
			}
			else
			{
				pUserTaskEntryLink->pNextStatusEntry=pUserTaskEntry;
				pUserTaskEntryLink=pUserTaskEntryLink->pNextStatusEntry;				
			}

			pUserTaskEntryLink->pNextStatusEntry=NULL;
		}	

		pUserTaskEntry=pUserTaskEntry->pNextTaskEntry;
	}

	return pUserTaskEntryHead;
}

//创建任务
tagUserTaskEntry * CUserTaskManager::CreateUserTaskEntry()
{
	//变量定义
	tagUserTaskEntry * pUserTaskEntry=NULL;

	//查找缓冲
	if(m_UserTaskEntryBuffer.GetCount()>0)
	{
		pUserTaskEntry=m_UserTaskEntryBuffer[0];
		m_UserTaskEntryBuffer.RemoveAt(0);
	}

	//创建任务
	if(pUserTaskEntry==NULL)
	{		
		try
		{
			pUserTaskEntry=new tagUserTaskEntry;
			if(pUserTaskEntry==NULL) throw TEXT("内存不足，资源申请失败！");			
		}
		catch(...)
		{
			ASSERT(FALSE);
			SafeDelete(pUserTaskEntry);
		}
	}

	//初始内存
	ZeroMemory(pUserTaskEntry,sizeof(tagUserTaskEntry));

	return pUserTaskEntry;
}

//重置对象
VOID CUserTaskManager::ResetTaskManager()
{
	//移除参数
	RemoveTaskParameter();
	
	//变量定义
	DWORD dwKey=0;
	POSITION Position=NULL;

	//移除任务
	tagUserTaskEntry * pUserTaskEntry=NULL;
	Position=m_UserTaskEntryMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserTaskEntryMap.GetNextAssoc(Position,dwKey,pUserTaskEntry);
		m_UserTaskEntryBuffer.Add(pUserTaskEntry);
	}	

	//移除元素
	m_TaskParameterMap.RemoveAll();
	m_UserTaskEntryMap.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////
