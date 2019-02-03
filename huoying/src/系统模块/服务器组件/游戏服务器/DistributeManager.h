#ifndef DISTRIBUTE_MANAGER_HEAD_FILE
#define DISTRIBUTE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//分组数据
struct tagDistributeInfo
{
	WORD							wDistribute;						//分配等级
	WORD							wLastTableID;						//上局桌号	
	IServerUserItem *				pIServerUserItem;					//用户接口
	VOID *                          pPertainNode;                       //所属结点 
};

//分组结点
struct tagDistributeNode
{
	tagDistributeNode *             pPrepDistributeNode;               //前一结点
	tagDistributeNode *			    pNextDistributeNode;               //下一结点
	tagDistributeInfo               DistributeInfo;                    //分组信息
};  

//////////////////////////////////////////////////////////////////////////////////

typedef CWHArray<tagDistributeInfo> CDistributeInfoArray;				//分组数组

//////////////////////////////////////////////////////////////////////////////////

//结点池
class  CDistributeNodePool
{
	//管理变量
public:
	static const int BLOCK_SIZE;
	static tagDistributeNode * m_pHeadOfFreeList;

	//函数定义
public:
	//构造函数
	CDistributeNodePool();
	//析构函数
	~CDistributeNodePool();

	//功能函数
public:
	//分配结点
	tagDistributeNode * AllocNode();
	//释放结点
	VOID FreeNode(void * pNode);
};

//////////////////////////////////////////////////////////////////////////////////

//分组管理
class CDistributeManager
{
	//数目变量
protected:
	WORD							m_wNodeCount;				      //结点数目
	WORD                            m_wRealCount;	                  //玩家数目
	WORD                            m_wAndroidCount;		          //机器数目 

	//管理变量
protected:
	CDistributeNodePool             m_DistributeNodePool;             //结点池 
	tagDistributeNode *				m_pHeadNode;					  //头部结点

	//分组变量
protected:
	BYTE							m_cbDistributeRule;				  //分组规则

	//函数定义
public:
	//构造函数
	CDistributeManager();
	//析构函数
	virtual ~CDistributeManager();

	//内联函数
public:
	//结点数目
	inline WORD  GetCount() { return m_wNodeCount; }
	//真人数目
	inline WORD  GetRealCount() { return m_wRealCount; }
	//机器数目
	inline WORD  GetAndroidCount() { return m_wAndroidCount; }
	//设置规则
	inline VOID  SetDistributeRule(BYTE cbDistributeRule) { m_cbDistributeRule=cbDistributeRule; }

	//内部函数
protected:
	//查找结点
	tagDistributeNode* SearchNode(const IServerUserItem  * const pIServerUserItem);

	//功能函数
public:	
	//移除结点
	VOID RemoveAll();
	//移除结点
	VOID RemoveDistributeNode(tagDistributeNode * pDistributeNode);	
	//移除结点
	VOID RemoveDistributeNode(const IServerUserItem  *  pIServerUserItem);
	//插入结点
	BOOL InsertDistributeNode(const tagDistributeInfo & DistributeInfo);	
	//执行分组
	WORD PerformDistribute(CDistributeInfoArray & DistributeInfoArray,WORD wNeedCount);

	//过滤函数
protected:
	//IP同址
	inline BOOL  FilterRuleExitsIPAddr(const CDistributeInfoArray & DistributeInfoArray,DWORD dwClientAddr);	
	//上局同桌
	inline BOOL  FilterRuleExitsSameTable(const CDistributeInfoArray & DistributeInfoArray,WORD wLastTableID);	
	//机器过滤
	inline BOOL  FilterRuleIsAllAndroid(const CDistributeInfoArray & DistributeInfoArray, IServerUserItem  * const pIServerUserItem);
};

//////////////////////////////////////////////////////////////////////////////////

#endif