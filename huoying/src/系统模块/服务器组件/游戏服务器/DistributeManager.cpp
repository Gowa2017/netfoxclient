#include "StdAfx.h"
#include "DistributeManager.h"

//////////////////////////////////////////////////////////////////////////////////

//静态成员
tagDistributeNode * CDistributeNodePool::m_pHeadOfFreeList=NULL;

//////////////////////////////////////////////////////////////////////////////////
//常量定义
const int CDistributeNodePool::BLOCK_SIZE = 40;

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDistributeNodePool::CDistributeNodePool()
{
}

//析构函数
CDistributeNodePool::~CDistributeNodePool()
{
	//释放列表
	if(m_pHeadOfFreeList!=NULL)
	{
		tagDistributeNode * pDistributeNode=m_pHeadOfFreeList;
		while(pDistributeNode!=NULL)
		{
			//安全释放
			m_pHeadOfFreeList = pDistributeNode->pNextDistributeNode;
			SafeDelete(pDistributeNode);
			pDistributeNode=m_pHeadOfFreeList;
		}
	}
}

//分配结点
tagDistributeNode * CDistributeNodePool::AllocNode()
{
	//获取头结点
	tagDistributeNode *pDistributeNode = m_pHeadOfFreeList; 
	if(pDistributeNode!=NULL)
	{
		m_pHeadOfFreeList = pDistributeNode->pNextDistributeNode;
	}
	else 
	{
		//分配内存
		for(int nIndex = 0; nIndex < BLOCK_SIZE; nIndex++)
		{
			tagDistributeNode * pNewBlock = new tagDistributeNode;
			pNewBlock->pNextDistributeNode=m_pHeadOfFreeList;
			m_pHeadOfFreeList=pNewBlock;
		}
		
		//设置结点
		pDistributeNode = m_pHeadOfFreeList;
		m_pHeadOfFreeList=pDistributeNode->pNextDistributeNode;
	}

	//初始节点
	ZeroMemory(pDistributeNode,sizeof(tagDistributeNode));

	return pDistributeNode;
}

//释放结点
VOID CDistributeNodePool::FreeNode(void * pNode)
{
	//归还结点
	tagDistributeNode * pDeadNode =  static_cast<tagDistributeNode*>(pNode);
	pDeadNode->pNextDistributeNode = m_pHeadOfFreeList;
	m_pHeadOfFreeList = pDeadNode;

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDistributeManager::CDistributeManager()
{
	//设置变量
	m_pHeadNode=NULL;
	m_wNodeCount=0;	
	m_wRealCount=0;
	m_wAndroidCount=0;
	m_cbDistributeRule=0;
}

//析构函数
CDistributeManager::~CDistributeManager()
{
	//移除节点
	RemoveAll();
}

//插入结点
BOOL CDistributeManager::InsertDistributeNode(const tagDistributeInfo & DistributeInfo)
{
	//查找用户
	if(SearchNode(DistributeInfo.pIServerUserItem)!=NULL) return false;

	//头部判断
	if(m_pHeadNode==NULL)
	{
		//分配结点
		m_pHeadNode = m_DistributeNodePool.AllocNode();
		if(m_pHeadNode==NULL) return false;

		//设置变量
		CopyMemory(&m_pHeadNode->DistributeInfo,&DistributeInfo,sizeof(DistributeInfo));
		m_pHeadNode->DistributeInfo.pPertainNode=m_pHeadNode;
	}
	else
	{
		//分配结点
		tagDistributeNode * pDistributeNode = m_DistributeNodePool.AllocNode();
		if(pDistributeNode==NULL) return false;

		//设置结点
		CopyMemory(&pDistributeNode->DistributeInfo,&DistributeInfo,sizeof(DistributeInfo));
		pDistributeNode->DistributeInfo.pPertainNode=pDistributeNode;
		
		//插入结点
		if(m_pHeadNode->pNextDistributeNode!=NULL)
		{
			m_pHeadNode->pNextDistributeNode->pPrepDistributeNode=pDistributeNode;
			pDistributeNode->pNextDistributeNode=m_pHeadNode->pNextDistributeNode;
		}
		
		//链接结点
		pDistributeNode->pPrepDistributeNode=m_pHeadNode;
		m_pHeadNode->pNextDistributeNode=pDistributeNode;
	}

	//更新数目
	if(DistributeInfo.pIServerUserItem->IsAndroidUser())
		++m_wAndroidCount;
	else
		++m_wRealCount;
	++m_wNodeCount;

	return true;
}

//移除结点
VOID  CDistributeManager::RemoveDistributeNode(const IServerUserItem  * pIServerUserItem)
{
	//查找结点
	tagDistributeNode  *pDistributeNode=SearchNode(pIServerUserItem);
	if(pDistributeNode!=NULL) RemoveDistributeNode(pDistributeNode);

	return;
}

//移除结点
VOID  CDistributeManager::RemoveDistributeNode(tagDistributeNode * pDistributeNode)
{
	//参数校验
	if(pDistributeNode==NULL) return;

	//变量定义
	tagDistributeNode  *pPrepNode=pDistributeNode->pPrepDistributeNode;
	tagDistributeNode  *pNextNode=pDistributeNode->pNextDistributeNode;

	//删除结点
	if(pPrepNode!=NULL)
	{
		if(pNextNode!=NULL)
		{
			pPrepNode->pNextDistributeNode=pNextNode;
			pNextNode->pPrepDistributeNode=pPrepNode;
		}
		else
		{
			//尾部结点
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

			//重置表头
			m_pHeadNode=pNextNode;
		}
		else
		{
			m_pHeadNode=NULL;
		}
	}

    //更新数目
	if(pDistributeNode->DistributeInfo.pIServerUserItem->IsAndroidUser())
		--m_wAndroidCount;
	else
		--m_wRealCount;
	--m_wNodeCount;

	//安全释放
	m_DistributeNodePool.FreeNode(pDistributeNode);
}

//移除结点
VOID CDistributeManager::RemoveAll()
{
	//释放内存
	while(m_pHeadNode!=NULL)
	{
		RemoveDistributeNode(m_pHeadNode);
	}

	//重置变量
	m_pHeadNode=NULL;
	m_wNodeCount=0;
	m_wAndroidCount=0;
	m_wRealCount=0;

	return;
}

//执行分组
WORD  CDistributeManager::PerformDistribute(CDistributeInfoArray & DistributeInfoArray,WORD wNeedCount)
{
	//定义变量
	tagDistributeNode  * pMoveNode=m_pHeadNode;
	if(pMoveNode!=NULL)
	{
		DistributeInfoArray.Add(pMoveNode->DistributeInfo);
		pMoveNode=pMoveNode->pNextDistributeNode;
	}

	//获取用户
	while(pMoveNode!=NULL)
	{
		//定义变量
		BOOL bFirstSuccess=TRUE;

		//等级过滤
		if(DistributeInfoArray.GetCount()>0 && DistributeInfoArray[0].wDistribute!=pMoveNode->DistributeInfo.wDistribute)
			bFirstSuccess=FALSE;

		//机器过滤
		if(bFirstSuccess==TRUE && DistributeInfoArray.GetCount()==wNeedCount-1 && 
			FilterRuleIsAllAndroid(DistributeInfoArray,pMoveNode->DistributeInfo.pIServerUserItem))
			bFirstSuccess=FALSE;

		//同桌过滤
		if(bFirstSuccess==TRUE && (m_cbDistributeRule&DISTRIBUTE_LAST_TABLE)==0 &&
			FilterRuleExitsSameTable(DistributeInfoArray,pMoveNode->DistributeInfo.wLastTableID)==TRUE)
			bFirstSuccess=FALSE;

		//同IP过滤
		if(bFirstSuccess==TRUE && (m_cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)==0 &&  
			FilterRuleExitsIPAddr(DistributeInfoArray,pMoveNode->DistributeInfo.pIServerUserItem->GetClientAddr())==TRUE)
			bFirstSuccess=FALSE;

		//获取成功
		if(bFirstSuccess==TRUE) DistributeInfoArray.Add(pMoveNode->DistributeInfo);

		//向前推进
		pMoveNode=pMoveNode->pNextDistributeNode;

		//成功判断
		if(DistributeInfoArray.GetCount()==wNeedCount) break;
	}

	return (WORD)DistributeInfoArray.GetCount();
}

//查找结点
tagDistributeNode * CDistributeManager::SearchNode(const IServerUserItem  * const pIServerUserItem)
{
	if(m_pHeadNode==NULL) return NULL;

	//设置变量
	tagDistributeNode * pMoveNode=m_pHeadNode;

	//查找结点
	while(pMoveNode!=NULL)
	{
		//接口判断
		if(pMoveNode->DistributeInfo.pIServerUserItem==pIServerUserItem)
			return pMoveNode;

		//向前推进
		pMoveNode=pMoveNode->pNextDistributeNode;
	}

	return NULL;
}

//IP同址
BOOL  CDistributeManager::FilterRuleExitsIPAddr(const CDistributeInfoArray & DistributeInfoArray,DWORD dwClientAddr)
{
	//查找同IP
	for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		if(DistributeInfoArray[nIndex].pIServerUserItem->GetClientAddr()==dwClientAddr)
			return TRUE;
	}

	return FALSE;
}

//上局同桌
BOOL  CDistributeManager::FilterRuleExitsSameTable(const CDistributeInfoArray & DistributeInfoArray,WORD wLastTableID)
{
	//查找同IP
	for(INT_PTR nIndex=0; nIndex<DistributeInfoArray.GetCount(); nIndex++)
	{
		if(DistributeInfoArray[nIndex].wLastTableID == wLastTableID &&
			wLastTableID != INVALID_TABLE )
			return TRUE;
	}

	return FALSE;
}

//机器过滤
BOOL CDistributeManager::FilterRuleIsAllAndroid(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem  * const pIServerUserItem)
{
	//参数校验
	if(pIServerUserItem==NULL || DistributeInfoArray.GetCount()==0) return FALSE;

	//变量定义
	WORD wAndroidCount=0;

	//统计机器
	for(INT_PTR nIndex=0; nIndex<DistributeInfoArray.GetCount(); nIndex++)
	{
		if(DistributeInfoArray[nIndex].pIServerUserItem->IsAndroidUser()==true)
			++wAndroidCount;
	}

	return (wAndroidCount==DistributeInfoArray.GetCount()) && pIServerUserItem->IsAndroidUser();
}

//////////////////////////////////////////////////////////////////////////////////
