#include "StdAfx.h"
#include "UserTaskManager.h"

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CUserTaskManager::CUserTaskManager()
{
	//��ʼ����
	m_TaskParameterMap.InitHashTable(TASK_MAX_COUNT+1);
}

//��������
CUserTaskManager::~CUserTaskManager()
{
	//ɾ������
	for(INT_PTR nIndex=0;nIndex<m_UserTaskEntryBuffer.GetCount();nIndex++)
	{
		SafeDelete(m_UserTaskEntryBuffer[nIndex]);
	}

	//ɾ������
	for(INT_PTR nIndex=0;nIndex<m_TaskParameterBuffer.GetCount();nIndex++)
	{
		SafeDelete(m_TaskParameterBuffer[nIndex]);
	}

	//��������
	WORD wKey=0;
	DWORD dwKey=0;
	POSITION Position=NULL;

	//ɾ������
	tagTaskParameter * pTaskParameter=NULL;
	Position=m_TaskParameterMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TaskParameterMap.GetNextAssoc(Position,wKey,pTaskParameter);
		SafeDelete(pTaskParameter);
	}	

	//ɾ������	
	tagUserTaskEntry * pUserTaskEntry=NULL;
	Position=m_UserTaskEntryMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserTaskEntryMap.GetNextAssoc(Position,dwKey,pUserTaskEntry);
		SafeDelete(pUserTaskEntry);
	}	

	//�Ƴ�Ԫ��
	m_TaskParameterMap.RemoveAll();
	m_UserTaskEntryMap.RemoveAll();	
	m_TaskParameterBuffer.RemoveAll();
	m_UserTaskEntryBuffer.RemoveAll();
}

//�ӿڲ�ѯ
VOID * CUserTaskManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IUserTaskManagerSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IUserTaskManagerSink,Guid,dwQueryVer);
	return NULL;
}

//��Ӳ���
bool CUserTaskManager::AddTaskParameter(tagTaskParameter TaskParameter[], WORD wPatemterCount)
{
	//����У��
	if(wPatemterCount==0 || TaskParameter==NULL) return false;

	//��������
    for(WORD i=0;i<wPatemterCount;i++)
	{
		//��������
		tagTaskParameter * pTaskParameter=NULL;

		//�����ִ�
		if(m_TaskParameterBuffer.GetCount()>0)
		{
			//��������
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

		//��������
		if(pTaskParameter==NULL)
		{
			try
			{
				pTaskParameter = new tagTaskParameter;
				if(pTaskParameter==NULL) throw(TEXT("�ڴ治�㣡"));
			}
			catch(...)
			{
				ASSERT(FALSE);
				break;
			}
		}

		//��������
		CopyMemory(pTaskParameter,&TaskParameter[i],sizeof(tagTaskParameter));

		//��������
		m_TaskParameterMap[pTaskParameter->wTaskID] = pTaskParameter;		
	}

	return true;
}

//��ȡ������Ŀ
WORD CUserTaskManager::GetTaskParameterCount()
{
	return (WORD)m_TaskParameterMap.GetCount();
}

//�Ƴ�����
VOID CUserTaskManager::RemoveTaskParameter()
{
	//��������
	WORD wKey=0;
	POSITION Position=NULL;

	//�Ƴ�����	
	tagTaskParameter * pTaskParameter=NULL;
	Position=m_TaskParameterMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TaskParameterMap.GetNextAssoc(Position,wKey,pTaskParameter);
		m_TaskParameterBuffer.Add(pTaskParameter);
	}

	//�Ƴ�����
	m_TaskParameterMap.RemoveAll();
}

//���Ҳ���
tagTaskParameter * CUserTaskManager::SearchTaskParameter(WORD wTaskID)
{
	//��������
	tagTaskParameter * pTaskParameter=NULL;
	m_TaskParameterMap.Lookup(wTaskID,pTaskParameter);

	return pTaskParameter;
}

//ö�ٲ���
tagTaskParameter * CUserTaskManager::EnumTaskParameter(POSITION & Position)
{
	//��������
	if(Position==NULL) Position=m_TaskParameterMap.GetStartPosition();
	if(Position==NULL) return NULL;

	//��������
	WORD wKey=0;
	tagTaskParameter * pTaskParameter=NULL;
	m_TaskParameterMap.GetNextAssoc(Position,wKey,pTaskParameter);		

	return pTaskParameter;
}

//��������
VOID CUserTaskManager::SetUserTaskInfo(DWORD dwUserID,tagUserTaskInfo UserTaskInfo[],WORD wTaskCount)
{
	//����У��
	ASSERT(wTaskCount>0 && UserTaskInfo!=NULL);
	if(wTaskCount==0 || UserTaskInfo==NULL) return;

	//��������
	tagUserTaskEntry * pUserTaskEntry=NULL;
	tagUserTaskEntry * pUserTaskEntryHead=NULL;

	//���Ҷ���
	m_UserTaskEntryMap.Lookup(dwUserID,pUserTaskEntryHead);

	//��������β��
	while(pUserTaskEntryHead!=NULL && pUserTaskEntryHead->pNextTaskEntry)
	{
		pUserTaskEntryHead=pUserTaskEntryHead->pNextTaskEntry;
	}

	//��������
    for(WORD i=0;i<wTaskCount;i++)
	{
		//��������
		pUserTaskEntry=CreateUserTaskEntry();
		if(pUserTaskEntry==NULL) break;

		//���ö���
		pUserTaskEntry->cbTaskStatus=UserTaskInfo[i].cbTaskStatus;		
		pUserTaskEntry->wTaskProgress=UserTaskInfo[i].wTaskProgress;
		pUserTaskEntry->dwResidueTime=UserTaskInfo[i].dwResidueTime;
		pUserTaskEntry->dwLastUpdateTime=UserTaskInfo[i].dwLastUpdateTime;

		//���Ҳ���
		m_TaskParameterMap.Lookup(UserTaskInfo[i].wTaskID,pUserTaskEntry->pTaskParameter);

		//��������
		if(pUserTaskEntryHead!=NULL)
		{
			pUserTaskEntryHead->pNextTaskEntry=pUserTaskEntry;
			pUserTaskEntryHead=pUserTaskEntryHead->pNextTaskEntry;
		}

		//��������ͷ
		if(i==0 && pUserTaskEntryHead==NULL) 
		{
			pUserTaskEntryHead=pUserTaskEntry;
			m_UserTaskEntryMap[dwUserID]=pUserTaskEntryHead;
		}

	}	

	return;
}

//�Ƴ�����
VOID CUserTaskManager::RemoveUserTask(DWORD dwUserID)
{
	//���Ҷ���
	tagUserTaskEntry * pUserTaskEntry=NULL;
	if(m_UserTaskEntryMap.Lookup(dwUserID,pUserTaskEntry)==TRUE)
	{
		//��ӵ�����
		while(pUserTaskEntry!=NULL)
		{						
			m_UserTaskEntryBuffer.Add(pUserTaskEntry);
			pUserTaskEntry=pUserTaskEntry->pNextTaskEntry;
		}

		//�Ƴ�����
		m_UserTaskEntryMap.RemoveKey(dwUserID);
	}
}

//��ȡ����
tagUserTaskEntry * CUserTaskManager::GetUserTaskEntry(DWORD dwUserID)
{
	//��������
	tagUserTaskEntry * pUserTaskEntry=NULL;
	m_UserTaskEntryMap.Lookup(dwUserID,pUserTaskEntry);

	return pUserTaskEntry;
}

//��ȡ����
tagUserTaskEntry * CUserTaskManager::GetUserTaskEntry(DWORD dwUserID,BYTE cbTaskStatus)
{
	//��������
	tagUserTaskEntry * pUserTaskEntry=NULL;
	m_UserTaskEntryMap.Lookup(dwUserID,pUserTaskEntry);
	if(pUserTaskEntry==NULL) return NULL;

	//��������
	tagUserTaskEntry * pUserTaskEntryHead=NULL;
	tagUserTaskEntry * pUserTaskEntryLink=NULL;

	//��������
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

//��������
tagUserTaskEntry * CUserTaskManager::CreateUserTaskEntry()
{
	//��������
	tagUserTaskEntry * pUserTaskEntry=NULL;

	//���һ���
	if(m_UserTaskEntryBuffer.GetCount()>0)
	{
		pUserTaskEntry=m_UserTaskEntryBuffer[0];
		m_UserTaskEntryBuffer.RemoveAt(0);
	}

	//��������
	if(pUserTaskEntry==NULL)
	{		
		try
		{
			pUserTaskEntry=new tagUserTaskEntry;
			if(pUserTaskEntry==NULL) throw TEXT("�ڴ治�㣬��Դ����ʧ�ܣ�");			
		}
		catch(...)
		{
			ASSERT(FALSE);
			SafeDelete(pUserTaskEntry);
		}
	}

	//��ʼ�ڴ�
	ZeroMemory(pUserTaskEntry,sizeof(tagUserTaskEntry));

	return pUserTaskEntry;
}

//���ö���
VOID CUserTaskManager::ResetTaskManager()
{
	//�Ƴ�����
	RemoveTaskParameter();
	
	//��������
	DWORD dwKey=0;
	POSITION Position=NULL;

	//�Ƴ�����
	tagUserTaskEntry * pUserTaskEntry=NULL;
	Position=m_UserTaskEntryMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_UserTaskEntryMap.GetNextAssoc(Position,dwKey,pUserTaskEntry);
		m_UserTaskEntryBuffer.Add(pUserTaskEntry);
	}	

	//�Ƴ�Ԫ��
	m_TaskParameterMap.RemoveAll();
	m_UserTaskEntryMap.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////
