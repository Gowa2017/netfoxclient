#include "StdAfx.h"
#include "DistributeManager.h"

//////////////////////////////////////////////////////////////////////////////////

//�ڴ����
tagDistributeNode * CDistributeNodePool::m_pHeadOfFreeList=NULL;
const int CDistributeNodePool::BLOCK_SIZE = 20;

//��������
#define DISTRIBUTE_WAIT_TIMESTAMP			10							//�ȴ�ʱ���

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDistributeNodePool::CDistributeNodePool()
{
}

//��������
CDistributeNodePool::~CDistributeNodePool()
{
	if(m_pHeadOfFreeList!=NULL)
	{
		tagDistributeNode * pDistributeNode=m_pHeadOfFreeList;
		while(pDistributeNode!=NULL)
		{
			//��ȫ�ͷ�
			m_pHeadOfFreeList = pDistributeNode->pNextDistributeNode;
			SafeDelete(pDistributeNode);
			pDistributeNode=m_pHeadOfFreeList;
		}
	}
}

//������
tagDistributeNode * CDistributeNodePool::AllocNode()
{
	//��ȡͷ���
	tagDistributeNode *pDistributeNode = m_pHeadOfFreeList; 
	if (pDistributeNode!=NULL)
	{
		m_pHeadOfFreeList = pDistributeNode->pNextDistributeNode;
	}
	else 
	{
		//�������ڴ�
		for (int nIndex=0;nIndex<BLOCK_SIZE;nIndex++)
		{
			tagDistributeNode * pNewBlock = new tagDistributeNode;
			pNewBlock->pNextDistributeNode=m_pHeadOfFreeList;
			m_pHeadOfFreeList=pNewBlock;
		}
		
		//���ý��
		pDistributeNode = m_pHeadOfFreeList;
		m_pHeadOfFreeList=pDistributeNode->pNextDistributeNode;
	}

	return pDistributeNode;
}

//�ͷŽ��
VOID CDistributeNodePool::FreeNode(void * pNode)
{
	//�黹���
	tagDistributeNode * pDeadNode =  static_cast<tagDistributeNode*>(pNode);
	pDeadNode->pNextDistributeNode = m_pHeadOfFreeList;
	m_pHeadOfFreeList = pDeadNode;

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDistributeManager::CDistributeManager()
{
	//���ñ���
	m_pHeadNode=NULL;
	m_wNodeCount=0;
	m_wAndroidCount=0;
	m_wRealCount=0;
	m_cbDistributeRule=0;

	//�����ֵ�
	m_SameTableMap.InitHashTable(10003);
}

//��������
CDistributeManager::~CDistributeManager()
{
	//�Ƴ��ڵ�
	RemoveAll();

	//�ͷŶ���
	m_SameTableBuffer.Append(m_SameTableActive);
	for(INT_PTR nIndex=0;nIndex<m_SameTableBuffer.GetCount();nIndex++)
	{
		SafeDelete(m_SameTableBuffer[nIndex]);
	}

	//�Ƴ�Ԫ��
	m_SameTableBuffer.RemoveAll();
	m_SameTableActive.RemoveAll();
}


//������
BOOL CDistributeManager::InsertDistributeNode(const tagDistributeInfo & DistributeInfo)
{
	//�����û�
	if (SearchNode(DistributeInfo.pIServerUserItem)!=NULL) return true;

	//ͷ���ж�
	if (m_pHeadNode==NULL)
	{
		//������
		m_pHeadNode = m_DistributeNodePool.AllocNode();
		if(m_pHeadNode==NULL) return false;

		//���ñ���
		m_pHeadNode->pNextDistributeNode=NULL;
		m_pHeadNode->pPrepDistributeNode=NULL;
		CopyMemory(&m_pHeadNode->DistributeInfo,&DistributeInfo,sizeof(DistributeInfo));
		m_pHeadNode->DistributeInfo.pPertainNode=m_pHeadNode;
	}
	else
	{
		//������
		tagDistributeNode * pDistributeNode=m_DistributeNodePool.AllocNode();
		if (pDistributeNode==NULL) return false;

		//���ý��
		pDistributeNode->pNextDistributeNode=NULL;
		pDistributeNode->pPrepDistributeNode=NULL;
		CopyMemory(&pDistributeNode->DistributeInfo,&DistributeInfo,sizeof(DistributeInfo));
		pDistributeNode->DistributeInfo.pPertainNode=pDistributeNode;
		
		//��ͷ���
		if (m_pHeadNode->pNextDistributeNode!=NULL)
		{
			m_pHeadNode->pNextDistributeNode->pPrepDistributeNode=pDistributeNode;
			pDistributeNode->pNextDistributeNode=m_pHeadNode->pNextDistributeNode;
		}
		
		pDistributeNode->pPrepDistributeNode=m_pHeadNode;
		m_pHeadNode->pNextDistributeNode=pDistributeNode;
	}

	//������Ŀ
	if (DistributeInfo.pIServerUserItem->IsAndroidUser())
		++m_wAndroidCount;
	else
		++m_wRealCount;

	++m_wNodeCount;

	return true;
}

//�Ƴ����
VOID  CDistributeManager::RemoveDistributeNode(IServerUserItem  * pIServerUserItem)
{
	//���ҽ��
	tagDistributeNode  *pDistributeNode=SearchNode(pIServerUserItem);
	if (pDistributeNode!=NULL) 
	{
		RemoveDistributeNode(pDistributeNode);
	}

	return;
}

//�Ƴ����
VOID  CDistributeManager::RemoveDistributeNode(tagDistributeNode * pDistributeNode)
{
	//����У��
	if (pDistributeNode==NULL) return;

	//�����û�
	if (SearchNode(pDistributeNode->DistributeInfo.pIServerUserItem)==NULL) return;

	//��������
	tagDistributeNode  *pPrepNode=pDistributeNode->pPrepDistributeNode;
	tagDistributeNode  *pNextNode=pDistributeNode->pNextDistributeNode;

	//�������
	if (pPrepNode!=NULL)
	{
		//�м���
		if (pNextNode!=NULL)
		{
			pPrepNode->pNextDistributeNode=pNextNode;
			pNextNode->pPrepDistributeNode=pPrepNode;
		}
		else
		{
			//β�����
			if (pPrepNode->pNextDistributeNode==pDistributeNode)
			{
				pPrepNode->pNextDistributeNode=NULL;
			}
		}
	}
	else
	{
		//ͷ�����
		if (pNextNode!=NULL)
		{
			pNextNode->pPrepDistributeNode=NULL;
			m_pHeadNode=pNextNode;
		}
		else
		{
			m_pHeadNode=NULL;
		}
	}

    //������Ŀ
	if (pDistributeNode->DistributeInfo.pIServerUserItem->IsAndroidUser())
		--m_wAndroidCount;
	else
		--m_wRealCount;
	--m_wNodeCount;

	//��ȫ�ͷ�
	m_DistributeNodePool.FreeNode(pDistributeNode);
}

//�Ƴ����
VOID CDistributeManager::RemoveAll()
{
	//�ͷ��ڴ�
	while(m_pHeadNode!=NULL) RemoveDistributeNode(m_pHeadNode);

	//���ñ���
	m_pHeadNode=NULL;
	m_wNodeCount=0;
	m_wAndroidCount=0;
	m_wRealCount=0;

	return;
}

//ִ�з���
WORD CDistributeManager::PerformDistribute(CDistributeInfoArray & DistributeInfoArray,WORD wNeedCount)
{
	//�������
	tagDistributeNode  * pMoveNode=NULL;
	tagDistributeNode  * pMoveStartNode=NULL;
	if (m_pHeadNode!=NULL && m_wNodeCount>1)
	{
		pMoveNode = m_pHeadNode;
		WORD wRandNodeIndex=rand()%m_wNodeCount;		
		while(wRandNodeIndex-->0)
		{
			if(pMoveNode==NULL)
			{
				pMoveNode = m_pHeadNode;
				break;
			}
			pMoveNode = pMoveNode->pNextDistributeNode; 
		}

		//���ñ���
		if(pMoveNode!=NULL) pMoveStartNode = pMoveNode;
	}

	//
	if(pMoveNode==NULL) return 0;

	//��ȡʱ���
	DWORD dwCurrentStamp = (DWORD)time(NULL);

	//��ȡ�û�
	do
	{
		//�������
		BOOL bFirstSuccess=TRUE;

		//�ȴ�ʱ��
		if (dwCurrentStamp-pMoveNode->DistributeInfo.dwInsertStamp< DISTRIBUTE_WAIT_TIMESTAMP)
			bFirstSuccess=FALSE;

		//�ȼ�����
		if (DistributeInfoArray.GetCount()>0 && DistributeInfoArray[0].wDistribute!=pMoveNode->DistributeInfo.wDistribute)
			bFirstSuccess=FALSE;

		//��������
		if (bFirstSuccess==TRUE && DistributeInfoArray.GetCount()==wNeedCount-1 && 
			FilterRuleIsAllAndroid(DistributeInfoArray,pMoveNode->DistributeInfo.pIServerUserItem))
			bFirstSuccess=FALSE;

		//ͬIP����
		if (bFirstSuccess==TRUE && (m_cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)==0 &&  
			FilterRuleExitsIPAddr(DistributeInfoArray,pMoveNode->DistributeInfo.dwClientAddr)==TRUE)
			bFirstSuccess=FALSE;

		//ͬ������
		if(bFirstSuccess==TRUE && (m_cbDistributeRule&DISTRIBUTE_LAST_TABLE)==0 &&  
			FilterRuleIsLastSameTable(DistributeInfoArray,pMoveNode->DistributeInfo.pIServerUserItem->GetUserID())==TRUE)
			bFirstSuccess=FALSE;

		//��ȡ�ɹ�
		if (bFirstSuccess==TRUE) DistributeInfoArray.Add(pMoveNode->DistributeInfo);

		//��ǰ�ƽ�
		pMoveNode=pMoveNode->pNextDistributeNode;
		if (pMoveNode==NULL) pMoveNode=m_pHeadNode;

		//�ɹ��ж�
		if (DistributeInfoArray.GetCount()==wNeedCount) break;

	}while (pMoveNode && pMoveNode!=pMoveStartNode);

	return (WORD)DistributeInfoArray.GetCount();
}

//���ҽ��
tagDistributeNode * CDistributeManager::SearchNode(IServerUserItem  * pIServerUserItem)
{
	if (m_pHeadNode==NULL) return NULL;

	//���ñ���
	tagDistributeNode *pMoveNode=m_pHeadNode;

	//���ҽ��
	while (pMoveNode!=NULL)
	{
		//�ӿ��ж�
		if (pMoveNode->DistributeInfo.pIServerUserItem==pIServerUserItem)
			return pMoveNode;

		//��ǰ�ƽ�
		pMoveNode=pMoveNode->pNextDistributeNode;
	}

	return NULL;
}

//��ȡ��Ϣ
tagSameTableInfo * CDistributeManager::GetUserSameTableInfo(DWORD dwUserID)
{
	//��������
	tagSameTableInfo * pSameTableInfo;
	if(m_SameTableMap.Lookup(dwUserID,pSameTableInfo)==FALSE)
	{
		pSameTableInfo = ActiveSameTableInfo();
		m_SameTableMap[dwUserID] = pSameTableInfo;
	}

	return pSameTableInfo;
}

//�Ƴ���Ϣ
VOID CDistributeManager::RemoveUserSameTableInfo(DWORD dwUserID)
{
	//��������
	tagSameTableInfo * pSameTableInfo;
	if(m_SameTableMap.Lookup(dwUserID,pSameTableInfo)==TRUE)
	{
		//�Ƴ���Ϣ
		m_SameTableMap.RemoveKey(dwUserID);
		RemoveSameTableInfo(pSameTableInfo);		
	}
}

//�������
tagSameTableInfo * CDistributeManager::ActiveSameTableInfo()
{
	//���һ���
	if(m_SameTableBuffer.GetCount()>0)
	{
		tagSameTableInfo * pSameTableInfo = m_SameTableBuffer.GetAt(0);
		m_SameTableBuffer.RemoveAt(0);
		m_SameTableActive.Add(pSameTableInfo);
		return pSameTableInfo;
	}

	//��������
	try
	{
		//��������
		tagSameTableInfo * pSameTableInfo = new tagSameTableInfo;
		if(pSameTableInfo==NULL) throw(TEXT("�ڴ治��,���󴴽�ʧ��!"));

		//���ö���
		ZeroMemory(pSameTableInfo,sizeof(tagSameTableInfo));
		m_SameTableActive.Add(pSameTableInfo);

		return pSameTableInfo;
	}
	catch(...)
	{
		ASSERT(FALSE);
	}

	return NULL;
}

//�Ƴ�����
VOID CDistributeManager::RemoveSameTableInfo(tagSameTableInfo * pSameTableInfo) 
{
	ZeroMemory(pSameTableInfo,sizeof(tagSameTableInfo));
	m_SameTableBuffer.Add(pSameTableInfo);

	//���Ҷ���
	for(INT_PTR nIndex=0;nIndex<m_SameTableActive.GetCount();nIndex++)
	{
		if(m_SameTableActive[nIndex]==pSameTableInfo)
		{
			m_SameTableActive.RemoveAt(nIndex);
			break;
		}
	}
}

//IPַͬ
BOOL  CDistributeManager::FilterRuleExitsIPAddr(const CDistributeInfoArray & DistributeInfoArray,DWORD dwClientAddr)
{
	//����ͬIP
	for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		if(DistributeInfoArray[nIndex].dwClientAddr==dwClientAddr)
			return TRUE;
	}

	return FALSE;
}

//��������
BOOL CDistributeManager::FilterRuleIsAllAndroid(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem  * const pIServerUserItem)
{
	//����У��
	if(pIServerUserItem==NULL || DistributeInfoArray.GetCount()==0) return FALSE;

	//��������
	WORD wAndroidCount=0;

	//ͳ�ƻ���
	for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		if(DistributeInfoArray[nIndex].pIServerUserItem->IsAndroidUser()==true)
			++wAndroidCount;
	}

	return (wAndroidCount==DistributeInfoArray.GetCount()) && pIServerUserItem->IsAndroidUser();
}

//�Ͼ�ͬ��
BOOL CDistributeManager::FilterRuleIsLastSameTable(const CDistributeInfoArray & DistributeInfoArray,DWORD dwUserID)
{
	//����У��
	if(DistributeInfoArray.GetCount()==0) return FALSE;

	//��������
	tagSameTableInfo * pSameTableInfo=NULL;

	for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		pSameTableInfo = GetUserSameTableInfo(DistributeInfoArray[nIndex].pIServerUserItem->GetUserID());
		if(pSameTableInfo!=NULL)
		{
			for(WORD i=0;i<pSameTableInfo->wPlayerCount;i++)
			{
				if(pSameTableInfo->wPlayerIDSet[i]==dwUserID) return TRUE;				
			}
		}
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
