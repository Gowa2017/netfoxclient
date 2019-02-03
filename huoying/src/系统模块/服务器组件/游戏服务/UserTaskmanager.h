#ifndef USER_TASK_MANAGER_HEAD_FILE
#define USER_TASK_MANAGER_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CWHArray<tagUserTaskEntry *>	CUserTaskEntryArray;	
typedef CWHArray<tagTaskParameter *>	CTaskParameterArray;

//���Ͷ���
typedef CMap<WORD,WORD,tagTaskParameter *,tagTaskParameter *>  CTaskParameterMap;
typedef CMap<DWORD,DWORD,tagUserTaskEntry *,tagUserTaskEntry *>  CUserTaskEntryMap;			

//////////////////////////////////////////////////////////////////////////////////

//���߹���
class GAME_SERVICE_CLASS CUserTaskManager :  public IUserTaskManagerSink
{
	//��������
protected:	
	CTaskParameterMap				m_TaskParameterMap;					//�������
	CUserTaskEntryMap				m_UserTaskEntryMap;					//�������
	CTaskParameterArray				m_TaskParameterBuffer;				//��������
	CUserTaskEntryArray				m_UserTaskEntryBuffer;				//���񻺳�	

	//��������
public:
	//���캯��
	CUserTaskManager();
	//��������
	virtual ~CUserTaskManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�������
public:
	//�Ƴ�����
	virtual VOID RemoveTaskParameter();
	//���Ҳ���
	virtual tagTaskParameter * SearchTaskParameter(WORD wTaskID);
	//ö�ٲ���
	virtual tagTaskParameter * EnumTaskParameter(POSITION & Position);	
	//��Ӳ���
	virtual bool AddTaskParameter(tagTaskParameter TaskParameter[], WORD wPatemterCount);
	//��ȡ������Ŀ
	virtual WORD GetTaskParameterCount();

	//�û�����
public:
	//�Ƴ�����
	virtual VOID RemoveUserTask(DWORD dwUserID);
	//��ȡ����
	virtual tagUserTaskEntry * GetUserTaskEntry(DWORD dwUserID);
	//��ȡ����
	virtual tagUserTaskEntry * GetUserTaskEntry(DWORD dwUserID,BYTE cbTaskStatus);	
	//��������
	virtual VOID SetUserTaskInfo(DWORD dwUserID,tagUserTaskInfo UserTaskInfo[],WORD wTaskCount);	

	//��������
protected:
	//��������
	tagUserTaskEntry * CreateUserTaskEntry();	

	//��������
public:
	//���ö���
	VOID ResetTaskManager();	
};

//////////////////////////////////////////////////////////////////////////////////

#endif