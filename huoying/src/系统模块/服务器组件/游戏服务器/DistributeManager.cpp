#include "StdAfx.h"
#include "DistributeManager.h"

//////////////////////////////////////////////////////////////////////////////////

//��̬��Ա
tagDistributeNode * CDistributeNodePool::m_pHeadOfFreeList=NULL;

//////////////////////////////////////////////////////////////////////////////////
//��������
const int CDistributeNodePool::BLOCK_SIZE = 40;

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDistributeNodePool::CDistributeNodePool()
{
}

//��������
CDistributeNodePool::~CDistributeNodePool()
{
	//�ͷ��б�
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
	if(pDistributeNode!=NULL)
	{
		m_pHeadOfFreeList = pDistributeNode->pNextDistributeNode;
	}
	else 
	{
		//�����ڴ�
		for(int nIndex = 0; nIndex < BLOCK_SIZE; nIndex++)
		{
			tagDistributeNode * pNewBlock = new tagDistributeNode;
			pNewBlock->pNextDistributeNode=m_pHeadOfFreeList;
			m_pHeadOfFreeList=pNewBlock;
		}
		
		//���ý��
		pDistributeNode = m_pHeadOfFreeList;
		m_pHeadOfFreeList=pDistributeNode->pNextDistributeNode;
	}

	//��ʼ�ڵ�
	ZeroMemory(pDistributeNode,sizeof(tagDistributeNode));

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
	m_wRealCount=0;
	m_wAndroidCount=0;
	m_cbDistributeRule=0;
}

//��������
CDistributeManager::~CDistributeManager()
{
	//�Ƴ��ڵ�
	RemoveAll();
}

//������
BOOL CDistributeManager::InsertDistributeNode(const tagDistributeInfo & DistributeInfo)
{
	//�����û�
	if(SearchNode(DistributeInfo.pIServerUserItem)!=NULL) return false;

	//ͷ���ж�
	if(m_pHeadNode==NULL)
	{
		//������
		m_pHeadNode = m_DistributeNodePool.AllocNode();
		if(m_pHeadNode==NULL) return false;

		//���ñ���
		CopyMemory(&m_pHeadNode->DistributeInfo,&DistributeInfo,sizeof(DistributeInfo));
		m_pHeadNode->DistributeInfo.pPertainNode=m_pHeadNode;
	}
	else
	{
		//������
		tagDistributeNode * pDistributeNode = m_DistributeNodePool.AllocNode();
		if(pDistributeNode==NULL) return false;

		//���ý��
		CopyMemory(&pDistributeNode->DistributeInfo,&DistributeInfo,sizeof(DistributeInfo));
		pDistributeNode->DistributeInfo.pPertainNode=pDistributeNode;
		
		//������
		if(m_pHeadNode->pNextDistributeNode!=NULL)
		{
			m_pHeadNode->pNextDistributeNode->pPrepDistributeNode=pDistributeNode;
			pDistributeNode->pNextDistributeNode=m_pHeadNode->pNextDistributeNode;
		}
		
		//���ӽ��
		pDistributeNode->pPrepDistributeNode=m_pHeadNode;
		m_pHeadNode->pNextDistributeNode=pDistributeNode;
	}

	//������Ŀ
	if(DistributeInfo.pIServerUserItem->IsAndroidUser())
		++m_wAndroidCount;
	else
		++m_wRealCount;
	++m_wNodeCount;

	return true;
}

//�Ƴ����
VOID  CDistributeManager::RemoveDistributeNode(const IServerUserItem  * pIServerUserItem)
{
	//���ҽ��
	tagDistributeNode  *pDistributeNode=SearchNode(pIServerUserItem);
	if(pDistributeNode!=NULL) RemoveDistributeNode(pDistributeNode);

	return;
}

//�Ƴ����
VOID  CDistributeManager::RemoveDistributeNode(tagDistributeNode * pDistributeNode)
{
	//����У��
	if(pDistributeNode==NULL) return;

	//��������
	tagDistributeNode  *pPrepNode=pDistributeNode->pPrepDistributeNode;
	tagDistributeNode  *pNextNode=pDistributeNode->pNextDistributeNode;

	//ɾ�����
	if(pPrepNode!=NULL)
	{
		if(pNextNode!=NULL)
		{
			pPrepNode->pNextDistributeNode=pNextNode;
			pNextNode->pPrepDistributeNode=pPrepNode;
		}
		else
		{
			//β�����
			if(pPrepNode->pNextDistributeNode==pDistributeNode)
			{
				pPrepNode->pNextDistributeNode=NULL;
			}
		}
	}
	else
	{
		if(pNextNode!=NULL)
		{
			pNextNode->pPrepDistributeNode=NULL;

			//���ñ�ͷ
			m_pHeadNode=pNextNode;
		}
		else
		{
			m_pHeadNode=NULL;
		}
	}

    //������Ŀ
	if(pDistributeNode->DistributeInfo.pIServerUserItem->IsAndroidUser())
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
	while(m_pHeadNode!=NULL)
	{
		RemoveDistributeNode(m_pHeadNode);
	}

	//���ñ���
	m_pHeadNode=NULL;
	m_wNodeCount=0;
	m_wAndroidCount=0;
	m_wRealCount=0;

	return;
}

//ִ�з���
WORD  CDistributeManager::PerformDistribute(CDistributeInfoArray & DistributeInfoArray,WORD wNeedCount)
{
	//�������
	tagDistributeNode  * pMoveNode=m_pHeadNode;
	if(pMoveNode!=NULL)
	{
		DistributeInfoArray.Add(pMoveNode->DistributeInfo);
		pMoveNode=pMoveNode->pNextDistributeNode;
	}

	//��ȡ�û�
	while(pMoveNode!=NULL)
	{
		//�������
		BOOL bFirstSuccess=TRUE;

		//�ȼ�����
		if(DistributeInfoArray.GetCount()>0 && DistributeInfoArray[0].wDistribute!=pMoveNode->DistributeInfo.wDistribute)
			bFirstSuccess=FALSE;

		//��������
		if(bFirstSuccess==TRUE && DistributeInfoArray.GetCount()==wNeedCount-1 && 
			FilterRuleIsAllAndroid(DistributeInfoArray,pMoveNode->DistributeInfo.pIServerUserItem))
			bFirstSuccess=FALSE;

		//ͬ������
		if(bFirstSuccess==TRUE && (m_cbDistributeRule&DISTRIBUTE_LAST_TABLE)==0 &&
			FilterRuleExitsSameTable(DistributeInfoArray,pMoveNode->DistributeInfo.wLastTableID)==TRUE)
			bFirstSuccess=FALSE;

		//ͬIP����
		if(bFirstSuccess==TRUE && (m_cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)==0 &&  
			FilterRuleExitsIPAddr(DistributeInfoArray,pMoveNode->DistributeInfo.pIServerUserItem->GetClientAddr())==TRUE)
			bFirstSuccess=FALSE;

		//��ȡ�ɹ�
		if(bFirstSuccess==TRUE) DistributeInfoArray.Add(pMoveNode->DistributeInfo);

		//��ǰ�ƽ�
		pMoveNode=pMoveNode->pNextDistributeNode;

		//�ɹ��ж�
		if(DistributeInfoArray.GetCount()==wNeedCount) break;
	}

	return (WORD)DistributeInfoArray.GetCount();
}

//���ҽ��
tagDistributeNode * CDistributeManager::SearchNode(const IServerUserItem  * const pIServerUserItem)
{
	if(m_pHeadNode==NULL) return NULL;

	//���ñ���
	tagDistributeNode * pMoveNode=m_pHeadNode;

	//���ҽ��
	while(pMoveNode!=NULL)
	{
		//�ӿ��ж�
		if(pMoveNode->DistributeInfo.pIServerUserItem==pIServerUserItem)
			return pMoveNode;

		//��ǰ�ƽ�
		pMoveNode=pMoveNode->pNextDistributeNode;
	}

	return NULL;
}

//IPַͬ
BOOL  CDistributeManager::FilterRuleExitsIPAddr(const CDistributeInfoArray & DistributeInfoArray,DWORD dwClientAddr)
{
	//����ͬIP
	for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		if(DistributeInfoArray[nIndex].pIServerUserItem->GetClientAddr()==dwClientAddr)
			return TRUE;
	}

	return FALSE;
}

//�Ͼ�ͬ��
BOOL  CDistributeManager::FilterRuleExitsSameTable(const CDistributeInfoArray & DistributeInfoArray,WORD wLastTableID)
{
	//����ͬIP
	for(INT_PTR nIndex=0; nIndex<DistributeInfoArray.GetCount(); nIndex++)
	{
		if(DistributeInfoArray[nIndex].wLastTableID == wLastTableID &&
			wLastTableID != INVALID_TABLE )
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
	for(INT_PTR nIndex=0; nIndex<DistributeInfoArray.GetCount(); nIndex++)
	{
		if(DistributeInfoArray[nIndex].pIServerUserItem->IsAndroidUser()==true)
			++wAndroidCount;
	}

	return (wAndroidCount==DistributeInfoArray.GetCount()) && pIServerUserItem->IsAndroidUser();
}

//////////////////////////////////////////////////////////////////////////////////
