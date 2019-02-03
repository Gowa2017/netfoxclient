#ifndef USER_TASK_MANAGER_HEAD_FILE
#define USER_TASK_MANAGER_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//类型定义
typedef CWHArray<tagUserTaskEntry *>	CUserTaskEntryArray;	
typedef CWHArray<tagTaskParameter *>	CTaskParameterArray;

//类型定义
typedef CMap<WORD,WORD,tagTaskParameter *,tagTaskParameter *>  CTaskParameterMap;
typedef CMap<DWORD,DWORD,tagUserTaskEntry *,tagUserTaskEntry *>  CUserTaskEntryMap;			

//////////////////////////////////////////////////////////////////////////////////

//道具管理
class GAME_SERVICE_CLASS CUserTaskManager :  public IUserTaskManagerSink
{
	//变量定义
protected:	
	CTaskParameterMap				m_TaskParameterMap;					//任务参数
	CUserTaskEntryMap				m_UserTaskEntryMap;					//任务入口
	CTaskParameterArray				m_TaskParameterBuffer;				//参数缓冲
	CUserTaskEntryArray				m_UserTaskEntryBuffer;				//任务缓冲	

	//函数定义
public:
	//构造函数
	CUserTaskManager();
	//析构函数
	virtual ~CUserTaskManager();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//任务参数
public:
	//移除参数
	virtual VOID RemoveTaskParameter();
	//查找参数
	virtual tagTaskParameter * SearchTaskParameter(WORD wTaskID);
	//枚举参数
	virtual tagTaskParameter * EnumTaskParameter(POSITION & Position);	
	//添加参数
	virtual bool AddTaskParameter(tagTaskParameter TaskParameter[], WORD wPatemterCount);
	//获取参数数目
	virtual WORD GetTaskParameterCount();

	//用户任务
public:
	//移除任务
	virtual VOID RemoveUserTask(DWORD dwUserID);
	//获取任务
	virtual tagUserTaskEntry * GetUserTaskEntry(DWORD dwUserID);
	//获取任务
	virtual tagUserTaskEntry * GetUserTaskEntry(DWORD dwUserID,BYTE cbTaskStatus);	
	//设置任务
	virtual VOID SetUserTaskInfo(DWORD dwUserID,tagUserTaskInfo UserTaskInfo[],WORD wTaskCount);	

	//辅助函数
protected:
	//创建任务
	tagUserTaskEntry * CreateUserTaskEntry();	

	//辅助函数
public:
	//重置对象
	VOID ResetTaskManager();	
};

//////////////////////////////////////////////////////////////////////////////////

#endif